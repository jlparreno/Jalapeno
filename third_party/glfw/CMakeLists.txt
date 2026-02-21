# Precompiled GLFW library
add_library(glfw STATIC IMPORTED GLOBAL)

# Include headers
set_target_properties(glfw PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# Select correct library depending on Debug/Release mode
if (MSVC)
    set_target_properties(glfw PROPERTIES
        IMPORTED_LOCATION_DEBUG          "${CMAKE_CURRENT_SOURCE_DIR}/lib/Debug/glfw3.lib"
        IMPORTED_LOCATION_RELEASE        "${CMAKE_CURRENT_SOURCE_DIR}/lib/Release/glfw3.lib"
        IMPORTED_LOCATION_RELWITHDEBINFO "${CMAKE_CURRENT_SOURCE_DIR}/lib/Release/glfw3.lib"
        IMPORTED_LOCATION_MINSIZEREL     "${CMAKE_CURRENT_SOURCE_DIR}/lib/Release/glfw3.lib"
    )
else()
    # For future Linux integration
    set_target_properties(glfw PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/libglfw3.a"
    )
endif()
