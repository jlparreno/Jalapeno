# Create an exceutable with all our sources
file(GLOB_RECURSE SRC_FILES
    "main.cpp"
    "Camera.cpp"
    "Texture.cpp"
    "TextureManager.cpp"
    "ShaderProgram.cpp"
    "ShaderManager.cpp"
    "Mesh.cpp"
    "Model.cpp"
    "Framebuffer.cpp"
    "FramebufferManager.cpp"
    "Renderer.cpp"
    "Scene.cpp"
    "Light.cpp"
    "UIManager.cpp"
    "InputManager.cpp"
    "Material.cpp"
    "PhongMaterial.cpp"
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

# Shaders and SPDLOG level macros
target_compile_definitions(${PROJECT_NAME} PRIVATE
    TEXTURES_DIR="${CMAKE_SOURCE_DIR}/textures/"
    MODELS_DIR="${CMAKE_SOURCE_DIR}/models/"
    SHADERS_DIR="${CMAKE_SOURCE_DIR}/shaders/"
    SPDLOG_ACTIVE_LEVEL=${SPDLOG_LEVEL_NUM}
)

# Copy Assimp DLL to executable dir
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/third_party/assimp/lib/$<CONFIG>/assimp-vc145-$<IF:$<CONFIG:Debug>,mtd,mt>.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
