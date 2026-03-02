#include "UIManager.h"
#include "Renderer.h"

void textures_menu(MaterialManager& tex_mgr);

void UIManager::init(GLFWwindow* window)
{
	//Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	// Enable Keyboard Controls and docking
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
}

void UIManager::update() 
{
	ImGuiIO& io = ImGui::GetIO();
	auto& renderer = Renderer::instance();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Call here to the specific functions of UI components
	/*DrawDockspace();
	DrawHierarchy();
	DrawViewport(FramebufferManager::instance().get_framebuffer("main"));*/
    textures_menu(MaterialManager::instance());

	//ImGui::ShowDemoWindow();
}

void UIManager::render() 
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void  UIManager::terminate() 
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void textures_menu(MaterialManager& mat_mgr)
{
    if (ImGui::Begin("Textures"))
    {
        float thumbnailSize = 64.0f;
        float padding = 16.0f;

        /*float cellSize = thumbnailSize + padding;
        float panelWidth = ImGui::GetContentRegionAvail().x;

        int columns = (int)(panelWidth / cellSize);
        if (columns < 1) columns = 1;*/

        //ImGui::Columns(columns, 0, false);

        for (auto& [matName, material] : mat_mgr.get_all_materials())
        {
            // Crea un nodo por material
            if (ImGui::TreeNode(matName.c_str()))
            {
                // Asegura IDs únicos por cada material
                ImGui::PushID(matName.c_str());

                for (auto& [texName, tex] : material->get_all_textures())
                {
                    ImGui::PushID(texName.c_str());

                    // Imagen de la textura
                    ImGui::Image(
                        (void*)(intptr_t)tex->get_id(),
                        ImVec2(thumbnailSize, thumbnailSize),
                        ImVec2(0, 1),
                        ImVec2(1, 0)
                    );

                    ImGui::SameLine();
                    ImGui::TextWrapped("%s", texName.c_str());

                    ImGui::PopID();

                    // Espacio entre texturas
                    ImGui::Dummy(ImVec2(0, padding));
                }

                ImGui::PopID();
                ImGui::TreePop();
            }
        }

        //ImGui::Columns(1);
    }
    ImGui::End();
}

//void DrawDockspace()
//{
//    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
//
//    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
//
//    ImGuiViewport* viewport = ImGui::GetMainViewport();
//    ImGui::SetNextWindowPos(viewport->WorkPos);
//    ImGui::SetNextWindowSize(viewport->WorkSize);
//    ImGui::SetNextWindowViewport(viewport->ID);
//
//    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
//        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
//        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
//
//    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
//    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
//
//    ImGui::Begin("Dockspace Window", nullptr, window_flags);
//    ImGui::PopStyleVar(2);
//
//    ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
//    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
//
//    ImGui::End();
//}
//
//void DrawHierarchy()
//{
//    ImGui::Begin("Hierarchy");
//
//    if (ImGui::TreeNode("World"))
//    {
//        ImGui::TreeNode("Terrain");
//        ImGui::TreeNode("Camera");
//        ImGui::TreeNode("Player");
//        ImGui::TreePop();
//    }
//
//    ImGui::End();
//}

//void DrawViewport(uint32_t textureID)
//{
//    ImGui::Begin("Scene");
//
//    ImVec2 size = ImGui::GetContentRegionAvail();
//    int new_width = static_cast<int>(size.x);
//    int new_height = static_cast<int>(size.y);
//
//    // Check if the size has changed and resize FBOs
//    if (new_width != viewport_width || new_height != viewport_height)
//    {
//        viewport_width = new_width;
//        viewport_height = new_height;
//        FramebufferManager::instance().resize_all(viewport_width, viewport_height);
//    }
//
//    // Draw the FBO received in viewport
//    ImGui::Image((void*)(intptr_t)textureID, size, ImVec2(0, 1), ImVec2(1, 0));
//
//
//    //Check if mouse is overing the viewport
//    viewport_pos = ImGui::GetWindowPos();
//    ImVec2 mouse_pos = ImGui::GetIO().MousePos;
//
//    mouse_in_viewport = mouse_pos.x >= viewport_pos.x && mouse_pos.x <= viewport_pos.x + size.x &&
//                        mouse_pos.y >= viewport_pos.y && mouse_pos.y <= viewport_pos.y + size.y;
//
//    ImGui::End();
//}
