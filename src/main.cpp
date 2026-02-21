#include <spdlog/spdlog.h>

#include "Renderer.h"
#include "Scene.h"

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

    // Models
    scene->add_model("backpack", static_cast<std::string>(MODELS_DIR) + "backpack/backpack.obj");

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