# 빌드 시 리소스 복사
add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/Resources
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/icons
        ${CMAKE_BINARY_DIR}/Resources/icons
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/images
        ${CMAKE_BINARY_DIR}/Resources/images
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/themes
        ${CMAKE_BINARY_DIR}/Resources/themes
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/tessdata
        ${CMAKE_BINARY_DIR}/Resources/tessdata
    COMMENT "Copying resource files..."
)

# 설치 시 리소스 포함
install(DIRECTORY 
    icons/
    themes/
    tessdata/
    images/
    DESTINATION Resources
    FILES_MATCHING PATTERN "*"
)