#include <windows.h>
#include <QtWidgets>
#include "ImageProcessor.h"
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <tesseract/capi.h>
#include <QDebug>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <limits>
#include <functional>

using namespace cv;
using namespace std;

namespace {
    cv::Mat autoCrop(const cv::Mat& img) {
        cv::Mat binary;
        cv::threshold(img, binary, 250, 255, cv::THRESH_BINARY_INV);
        vector<vector<Point>> contours;
        cv::findContours(binary, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        if (contours.empty()) return img;

        cv::Rect bbox = cv::boundingRect(contours[0]);
        for (size_t i = 1; i < contours.size(); i++) {
            bbox |= cv::boundingRect(contours[i]);
        }
        if (bbox.width < 5 || bbox.height < 5) return img;
        return img(bbox).clone();
    }

    cv::Mat extractDynamicROI(const cv::Mat& cell) {
        cv::Mat binary;
        cv::threshold(cell, binary, 200, 255, cv::THRESH_BINARY);
        vector<vector<Point>> contours;
        cv::findContours(binary, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        if (contours.empty()) return cell;

        double maxArea = 0.0;
        int bestIdx = -1;
        for (size_t i = 0; i < contours.size(); ++i) {
            double area = cv::contourArea(contours[i]);
            if (area > maxArea) {
                maxArea = area;
                bestIdx = i;
            }
        }
        if (bestIdx == -1) return cell;

        cv::Rect roi = cv::boundingRect(contours[bestIdx]);
        return cell(roi).clone();
    }   
}

cv::Mat ImageProcessor::qImageToMat(const QImage& image) {
    // QImage를 BGR 포맷의 cv::Mat으로 변환
    QImage img = image.convertToFormat(QImage::Format_RGB32); // ARGB → RGB
    cv::Mat mat(img.height(), img.width(), CV_8UC4, const_cast<uchar*>(img.bits()), img.bytesPerLine());
    cv::Mat result;
    cv::cvtColor(mat, result, cv::COLOR_BGRA2BGR); // BGRA → BGR
    return result;
}

ImageProcessor::ImageProcessor() {
    preprocessMethods = {
        {"Original", [](const cv::Mat& img) { return img.clone(); }},
        {"Adjusted", [](const cv::Mat& img) {
            cv::Mat adjusted;
            img.convertTo(adjusted, -1, 1.2, 30);
            return adjusted;
        }},
        {"SimpleThresh", [](const cv::Mat& img) {
            cv::Mat thresh;
            cv::threshold(img, thresh, 150, 255, cv::THRESH_BINARY);
            return thresh;
        }},
        {"Scaled", [](const cv::Mat& img) {
            cv::Mat scaled;
            cv::resize(img, scaled, cv::Size(), 2.0, 2.0, cv::INTER_LINEAR);
            return scaled;
        }},
        {"MildErosion", [](const cv::Mat& img) {
            cv::Mat eroded;
            cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
            cv::erode(img, eroded, kernel, cv::Point(-1, -1), 1);
            return eroded;
        }},
        {"DynamicROI", [](const cv::Mat& img) {
            return extractDynamicROI(img);
        }}
    };
}
cv::Mat ImageProcessor::preprocessImage(const cv::Mat& img) {
    Mat gray;
    if (img.channels() != 1) {
        cvtColor(img, gray, COLOR_BGR2GRAY);
    }
    else {
        gray = img;
    }
    Mat preprocessed;
    medianBlur(gray, preprocessed, 3);
    Ptr<CLAHE> clahe = createCLAHE(2.0, Size(8, 8));
    clahe->apply(preprocessed, preprocessed);
    return preprocessed;
}

cv::Mat ImageProcessor::extractCell(const cv::Mat& preprocessed, int row, int col) {
    const int rows = 10, cols = 17;
    double dcellWidth = static_cast<double>(preprocessed.cols) / cols;
    double dcellHeight = static_cast<double>(preprocessed.rows) / rows;

    int x = static_cast<int>(round(col * dcellWidth));
    int y = static_cast<int>(round(row * dcellHeight));
    int w = static_cast<int>(round((col + 1) * dcellWidth)) - x;
    int h = static_cast<int>(round((row + 1) * dcellHeight)) - y;

    x = max(0, x);
    y = max(0, y);
    if (x + w > preprocessed.cols) w = preprocessed.cols - x;
    if (y + h > preprocessed.rows) h = preprocessed.rows - y;

    Rect cellRect(x, y, w, h);
    return autoCrop(preprocessed(cellRect));
}

std::vector<OCRResult> ImageProcessor::performOCROnCell(const cv::Mat& cell, tesseract::TessBaseAPI& api) {
    std::vector<OCRResult> results;
    for (const auto& [name, method] : preprocessMethods) {
        cv::Mat processed = method(cell);
        api.SetImage(processed.data, processed.cols, processed.rows, 1, processed.step);
        api.Recognize(0);
        char* text = api.GetUTF8Text();
        std::string recognizedText = (text ? string(text) : "");
        if (text) TessDeleteText(text);
        int confidence = static_cast<int>(api.MeanTextConf());
        api.Clear();
        results.push_back({ name, processed, recognizedText, confidence });
    }
    return results;
}

OCRResult ImageProcessor::selectBestResult(const std::vector<OCRResult>& candidates) {
    // Group candidates by recognized digit
    unordered_map<char, pair<int, int>> freq;  // {digit, {count, sumConfidence}}
    unordered_map<char, OCRResult> bestPerDigit;
    for (const auto& cand : candidates) {
        if (!cand.recognizedText.empty() && cand.recognizedText[0] != '0' && isdigit(cand.recognizedText[0]) && cand.confidence >= 65) {
            char digit = cand.recognizedText[0];
            freq[digit].first++;
            freq[digit].second += cand.confidence;
            if (bestPerDigit[digit].confidence < cand.confidence) {
                bestPerDigit[digit] = cand;
            }
        }
    }

    OCRResult bestCandidate;
    bool found = false;
    double bestWeightedScore = 0.0;
    double beta = 0.2;
    for (const auto& kv : freq) {
        int count = kv.second.first;
        int sumConf = kv.second.second;
        double avgConf = static_cast<double>(sumConf) / count;
        double weightedScore = avgConf * pow(count, beta);
        if (!found || weightedScore > bestWeightedScore) {
            bestWeightedScore = weightedScore;
            bestCandidate = bestPerDigit[kv.first];
            found = true;
        }
    }
    if (!found) {
        int maxConf = 0;
        for (const auto& cand : candidates) {
            if (!cand.recognizedText.empty() && isdigit(cand.recognizedText[0]) && cand.confidence > maxConf) {
                maxConf = cand.confidence;
                bestCandidate = cand;
            }
        }
    }

    // '3'에 대한 특수 처리
    if (!bestCandidate.recognizedText.empty() && bestCandidate.recognizedText[0] == '3') {
        for (const auto& cand : candidates) {
            if (cand.methodName == "MildErosion" && !cand.recognizedText.empty() && cand.recognizedText[0] == '8') {
                bestCandidate = cand;
                break;
            }
            else if (cand.methodName == "Adjusted" && !cand.recognizedText.empty() && cand.recognizedText[0] == '6') {
                bestCandidate = cand;
                break;
            }
        }
    }

    if (bestCandidate.recognizedText.empty()) {
        bestCandidate.recognizedText = "X";
        bestCandidate.confidence = 0;
    }
    return bestCandidate;
}

std::vector<std::vector<int>> ImageProcessor::performOCR(
    const cv::Mat& inputImage,
    tesseract::TessBaseAPI& api,
    std::function<void(int)> onProgress
) {
    api.SetVariable("tessedit_char_whitelist", "0123456789");
    api.SetPageSegMode(tesseract::PSM_SINGLE_CHAR);

    cv::Mat preprocessed = preprocessImage(inputImage);
    const int rows = 10, cols = 17;
    ocrResultCache.resize(rows, vector<vector<OCRResult>>(cols));
    vector<vector<int>> numberGrid(rows, vector<int>(cols, 0));

    int totalCells = rows * cols, processedCells = 0;
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {

            cv::Mat cell = extractCell(preprocessed, r, c);
            vector<OCRResult> results = performOCROnCell(cell, api);
            ocrResultCache[r][c] = results;
            OCRResult best = selectBestResult(results);
            numberGrid[r][c] = (isdigit(best.recognizedText[0])) ? best.recognizedText[0] - '0' : 0;

            processedCells++;
            if (onProgress) onProgress((processedCells * 100) / totalCells);
        }
    }
    return numberGrid;
}

bool ImageProcessor::quickValidateOCR(const cv::Mat& img, tesseract::TessBaseAPI& api) noexcept {
    // 1) 그레이스케일로 변환
    cv::Mat gray;
    if (img.channels() == 1) {
        gray = img;
    }
    else {
        cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    }

    api.SetVariable("tessedit_char_whitelist", "0123456789");
    api.SetPageSegMode(tesseract::PSM_SINGLE_CHAR);

    const int rows = 10, cols = 17;
    double dcellWidth = static_cast<double>(gray.cols) / cols;
    double dcellHeight = static_cast<double>(gray.rows) / rows;

    // 시험용 셀 좌표
    vector<pair<int, int>> testCells = {
        {0,0}, {cols - 1,0}, {cols - 1,rows - 1}, {0,rows - 1}, {cols / 2, rows / 2}
    };

    for (auto [c, r] : testCells) {
        int x = int(round(c * dcellWidth));
        int y = int(round(r * dcellHeight));
        int w = int(round((c + 1) * dcellWidth)) - x;
        int h = int(round((r + 1) * dcellHeight)) - y;
        x = max(0, x);  y = max(0, y);
        if (x + w > gray.cols)  w = gray.cols - x;
        if (y + h > gray.rows)  h = gray.rows - y;
        if (w <= 0 || h <= 0)    return false;

        // 2) autoCrop에 그레이셀만 넘김
        cv::Mat cellGray = autoCrop(gray(cv::Rect(x, y, w, h)));
        api.SetImage(cellGray.data, cellGray.cols, cellGray.rows, 1, cellGray.step);
        string txt = string(api.GetUTF8Text());
        if (txt.empty() || !any_of(txt.begin(), txt.end(), ::isdigit))
            return false;
    }
    return true;
}
