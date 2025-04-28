#include <QtWidgets>
#include "GridRemover.h"
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <limits>

using namespace std;

// 상수화된 매직 넘버: 휴리스틱 제거 단계의 가중치와 영역 제한
static constexpr double WEIGHT_AREA = 4.55779;
static constexpr double WEIGHT_CENTER = -6.89869;
static constexpr double WEIGHT_PREV = 5.32938;
static constexpr int    MIN_AREA = 2;
static constexpr int    MAX_AREA = 10;

GridRemover::GridRemover()
    : grid(ROWS, vector<int>(COLS, 0)),
    prefixSum(ROWS + 1, vector<int>(COLS + 1, 0)),
    prefixCount(ROWS + 1, vector<int>(COLS + 1, 0)),
    totalCellsRemoved(0)
{
    // 그리드의 기본 값은 0이며, 이전 제거 기준을 그리드 중앙으로 초기화
    prevX = ROWS / 2.0;
    prevY = COLS / 2.0;
}

void GridRemover::setGrid(const vector<vector<int>>& newGrid) {
    grid = newGrid;
    updatePrefixData();
}

const vector<vector<int>>& GridRemover::getGrid() const {
    return grid;
}

void GridRemover::updatePrefixData() {
    // prefixSum과 prefixCount 배열을 모두 초기화
    for (int i = 0; i <= ROWS; ++i)
        for (int j = 0; j <= COLS; ++j) {
            prefixSum[i][j] = 0;
            prefixCount[i][j] = 0;
        }
    // prefixSum 계산
    for (int i = 1; i <= ROWS; ++i) {
        for (int j = 1; j <= COLS; ++j) {
            prefixSum[i][j] = grid[i - 1][j - 1]
                + prefixSum[i - 1][j]
                + prefixSum[i][j - 1]
                - prefixSum[i - 1][j - 1];
        }
    }
    // prefixCount 계산 (0이 아닌 셀 개수)
    for (int i = 1; i <= ROWS; ++i) {
        for (int j = 1; j <= COLS; ++j) {
            prefixCount[i][j] = (grid[i - 1][j - 1] != 0 ? 1 : 0)
                + prefixCount[i - 1][j]
                + prefixCount[i][j - 1]
                - prefixCount[i - 1][j - 1];
        }
    }
}

int GridRemover::sumRegion(int x1, int y1, int x2, int y2) {
    // (x1, y1)부터 (x2, y2)까지의 영역 합을 prefix sum을 사용해 계산
    return prefixSum[x2 + 1][y2 + 1]
        - prefixSum[x1][y2 + 1]
        - prefixSum[x2 + 1][y1]
        + prefixSum[x1][y1];
}

int GridRemover::countNonZero(int x1, int y1, int x2, int y2) {
    // (x1, y1)부터 (x2, y2)까지의 0이 아닌 셀 개수 계산
    return prefixCount[x2 + 1][y2 + 1]
        - prefixCount[x1][y2 + 1]
        - prefixCount[x2 + 1][y1]
        + prefixCount[x1][y1];
}

void GridRemover::removeRegion(int x1, int y1, int x2, int y2) {
    // 지정 영역의 셀을 0으로 변경한 후 prefix 데이터를 갱신
    for (int i = x1; i <= x2; i++) {
        for (int j = y1; j <= y2; j++) {
            grid[i][j] = 0;
        }
    }
    updatePrefixData();
}

int GridRemover::heuristicRemovalStep() {
    double bestScore = -numeric_limits<double>::infinity();
    int bestX1 = -1, bestY1 = -1, bestX2 = -1, bestY2 = -1;
    int bestCandidateCount = 0;
    double bestCenterX = 0.0, bestCenterY = 0.0;

    double gridCenterX = ROWS / 2.0;
    double gridCenterY = COLS / 2.0;

    // 모든 후보 영역을 순회 (영역의 면적은 MIN_AREA ~ MAX_AREA)
    for (int x1 = 0; x1 < ROWS; x1++) {
        for (int y1 = 0; y1 < COLS; y1++) {
            for (int x2 = x1; x2 < ROWS && (x2 - x1 + 1) <= MAX_AREA; x2++) {
                for (int y2 = y1; y2 < COLS && (y2 - y1 + 1) <= MAX_AREA; y2++) {
                    int area = (x2 - x1 + 1) * (y2 - y1 + 1);
                    if (area < MIN_AREA || area > MAX_AREA) continue;

                    // 후보 영역의 합이 10인지 확인
                    if (sumRegion(x1, y1, x2, y2) != 10)
                        continue;

                    // 후보 영역 내 0이 아닌 셀이 존재해야 함
                    int cnt = countNonZero(x1, y1, x2, y2);
                    if (cnt <= 0)
                        continue;

                    // 후보 영역 중심 좌표 계산
                    double candidateCenterX = (x1 + x2) / 2.0;
                    double candidateCenterY = (y1 + y2) / 2.0;

                    double f_area = -area;
                    double f_center = sqrt(pow(candidateCenterX - gridCenterX, 2) +
                        pow(candidateCenterY - gridCenterY, 2));
                    double f_prev = sqrt(pow(candidateCenterX - prevX, 2) +
                        pow(candidateCenterY - prevY, 2));

                    double score = WEIGHT_AREA * f_area
                        + WEIGHT_CENTER * f_center
                        + WEIGHT_PREV * f_prev;

                    if (score > bestScore) {
                        bestScore = score;
                        bestX1 = x1; bestY1 = y1; bestX2 = x2; bestY2 = y2;
                        bestCandidateCount = cnt;
                        bestCenterX = candidateCenterX;
                        bestCenterY = candidateCenterY;
                    }
                }
            }
        }
    }

    // 유효한 후보 영역이 존재하면 해당 영역을 제거하고, 제거 단계를 기록
    if (bestScore > -numeric_limits<double>::infinity()) {
        removeRegion(bestX1, bestY1, bestX2, bestY2);
        totalCellsRemoved += bestCandidateCount;

        prevX = bestCenterX;
        prevY = bestCenterY;

        RemovalStep step = { bestX1, bestY1, bestX2, bestY2 };
        removalPath.push_back(step);
        return bestCandidateCount;
    }
    return 0; // 제거 가능한 후보 영역이 없으면 0 반환
}

void GridRemover::performHeuristicRemovals() {
    while (true) {
        int removed = heuristicRemovalStep();
        if (removed == 0)
            break;
    }
}
