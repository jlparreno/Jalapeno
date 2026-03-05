#include <spdlog/spdlog.h>

#include "Renderer.h"
#include "Scene.h"
#include "LambertMaterial.h"
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

    // MATERIALS
    auto& material_mgr = MaterialManager::instance();
    LambertMaterial* lambert_material = material_mgr.add_material<LambertMaterial>("red");
    if (lambert_material)
    {
        lambert_material->set_diffuse_color(glm::vec3(1.0f, 0.0f, 0.0f));
    }

    PhongMaterial* phong_material = material_mgr.add_material<PhongMaterial>("blue_phong");
    if (phong_material)
    {
        phong_material->set_diffuse_color(glm::vec3(0.0f, 0.0f, 1.0f));
    }

    // MODELS
    /*Sphere* sphere = scene->add_sphere("sphere1");
    if (sphere)
    {
        sphere->set_material(phong_material);
        sphere->set_position(glm::vec3(0.0f, 1.0f, 0.0f));
        sphere->set_scale(glm::vec3(0.5f, 0.5f, 0.5f));
    }*/

    Model* sponza = scene->add_model("sponza", static_cast<std::string>(MODELS_DIR) + "Sponza/glTF/Sponza.gltf");
    if (sponza)
    {
        sponza->set_scale(glm::vec3(0.01f, 0.01f, 0.01f));	// it's a bit too big for our scene, so scale it down
    }

    //Renderable* backpack = scene->add_model("backpack", static_cast<std::string>(MODELS_DIR) + "backpack/backpack.obj");
    //if (backpack)C:\dev\Jalapeno\include\types.h
    //{
    //    backpack->set_position(glm::vec3(0.0f, 1.0f, 0.0f)); // translate it down so it's at the center of the scene
    //    backpack->set_scale(glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
    //}

    // LIGHTS
    DirectionalLight* sun = scene->add_directional_light(
        "sun",
        glm::vec3(-0.5f, -1.0f, -0.3f),  // dirección
        glm::vec3(1.0f, 0.95f, 0.8f),    // color cálido
        1.5f                               // intensidad
    );

    /*PointLight* fill = scene->add_point_light(
        "fill",
        glm::vec3(0.0f, 5.0f, 0.0f),
        glm::vec3(1.0f),
        1.0f
    );
    fill->set_attenuation(1.0f, 0.09f, 0.032f);*/

    // ------------------------------------------------------------------------------
    // RENDER LOOP
    // ------------------------------------------------------------------------------
    renderer.set_scene(scene);
    renderer.run();

    SPDLOG_INFO("Closing engine...");

    return 0;
}