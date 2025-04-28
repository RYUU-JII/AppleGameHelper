set(CORE_SRC
    GridRemover.cpp
    ImageProcessor.cpp
    AutoWorker.cpp
    OCRWorker.cpp
)

set(CORE_HEADERS
    ${CMAKE_SOURCE_DIR}/include/Core/GridRemover.h
    ${CMAKE_SOURCE_DIR}/include/Core/ImageProcessor.h
    ${CMAKE_SOURCE_DIR}/include/Core/AppState.h
    ${CMAKE_SOURCE_DIR}/include/Core/AutoWorker.h
    ${CMAKE_SOURCE_DIR}/include/Core/OCRWorker.h
)

add_library(CoreLib ${CORE_SRC} ${CORE_HEADERS})

target_include_directories(CoreLib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/Controllers>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/Core>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/Shared>

)

target_link_libraries(CoreLib
    PUBLIC
        ${OpenCV_LIBS}
        Tesseract::libtesseract
    PRIVATE
        Qt6::Core Qt6::Widgets
)