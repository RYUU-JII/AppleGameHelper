add_library(UILib
    MainTab.cpp
    ${CMAKE_SOURCE_DIR}/include/Tabs/MainTab.h
)

# 의존 라이브러리 링크
target_link_libraries(UILib
    PUBLIC
        ControllersLib
        StrategiesLib
        SharedLib
    PRIVATE
        Qt6::Widgets
        Qt6::Gui
        Qt6::Svg
)

# 포함 경로 설정
target_include_directories(UILib
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/Tabs
        ${CMAKE_SOURCE_DIR}/include/Controllers
        ${CMAKE_SOURCE_DIR}/include/Controllers/Strategies
        ${CMAKE_SOURCE_DIR}/include/Shared
        ${CMAKE_SOURCE_DIR}/include/Core
        ${OpenCV_INCLUDE_DIRS}
        ${Tesseract_INCLUDE_DIRS}
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
)