# Precompiled ASSIMP library
add_library(assimp STATIC IMPORTED GLOBAL)

# Include headers
set_target_properties(assimp PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# Select correct library depending on Debug/Release mode
if (MSVC)
    set_target_properties(assimp PROPERTIES
        IMPORTED_LOCATION_DEBUG          "${CMAKE_CURRENT_SOURCE_DIR}/lib/Debug/assimp-vc145-mtd.lib"
        IMPORTED_LOCATION_RELEASE        "${CMAKE_CURRENT_SOURCE_DIR}/lib/Release/assimp-vc145-mt.lib"
        IMPORTED_LOCATION_RELWITHDEBINFO "${CMAKE_CURRENT_SOURCE_DIR}/lib/Release/assimp-vc145-mt.lib"
        IMPORTED_LOCATION_MINSIZEREL     "${CMAKE_CURRENT_SOURCE_DIR}/lib/Release/assimp-vc145-mt.lib"
    )
else()
    # For future Linux integration
    set_target_properties(assimp PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp-vc145-mt.a"
    )
endif()
