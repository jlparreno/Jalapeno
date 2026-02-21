# Static GLAD library
add_library(glad STATIC
    src/glad.c
)

# Includes
target_include_directories(glad PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# OpenGL depending on platform
if (WIN32)
    target_link_libraries(glad PUBLIC opengl32)
elseif (APPLE)
    target_link_libraries(glad PUBLIC "-framework OpenGL")
else()
    target_link_libraries(glad PUBLIC GL)
endif()

