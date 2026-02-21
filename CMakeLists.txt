cmake_minimum_required(VERSION 3.20)

project(Jalapeno VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Visual Studio needs to be configured for UTF-8 strings (spdlog)
if (MSVC)
    add_compile_options(/utf-8)
endif()

# SPDLOG levels
option(LOG_LEVEL "Log level for spdlog" "debug")
string(TOLOWER "${LOG_LEVEL}" LOG_LEVEL_LOWER)

# Choose spdlog level depending on input string
if(LOG_LEVEL_LOWER STREQUAL "trace")
    set(SPDLOG_LEVEL_NUM 0)
elseif(LOG_LEVEL_LOWER STREQUAL "debug")
    set(SPDLOG_LEVEL_NUM 1)
elseif(LOG_LEVEL_LOWER STREQUAL "info")
    set(SPDLOG_LEVEL_NUM 2)
elseif(LOG_LEVEL_LOWER STREQUAL "warn")
    set(SPDLOG_LEVEL_NUM 3)
elseif(LOG_LEVEL_LOWER STREQUAL "error")
    set(SPDLOG_LEVEL_NUM 4)
elseif(LOG_LEVEL_LOWER STREQUAL "critical")
    set(SPDLOG_LEVEL_NUM 5)
else()
    message(WARNING "Unknown LOG_LEVEL: ${LOG_LEVEL}. Using 'info'")
    set(SPDLOG_LEVEL_NUM 2)
endif()

message(STATUS "Configuring spdlog with SPDLOG_ACTIVE_LEVEL=${SPDLOG_LEVEL_NUM}")

# Automatic Debug/Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

# --- Subdirectories ---

# GLAD
add_subdirectory(third_party/glad)

# GLFW
add_subdirectory(third_party/glfw)

# GLM
add_subdirectory(third_party/glm)

# STB
add_subdirectory(third_party/stb)

# IMGUI
add_subdirectory(third_party/imgui)

# ASSIMP
add_subdirectory(third_party/assimp)

# SPDLOG
add_subdirectory(third_party/spdlog)

# Sources
add_subdirectory(src)

# Libraries for linking
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        glad
        glfw
        glm
        stb_image
        imgui
        assimp
        spdlog
)

# Show shaders in Visual Studio solution
file(GLOB SHADER_FILES "${CMAKE_SOURCE_DIR}/shaders/*.*")

source_group("Shaders" FILES ${SHADER_FILES})

add_custom_target(Shaders SOURCES ${SHADER_FILES})
