#include <spdlog/spdlog.h>

#include "Renderer.h"
#include "Scene.h"
#include "PhongMaterial.h"

int main()
{
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
    // SCENE SETUP
    // ------------------------------------------------------------------------------
    Scene* scene = new Scene("main_scene");

    // Materials
    // TODO: Material manager to create, access and delete all available materials
    PhongMaterial* phong_material = new PhongMaterial("phong", ShaderManager::instance().get_program("lighting_phong"));

    // Models
    Model* backpack = scene->add_model("backpack", static_cast<std::string>(MODELS_DIR) + "backpack/backpack.obj");
    if (backpack)
    {
        backpack->set_material(phong_material);
        backpack->set_position(glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        backpack->set_scale(glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
    }

    // Lights
    Light* point_light1 = scene->add_light("Point1");
    if (point_light1)
    {
        point_light1->set_position(glm::vec3(1.2f, 1.0f, 2.0f));
    }

    // ------------------------------------------------------------------------------
    // RENDER LOOP
    // ------------------------------------------------------------------------------
    renderer.set_scene(scene);
    renderer.run();

    SPDLOG_INFO("Closing engine...");

    return 0;
}