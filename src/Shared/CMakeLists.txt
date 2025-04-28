set(SHARED_SRC
    AreaCaptureHelper.cpp
    HotKeyManager.cpp
    OverlayHelper.cpp
    RegionSelector.cpp
)

set(SHARED_HEADERS
    ${CMAKE_SOURCE_DIR}/include/Shared/AreaCaptureHelper.h
    ${CMAKE_SOURCE_DIR}/include/Shared/HotKeyManager.h
    ${CMAKE_SOURCE_DIR}/include/Shared/OverlayHelper.h
    ${CMAKE_SOURCE_DIR}/include/Shared/RegionSelector.h
    ${CMAKE_SOURCE_DIR}/include/Shared/Types.h
)

add_library(SharedLib STATIC ${SHARED_SRC} ${SHARED_HEADERS})

target_include_directories(SharedLib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/Shared>
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(SharedLib
    PUBLIC
        Qt6::Core
    PRIVATE
        Qt6::Widgets
        Qt6::Gui
)

if(WIN32)
    target_link_libraries(SharedLib PRIVATE user32)
endif()