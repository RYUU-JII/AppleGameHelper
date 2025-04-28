cmake_minimum_required(VERSION 3.16)
project(AppleGameHelper LANGUAGES CXX)

# ---- vcpkg toolchain configuration
set(CMAKE_TOOLCHAIN_FILE "C:/Users/113ji/vcpkg/scripts/buildsystems/vcpkg.cmake")

# ---- Paths for static build
set(CMAKE_PREFIX_PATH
    "C:/Users/113ji/vcpkg/installed/x64-windows-static"
    "C:/Qt/6.8.0/msvc2022_64"
)
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

set(OpenCV_DIR "C:/Users/113ji/vcpkg/installed/x64-windows-static/share/opencv4")
set(Protobuf_DIR "C:/Users/113ji/vcpkg/installed/x64-windows-static/share/protobuf")

# ---- Set Protobuf protoc executable path
set(Protobuf_PROTOC_EXECUTABLE "C:/Users/113ji/vcpkg/installed/x64-windows-static/tools/protobuf/protoc.exe")

# ---- C++20 and Qt settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# ---- Find packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Svg)
find_package(OpenCV REQUIRED)
find_package(Tesseract REQUIRED)
find_package(BZip2 REQUIRED)
find_package(LZ4 REQUIRED)
find_package(Zstd REQUIRED)
find_package(OpenSSL REQUIRED)

# ---- Add subdirectories
add_subdirectory(src/Core)
add_subdirectory(src/Shared)
add_subdirectory(src/Controllers)
add_subdirectory(src/Main)
add_subdirectory(src/UI)
add_subdirectory(src/Resources)

# ---- Main executable
add_executable(AppleGameHelper src/main.cpp "include/Shared/StopControllable.h" "include/Shared/CancellationToken.h")

# ---- Link libraries
target_link_libraries(AppleGameHelper
    PRIVATE
        MainLib
        ControllersLib
        SharedLib
        CoreLib
        UILib
        StrategiesLib
        Qt6::Core Qt6::Widgets Qt6::Gui Qt6::Svg
        ${OpenCV_LIBS}
        Tesseract::libtesseract
        ${BZIP2_LIBRARIES}
        optimized C:/Users/113ji/vcpkg/installed/x64-windows-static/lib/lz4.lib
        debug     C:/Users/113ji/vcpkg/installed/x64-windows-static/debug/lib/lz4.lib
        optimized C:/Users/113ji/vcpkg/installed/x64-windows-static/lib/zstd.lib
        debug     C:/Users/113ji/vcpkg/installed/x64-windows-static/debug/lib/zstd.lib
        OpenSSL::Crypto
)

# ---- Install resources
install(TARGETS AppleGameHelper RUNTIME DESTINATION .)
install(DIRECTORY src/Resources/icons src/Resources/themes src/Resources/tessdata
        DESTINATION . FILES_MATCHING PATTERN "*")

# ---- Copy resources post-build
add_custom_command(TARGET AppleGameHelper POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/src/Resources $<TARGET_FILE_DIR:AppleGameHelper>/Resources
)

# ---- (Dynamic build only) DLL copy command
# Kept as comment for future dynamic build
# add_custom_command(TARGET AppleGameHelper POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#     ${CMAKE_SOURCE_DIR}/dlls $<TARGET_FILE_DIR:AppleGameHelper>
# )