# Create an exceutable with all our sources
file(GLOB_RECURSE SRC_FILES
    "*.cpp"
)

file(GLOB_RECURSE HEADER_FILES
    "*.h"
    "*.hpp"
)

# Organize source and header files in Visual Studio filters matching folder structure
foreach(FILE ${SRC_FILES})
    get_filename_component(FILE_DIR ${FILE} DIRECTORY)
    file(RELATIVE_PATH FILE_GROUP ${CMAKE_CURRENT_SOURCE_DIR} ${FILE_DIR})
    #string(REPLACE "/" "/" FILE_GROUP ${FILE_GROUP})
    source_group("src/${FILE_GROUP}" FILES ${FILE})
endforeach()

foreach(FILE ${HEADER_FILES})
    get_filename_component(FILE_DIR ${FILE} DIRECTORY)
    file(RELATIVE_PATH FILE_GROUP ${CMAKE_CURRENT_SOURCE_DIR} ${FILE_DIR})
    #string(REPLACE "/" "/" FILE_GROUP ${FILE_GROUP})
    source_group("src/${FILE_GROUP}" FILES ${FILE})
endforeach()

add_executable(${PROJECT_NAME} ${SRC_FILES} ${HEADER_FILES})

# Include each subsystem directory so headers can be found
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/core
        ${CMAKE_CURRENT_SOURCE_DIR}/core/renderer
        ${CMAKE_CURRENT_SOURCE_DIR}/renderables
        ${CMAKE_CURRENT_SOURCE_DIR}/materials
        ${CMAKE_CURRENT_SOURCE_DIR}/lights
        ${CMAKE_CURRENT_SOURCE_DIR}/io
)

# Shaders and SPDLOG level macros
target_compile_definitions(${PROJECT_NAME} PRIVATE
    TEXTURES_DIR="${CMAKE_SOURCE_DIR}/resources/textures/"
    MODELS_DIR="${CMAKE_SOURCE_DIR}/resources/models/"
    SHADERS_DIR="${CMAKE_SOURCE_DIR}/shaders/"
    FONTS_DIR="${CMAKE_SOURCE_DIR}/resources/fonts/"
    SPDLOG_ACTIVE_LEVEL=${SPDLOG_LEVEL_NUM}
    GLFW_EXPOSE_NATIVE_WIN32
)

# Copy Assimp DLL to executable dir
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/third_party/assimp/lib/$<CONFIG>/assimp-vc145-$<IF:$<CONFIG:Debug>,mtd,mt>.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
