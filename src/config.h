#pragma once

// Window / viewport dimensions
constexpr int SRC_WIDTH		= 2400;
constexpr int SRC_HEIGHT	= 1200;

// Aspect ratio limits
constexpr float MIN_ASPECT = 4.0f / 3.0f;
constexpr float MAX_ASPECT = 16.0f / 9.0f;

// Multisampling
constexpr int NUM_SAMPLES = 8;

// Shadow maps size, always squared
constexpr int SHADOWMAPS_SIZE = 2048;

// Max lights — must match the #defines in lights.glsl
constexpr int MAX_POINT_LIGHTS			= 4;
constexpr int MAX_DIRECTIONAL_LIGHTS	= 2;

// UBO binding points
constexpr int POINT_LIGHT_UBO_BINDING       = 0;
constexpr int DIRECTIONAL_LIGHT_UBO_BINDING = 1;

// Texture units
constexpr int ALBEDO_TEX_UNIT				= 0;
constexpr int METALLIC_ROUGHNESS_TEX_UNIT	= 1;
constexpr int NORMAL_TEX_UNIT				= 2;

constexpr int DIRECTIONAL_SHADOW_START	= 8;	// Units 8,9 for directional shadow maps
constexpr int POINT_SHADOW_START		= 10;	// Units 10,11,12,13 for point shadow maps

constexpr int INACTIVE_SHADOW_2D_UNIT	= 14;	// for inactive directional lights (sampler2D)
constexpr int INACTIVE_SHADOW_CUBE_UNIT = 15;	// for inactive point lights (samplerCube)
