#include <spdlog/spdlog.h>

#include "Renderer.h"
#include "Scene.h"
#include "LambertMaterial.h"
#include "PhongMaterial.h"

int main()
{
    #ifdef _WIN32
        // Minimize console window on startup
        ShowWindow(GetConsoleWindow(), SW_MINIMIZE);
    #endif

    // ------------------------------------------------------------------------------
    // INIT LOGGER
    // ------------------------------------------------------------------------------
    spdlog::set_level(static_cast<spdlog::level::level_enum>(SPDLOG_ACTIVE_LEVEL));
    SPDLOG_INFO("Starting engine...");

    // ------------------------------------------------------------------------------
    // INIT RENDERER
    // ------------------------------------------------------------------------------
    auto& renderer = Renderer::instance();

    // ------------------------------------------------------------------------------
    // MATERIALS
    // ------------------------------------------------------------------------------
    auto& material_mgr = MaterialManager::instance();
    LambertMaterial* lambert_material = material_mgr.add_material<LambertMaterial>("Red Lambert");
    if (lambert_material)
    {
        lambert_material->set_diffuse_color(glm::vec3(1.0f, 0.0f, 0.0f));
    }

    PhongMaterial* phong_material = material_mgr.add_material<PhongMaterial>("Blue Phong");
    if (phong_material)
    {
        phong_material->set_diffuse_color(glm::vec3(0.0f, 0.0f, 1.0f));
    }

    PBRMaterial* pbr_1 = material_mgr.add_material<PBRMaterial>("PBR1");
    if (pbr_1)
    {
        pbr_1->set_albedo(glm::vec3(1.0f, 0.0f, 0.0f));
        pbr_1->set_metallic(1.0f);
        pbr_1->set_roughness(0.3f);
    }

    // ------------------------------------------------------------------------------
    // SCENE SETUP
    // ------------------------------------------------------------------------------
    auto scene = std::make_unique<Scene>("Main Scene");

    // MODELS
   /* Sphere* sphere = scene->add_sphere("Sphere 1");
    if (sphere)
    {
        sphere->set_material(pbr_1);
        sphere->set_position(glm::vec3(5.0f, 1.0f, 0.0f));
        sphere->set_scale(glm::vec3(0.5f, 0.5f, 0.5f));
    }*/

    Model* sponza = scene->add_model("Sponza", static_cast<std::string>(MODELS_DIR) + "Sponza/glTF/Sponza.gltf");
    if (sponza)
    {
        sponza->set_scale(glm::vec3(0.01f, 0.01f, 0.01f));	// it's a bit too big for our scene, so scale it down
    }
    
    // LIGHTS
    DirectionalLight* sun = scene->add_directional_light(
        "Sun Light",
        glm::vec3(-0.2f, -0.9f, -0.3f),  // Direction
        glm::vec3(1.0f, 0.95f, 0.85f),   // Warmer color
        3.0f                             // Intensity
    );
    sun->set_shadows_enabled(true);

    /*PointLight* point1 = scene->add_point_light(
        "Point Light",
        glm::vec3(0.0f, 5.0f, 0.0f),
        glm::vec3(1.0f),
        1.0f
    );
    point1->set_attenuation(1.0f, 0.09f, 0.032f);
    point1->set_shadows_enabled(true);*/

    // ------------------------------------------------------------------------------
    // RENDER LOOP
    // ------------------------------------------------------------------------------
    renderer.set_scene(scene.get());
    renderer.run();

    SPDLOG_INFO("Closing engine...");

    return 0;
}