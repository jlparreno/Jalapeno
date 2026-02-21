# Main ImGui files
set(IMGUI_SRC
    imgui.cpp
    imgui_draw.cpp
    imgui_widgets.cpp
    imgui_tables.cpp
    imgui_demo.cpp
)

# Used backends (GLFW + OpenGL3)
set(IMGUI_BACKENDS
    backends/imgui_impl_glfw.cpp
    backends/imgui_impl_opengl3.cpp
)

# Create static ImGui library
add_library(imgui STATIC
    ${IMGUI_SRC}
    ${IMGUI_BACKENDS}
)

# Include headers
target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}   
    ${CMAKE_CURRENT_SOURCE_DIR}/backends
)

# Link with needed libraries
target_link_libraries(imgui PUBLIC
    glfw
    glad
)
