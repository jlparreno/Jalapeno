# GLM is header-only
add_library(glm INTERFACE)

# Includes
target_include_directories(glm INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
