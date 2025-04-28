#ifndef GRID_REMOVER_H
#define GRID_REMOVER_H

#include <windows.h>
#include <vector>
#include "Types.h"

// 그리드의 행/열 크기 (필요에 따라 변경)
static const int ROWS = 10;
static const int COLS = 17;

class GridRemover {
public:
    GridRemover();

    // 그리드를 설정하면 prefix 데이터도 갱신
    void setGrid(const std::vector<std::vector<int>>& newGrid);
    const std::vector<std::vector<int>>& getGrid() const;

    // 후보 영역 제거를 가능한 한 번에 수행
    void performHeuristicRemovals();

    // 제거 경로(각 단계의 영역)를 가져오기 위한 getter
    const std::vector<RemovalStep>& getRemovalPath() const { return removalPath; }
    std::vector<RemovalStep> removalPath;

    // 전체 제거된 셀의 수
    int totalCellsRemoved;

private:
    std::vector<std::vector<int>> grid;
    std::vector<std::vector<int>> prefixSum;
    std::vector<std::vector<int>> prefixCount;

    // 이전 제거 단계 기준(중심 좌표)
    double prevX, prevY;

    // 내부 유틸리티 함수
    void updatePrefixData();
    int sumRegion(int x1, int y1, int x2, int y2);
    int countNonZero(int x1, int y1, int x2, int y2);
    void removeRegion(int x1, int y1, int x2, int y2);
    int heuristicRemovalStep();
};

#endif // GRID_REMOVER_H
