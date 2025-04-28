# src/main/CMakeLists.txt

# Qt 리소스 파일 추가
qt_add_resources(RESOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/Resources/resources.qrc
)

# Main 소스와 헤더 파일 지정
set(MAIN_SRC
    AppleGameHelper.cpp
)

set(MAIN_HEADERS
    ${CMAKE_SOURCE_DIR}/include/Main/AppleGameHelper.h
)

# 라이브러리 정의 (AppleGameHelper 관련 코드)
add_library(MainLib STATIC
    ${MAIN_SRC}
    ${MAIN_HEADERS}
    ${RESOURCE_FILES}
)

# MainLib 의존성 및 포함 경로
target_include_directories(MainLib
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/Controllers
        ${CMAKE_SOURCE_DIR}/include/Controllers/Strategies
        ${CMAKE_SOURCE_DIR}/include/Shared
        ${CMAKE_SOURCE_DIR}/include/Tabs
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(MainLib
    PUBLIC
        ControllersLib
        StrategiesLib
        SharedLib
        UILib
    PRIVATE
        Qt6::Widgets
        Qt6::Gui
        Qt6::Svg
)
