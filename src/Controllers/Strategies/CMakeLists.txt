# src/controllers/strategies/CMakeLists.txt

# strategies 라이브러리 정의
add_library(StrategiesLib
    NormalMode.cpp
    ScoreMode.cpp
    DataCollectionMode.cpp
    ${CMAKE_SOURCE_DIR}/include/Controllers/Strategies/ModeStrategy.h
    ${CMAKE_SOURCE_DIR}/include/Controllers/Strategies/NormalMode.h
    ${CMAKE_SOURCE_DIR}/include/Controllers/Strategies/ScoreMode.h
    ${CMAKE_SOURCE_DIR}/include/Controllers/Strategies/DataCollectionMode.h
)

# 의존성 링크
target_link_libraries(StrategiesLib
    PUBLIC
        ControllersLib  # 상위 모듈
        SharedLib
    PRIVATE
        Qt6::Widgets
        Qt6::Gui
        Qt6::Svg
        ${OpenCV_LIBS}
)

# 포함 경로 설정
target_include_directories(StrategiesLib
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include/Controllers
        ${CMAKE_SOURCE_DIR}/include/Controllers/Strategies
        ${CMAKE_SOURCE_DIR}/include/Core
        ${CMAKE_SOURCE_DIR}/include/Shared
        ${CMAKE_SOURCE_DIR}/include/Tabs
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
)