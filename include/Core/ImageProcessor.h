// ImageProcessor.h
#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <windows.h>
#include <QtWidgets>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <tesseract/baseapi.h>
#include <tesseract/capi.h>
#include <functional>
#include "CancellationToken.h"

struct OCRResult {
    std::string methodName;
    cv::Mat processedImage;
    std::string recognizedText;
    int confidence;
};

class ImageProcessor {
public:
    ImageProcessor();
    ~ImageProcessor() = default;

    std::vector<std::vector<int>> performOCR(
        const cv::Mat& inputImage,
        tesseract::TessBaseAPI& api,
        std::function<void(int)> onProgress
    );

    static cv::Mat qImageToMat(const QImage& image);
    bool quickValidateOCR(const cv::Mat& img, tesseract::TessBaseAPI& api) noexcept;

    const std::vector<std::vector<std::vector<OCRResult>>>& getOcrResultCache() const { return ocrResultCache; }

private:
    std::vector<std::vector<std::vector<OCRResult>>> ocrResultCache;

    cv::Mat preprocessImage(const cv::Mat& img);
    cv::Mat extractCell(const cv::Mat& preprocessed, int row, int col);
    std::vector<OCRResult> performOCROnCell(const cv::Mat& cell, tesseract::TessBaseAPI& api);
    OCRResult selectBestResult(const std::vector<OCRResult>& candidates);

    using PreprocessFunction = std::function<cv::Mat(const cv::Mat&)>;

    std::vector<std::pair<std::string, PreprocessFunction>> preprocessMethods;
};

#endif // IMAGEPROCESSOR_H