# stb_image is header-only
add_library(stb_image INTERFACE)

# Include headers
target_include_directories(stb_image INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
