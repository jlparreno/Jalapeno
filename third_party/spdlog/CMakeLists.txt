# SPDLOG is header-only
add_library(spdlog INTERFACE)

# Includes
target_include_directories(spdlog INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}
)
