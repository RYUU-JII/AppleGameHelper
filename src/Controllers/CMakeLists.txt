set(CONTROLLERS_SRC
    AppController.cpp
    CaptureManager.cpp
    OCRManager.cpp
    AutomationManager.cpp

    Strategies/NormalMode.cpp
    Strategies/ScoreMode.cpp
    Strategies/DataCollectionMode.cpp
)

set(CONTROLLERS_HEADERS
    ${CMAKE_SOURCE_DIR}/include/Controllers/AppController.h
    ${CMAKE_SOURCE_DIR}/include/Controllers/CaptureManager.h
    ${CMAKE_SOURCE_DIR}/include/Controllers/OCRManager.h
    ${CMAKE_SOURCE_DIR}/include/Controllers/AutomationManager.h

    ${CMAKE_SOURCE_DIR}/include/Controllers/Strategies/NormalMode.h
    ${CMAKE_SOURCE_DIR}/include/Controllers/Strategies/ScoreMode.h
    ${CMAKE_SOURCE_DIR}/include/Controllers/Strategies/DataCollectionMode.h
    ${CMAKE_SOURCE_DIR}/include/Controllers/Strategies/ModeStrategy.h
)

# 라이브러리 생성
add_library(ControllersLib
    ${CONTROLLERS_SRC}
    ${CONTROLLERS_HEADERS}
)

# Qt MOC 활성화
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# include 디렉토리 설정
target_include_directories(ControllersLib
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/Core
        ${CMAKE_SOURCE_DIR}/include/Controllers
        ${CMAKE_SOURCE_DIR}/include/Controllers/Strategies
)

# 연결할 라이브러리
target_link_libraries(ControllersLib
    PUBLIC
        ${OpenCV_LIBS}
        Tesseract::libtesseract
        SharedLib
        CoreLib
    PRIVATE
        Qt6::Core Qt6::Widgets
)