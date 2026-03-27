#include "UIManager.h"

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

    // Create and register log sink
    m_sink = std::make_shared<ConsoleSinkMt>();
    m_sink->set_pattern("[%l] %v");

    auto logger = spdlog::default_logger();
    logger->sinks().push_back(m_sink);

    // Customize IMGUI style
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Base colors
    colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.20f, 0.21f, 0.22f, 1.00f);

    // Headers — panels and collapsing headers
    colors[ImGuiCol_Header] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.28f, 0.30f, 0.38f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);

    // Title bar
    colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.11f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.10f, 0.10f, 0.11f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.10f, 0.11f, 1.00f);

    // Frames — inputs, dragfloat, etc.
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.21f, 0.22f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.28f, 0.29f, 0.31f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);

    // Buttons
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.30f, 0.38f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);

    // Accent — checkboxes, sliders, selected items
    colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.46f, 0.74f, 1.00f, 1.00f);

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.16f, 0.18f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.28f, 0.30f, 0.38f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.10f, 0.10f, 0.11f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.15f, 0.16f, 0.18f, 1.00f);

    // Docking
    colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.10f, 0.10f, 0.11f, 1.00f);

    // Separators
    colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.21f, 0.22f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);

    // Text
    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);

    // Rounding and spacing
    style.WindowRounding = 4.0f;
    style.FrameRounding = 3.0f;
    style.GrabRounding = 3.0f;
    style.TabRounding = 3.0f;
    style.ScrollbarRounding = 3.0f;
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.FramePadding = ImVec2(6.0f, 3.0f);
    style.ItemSpacing = ImVec2(8.0f, 4.0f);
    style.IndentSpacing = 16.0f;
    style.ScrollbarSize = 12.0f;
    style.GrabMinSize = 8.0f;

    // Load Inter as main font
    std::string fonts_dir = static_cast<std::string>(FONTS_DIR);
    io.Fonts->AddFontFromFileTTF((fonts_dir + "Inter-Medium.ttf").c_str(), 14.0f);

    // Fallback if font is not found
    io.Fonts->Build();

    // Set GLFW window
    m_window = window;
}

void UIManager::update() 
{
	ImGuiIO& io = ImGui::GetIO();
	auto& renderer = Renderer::instance();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Specific calls to UI components
    draw_title_bar();
    draw_dockspace();
    draw_hierarchy();
    draw_properties();
    draw_materials();
    draw_log_panel();
    draw_render_settings();
    draw_viewport();
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

void UIManager::draw_title_bar()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    float height = ImGui::GetFrameHeight() * 1.5f;

    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, height));
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoNav;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 4.0f));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.10f, 0.10f, 0.11f, 1.0f));

    ImGui::Begin("##titlebar", nullptr, flags);
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor();

    // Handle drag
    float buttons_width = 90.0f;
    ImVec2 drag_size(ImGui::GetContentRegionAvail().x - buttons_width,
        ImGui::GetContentRegionAvail().y);

    ImGui::InvisibleButton("##drag", drag_size);

    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
    {
        m_dragging = true;
        double mx, my;
        glfwGetCursorPos(m_window, &mx, &my);

        // Save cursor offset inside the window
        m_drag_offset_x = mx;
        m_drag_offset_y = my;
    }

    if (m_dragging)
    {
        if (ImGui::IsMouseDown(0))
        {
            // Abosolute position of cursor in screen
            double mx, my;
            glfwGetCursorPos(m_window, &mx, &my);

            int wx, wy;
            glfwGetWindowPos(m_window, &wx, &wy);

            // New position = Absolute position - initial offset
            int new_x = static_cast<int>(wx + mx - m_drag_offset_x);
            int new_y = static_cast<int>(wy + my - m_drag_offset_y);

            glfwSetWindowPos(m_window, new_x, new_y);
        }
        else
        {
            m_dragging = false;
        }
    }

    // Centered title
    std::string title = "Jalapeno Engine";
    float text_width = ImGui::CalcTextSize(title.c_str()).x;
    float window_width = ImGui::GetWindowWidth();

    ImGui::SetCursorPos(ImVec2((window_width - text_width) * 0.5f, (ImGui::GetWindowHeight() - ImGui::GetTextLineHeight()) * 0.5f));
    ImGui::TextUnformatted(title.c_str());

    // Minimize, maximize/restore and close buttons
    float button_width = 28.0f;
    float button_height = ImGui::GetFrameHeight();
    float bar_height = ImGui::GetWindowHeight();
    float total_width = button_width * 3 + 2.0f;

    ImGui::SameLine(ImGui::GetWindowWidth() - total_width - 4.0f);
    ImGui::SetCursorPosY((bar_height - button_height) * 0.5f);

    // Minimize
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
    if (ImGui::Button("_", ImVec2(button_width, button_height)))
    {
#ifdef _WIN32
        ShowWindow(glfwGetWin32Window(m_window), SW_MINIMIZE);
#endif
    }
    ImGui::PopStyleColor(2);

    ImGui::SameLine(0, 1.0f);

    // Maximize / Restore
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
    if (ImGui::Button("[]", ImVec2(button_width, button_height)))
    {
#ifdef _WIN32
        HWND hwnd = glfwGetWin32Window(m_window);
        if (IsZoomed(hwnd))
            ShowWindow(hwnd, SW_RESTORE);
        else
            ShowWindow(hwnd, SW_MAXIMIZE);
#endif
    }
    ImGui::PopStyleColor(2);

    ImGui::SameLine(0, 1.0f);

    // Close
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));

    if (ImGui::Button("X", ImVec2(button_width, button_height)))
    {
        glfwSetWindowShouldClose(m_window, true);
    }

    ImGui::PopStyleColor(2);

    // Bottom separator with the rest of panels
    ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 1.0f);
    ImGui::Separator();

    ImGui::End();
}

void UIManager::draw_dockspace()
{
    // Create dockspace between title bar (on top) and status bar (on bottom)
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    float titlebar_height = ImGui::GetFrameHeight() * 1.5f;
    float statusbar_height = ImGui::GetFrameHeight() * 1.5f;

    // --- DOCKSPACE ---
    {
        ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + titlebar_height));
        ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, viewport->WorkSize.y - titlebar_height - statusbar_height));
        ImGui::SetNextWindowViewport(viewport->ID);

        // Dockspace properties
        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus;
        //ImGuiWindowFlags_MenuBar;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f); // Flat corners
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f); // No borders
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f)); // No padding, dockspace has exact dimensions


        ImGui::Begin("Dockspace", nullptr, flags);
        ImGui::PopStyleVar(3);

        // TODO: --- MENU BAR (TOP) ---
        {
            /*if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Scene"))
                {
                    ImGui::MenuItem("New Scene");
                    ImGui::MenuItem("Open Scene");
                    ImGui::MenuItem("Save Scene");
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Panels"))
                {
                    ImGui::MenuItem("Hierarchy", nullptr, nullptr);
                    ImGui::MenuItem("Properties", nullptr, nullptr);
                    ImGui::MenuItem("Viewport", nullptr, nullptr);
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }*/
        }

        // --- DOCKSPACE REGION ---
        {
            ImGuiID dockspace_id = ImGui::GetID("MainDockspace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
        }

        // --- STATUS BAR (BOTTOM) ---
        {
            ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + viewport->WorkSize.y - statusbar_height));
            ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, statusbar_height));
            ImGui::SetNextWindowViewport(viewport->ID);

            ImGuiWindowFlags status_flags =
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoScrollbar |
                ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_NoDocking |
                ImGuiWindowFlags_NoNav;

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 5.0f));
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.10f, 0.10f, 0.11f, 1.0f)); // Same than TitleBar bg

            ImGui::Begin("##statusbar", nullptr, status_flags); // Internal ID "statusbar"
            ImGui::PopStyleVar(3);
            ImGui::PopStyleColor();

            // Separator on top
            ImGui::Separator();

            // FPS y ms centrados
            std::string stats = fmt::format("{:.0f} FPS  |  {:.2f} ms", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
            float stats_width = ImGui::CalcTextSize(stats.c_str()).x;
            float bar_width = ImGui::GetContentRegionAvail().x;

            ImGui::SetCursorPosX((bar_width - stats_width) * 0.5f);
            ImGui::TextUnformatted(stats.c_str());

            ImGui::End();
        }

        ImGui::End();
    }
}

void UIManager::draw_hierarchy()
{
    // First check if we have a valid scene
    if (!m_scene)
    {
        SPDLOG_ERROR("Error trying to render scene hierarchy panel, no valid scene available");
        return;
    }

    ImGui::Begin("Hierarchy");

    // Renderables
    if (ImGui::CollapsingHeader("Renderables", ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (auto renderable : m_scene->get_scene_renderables())
        {
            ImGuiTreeNodeFlags flags = 
                ImGuiTreeNodeFlags_Leaf |
                ImGuiTreeNodeFlags_SpanAvailWidth;

            // Highlight selected renderable
            if (m_selected_renderable == renderable)
                flags |= ImGuiTreeNodeFlags_Selected;

            bool opened = ImGui::TreeNodeEx(renderable->get_name().c_str(), flags);

            // Update selected item on click
            if (ImGui::IsItemClicked())
            {
                m_selected_renderable = renderable;
                m_selected_light = nullptr;
                m_selected_material = nullptr;
            }

            if (opened)
                ImGui::TreePop();
        }
    }

    // Lights
    if (ImGui::CollapsingHeader("Lights", ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (auto& light : m_scene->get_scene_lights())
        {
            ImGuiTreeNodeFlags flags = 
                ImGuiTreeNodeFlags_Leaf |
                ImGuiTreeNodeFlags_SpanAvailWidth;

            // Highlight selected light
            if (m_selected_light == light.get())
                flags |= ImGuiTreeNodeFlags_Selected;

            bool opened = ImGui::TreeNodeEx(light->get_name().c_str(), flags);

            // Update selected item on click
            if (ImGui::IsItemClicked())
            {
                m_selected_light = light.get();
                m_selected_renderable = nullptr;
                m_selected_material = nullptr;
            }

            if (opened)
                ImGui::TreePop();
        }
    }

    ImGui::End();
}

void UIManager::draw_properties()
{
    ImGui::Begin("Properties");

    // Depending on the selected object, draw specific properties
    if (m_selected_renderable)
    {
        ImGui::SeparatorText("Transform");

        float widget_width = ImGui::GetContentRegionAvail().x - 80.0f;

        glm::vec3 position = m_selected_renderable->get_position();
        glm::vec3 rotation = m_selected_renderable->get_rotation();
        glm::vec3 scale = m_selected_renderable->get_scale();

        ImGui::Text("Position"); ImGui::SameLine(80); ImGui::SetNextItemWidth(widget_width);
        if (ImGui::DragFloat3("##position", &position.x, 0.1f))
            m_selected_renderable->set_position(position);

        ImGui::Text("Rotation"); ImGui::SameLine(80); ImGui::SetNextItemWidth(widget_width);
        if (ImGui::DragFloat3("##rotation", &rotation.x, 0.5f))
            m_selected_renderable->set_rotation(rotation);

        ImGui::Text("Scale"); ImGui::SameLine(80); ImGui::SetNextItemWidth(widget_width);
        if (ImGui::DragFloat3("##scale", &scale.x, 0.01f, 0.001f, 1000.0f))
            m_selected_renderable->set_scale(scale);
    }
    else if (m_selected_light)
    {
        ImGui::SeparatorText("Light");

        float widget_width = ImGui::GetContentRegionAvail().x - 80.0f;

        // Tipo — read only
        std::string type_str;
        if (dynamic_cast<DirectionalLight*>(m_selected_light)) type_str = "Directional";
        else if (dynamic_cast<PointLight*>(m_selected_light))  type_str = "Point";
        else                                                   type_str = "Unknown";

        ImGui::Text("Type"); ImGui::SameLine(80);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.75f, 1.0f, 1.0f));
        ImGui::Text("%s", type_str.c_str());
        ImGui::PopStyleColor();

        ImGui::Spacing();
        ImGui::SeparatorText("Properties");

        ImGui::Text("Color"); ImGui::SameLine(80); ImGui::SetNextItemWidth(widget_width);
        glm::vec3 color = m_selected_light->get_color();
        if (ImGui::ColorEdit3("##color", &color.x))
            m_selected_light->set_color(color);

        ImGui::Text("Intensity"); ImGui::SameLine(80); ImGui::SetNextItemWidth(widget_width);
        float intensity = m_selected_light->get_intensity();
        if (ImGui::DragFloat("##intensity", &intensity, 0.05f, 0.0f, 100.0f))
            m_selected_light->set_intensity(intensity);

        bool shadows = m_selected_light->get_shadows_enabled();
        if (ImGui::Checkbox("Cast Shadows", &shadows))
            m_selected_light->set_shadows_enabled(shadows);

        ImGui::Spacing();

        if (auto* dl = dynamic_cast<DirectionalLight*>(m_selected_light))
        {
            ImGui::SeparatorText("Directional Light");

            ImGui::Text("Direction"); ImGui::SameLine(80); ImGui::SetNextItemWidth(widget_width);
            glm::vec3 direction = dl->get_direction();
            if (ImGui::DragFloat3("##direction", &direction.x, 0.01f, -1.0f, 1.0f))
                dl->set_direction(direction);
        }
        else if (auto* pl = dynamic_cast<PointLight*>(m_selected_light))
        {
            ImGui::SeparatorText("Point Light");

            ImGui::Text("Position"); ImGui::SameLine(80); ImGui::SetNextItemWidth(widget_width);
            glm::vec3 position = pl->get_position();
            if (ImGui::DragFloat3("##position", &position.x, 0.1f))
                pl->set_position(position);

            ImGui::Spacing();
            ImGui::SeparatorText("Attenuation");

            float constant = pl->get_constant();
            float linear = pl->get_linear();
            float quadratic = pl->get_quadratic();

            ImGui::Text("Constant");  ImGui::SameLine(80); ImGui::SetNextItemWidth(widget_width);
            if (ImGui::DragFloat("##constant", &constant, 0.01f, 0.0f, 10.0f))
                pl->set_attenuation(constant, linear, quadratic);

            ImGui::Text("Linear");    ImGui::SameLine(80); ImGui::SetNextItemWidth(widget_width);
            if (ImGui::DragFloat("##linear", &linear, 0.001f, 0.0f, 1.0f))
                pl->set_attenuation(constant, linear, quadratic);

            ImGui::Text("Quadratic"); ImGui::SameLine(80); ImGui::SetNextItemWidth(widget_width);
            if (ImGui::DragFloat("##quadratic", &quadratic, 0.0001f, 0.0f, 1.0f))
                pl->set_attenuation(constant, linear, quadratic);
        }
    }
    else if (m_selected_material)
    {
        ImGui::SeparatorText("Material");

        ImGui::Text("Name"); ImGui::SameLine(80); ImGui::TextDisabled("%s", m_selected_material->get_name().c_str());
        ImGui::Text("Type"); ImGui::SameLine(80);

        if (auto* pbr = dynamic_cast<PBRMaterial*>(m_selected_material))
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.75f, 1.0f, 1.0f));
            ImGui::Text("PBR");
            ImGui::PopStyleColor();

            ImGui::Spacing();
            ImGui::SeparatorText("Properties");

            // Fijar el ancho del widget dejando espacio para el label
            float widget_width = ImGui::GetContentRegionAvail().x - 80.0f;

            ImGui::Text("Albedo");   ImGui::SameLine(80); ImGui::SetNextItemWidth(widget_width);
            glm::vec3 albedo = pbr->get_albedo();
            if (ImGui::ColorEdit3("##albedo", &albedo.x))
                pbr->set_albedo(albedo);

            ImGui::Text("Metallic"); ImGui::SameLine(80); ImGui::SetNextItemWidth(widget_width);
            float metallic = pbr->get_metallic();
            if (ImGui::SliderFloat("##metallic", &metallic, 0.0f, 1.0f))
                pbr->set_metallic(metallic);

            ImGui::Text("Roughness"); ImGui::SameLine(80); ImGui::SetNextItemWidth(widget_width);
            float roughness = pbr->get_roughness();
            if (ImGui::SliderFloat("##roughness", &roughness, 0.0f, 1.0f))
                pbr->set_roughness(roughness);
        }
        else if (auto* phong = dynamic_cast<PhongMaterial*>(m_selected_material))
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.75f, 1.0f, 1.0f));
            ImGui::Text("Phong");
            ImGui::PopStyleColor();

            ImGui::Spacing();
            ImGui::SeparatorText("Properties");

            float widget_width = ImGui::GetContentRegionAvail().x - 80.0f;

            ImGui::Text("Ambient");  ImGui::SameLine(80); ImGui::SetNextItemWidth(widget_width);
            glm::vec3 ambient = phong->get_ambient_color();
            if (ImGui::ColorEdit3("##ambient", &ambient.x))
                phong->set_ambient_color(ambient);

            ImGui::Text("Diffuse");  ImGui::SameLine(80); ImGui::SetNextItemWidth(widget_width);
            glm::vec3 diffuse = phong->get_diffuse_color();
            if (ImGui::ColorEdit3("##diffuse", &diffuse.x))
                phong->set_diffuse_color(diffuse);

            ImGui::Text("Specular"); ImGui::SameLine(80); ImGui::SetNextItemWidth(widget_width);
            glm::vec3 specular = phong->get_specular_color();
            if (ImGui::ColorEdit3("##specular", &specular.x))
                phong->set_specular_color(specular);

            ImGui::Text("Shininess"); ImGui::SameLine(80); ImGui::SetNextItemWidth(widget_width);
            float shininess = phong->get_shininess();
            if (ImGui::SliderFloat("##shininess", &shininess, 1.0f, 256.0f))
                phong->set_shininess(shininess);
        }
        else if (auto* lambert = dynamic_cast<LambertMaterial*>(m_selected_material))
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.75f, 1.0f, 1.0f));
            ImGui::Text("Lambert");
            ImGui::PopStyleColor();

            ImGui::Spacing();
            ImGui::SeparatorText("Properties");

            float widget_width = ImGui::GetContentRegionAvail().x - 80.0f;

            ImGui::Text("Ambient"); ImGui::SameLine(80); ImGui::SetNextItemWidth(widget_width);
            glm::vec3 ambient = lambert->get_ambient_color();
            if (ImGui::ColorEdit3("##ambient", &ambient.x))
                lambert->set_ambient_color(ambient);

            ImGui::Text("Diffuse"); ImGui::SameLine(80); ImGui::SetNextItemWidth(widget_width);
            glm::vec3 diffuse = lambert->get_diffuse_color();
            if (ImGui::ColorEdit3("##diffuse", &diffuse.x))
                lambert->set_diffuse_color(diffuse);
        }
    }
    else
    {
        ImGui::TextDisabled("No object selected");
    }

    ImGui::End();
}

void UIManager::draw_materials()
{
    ImGui::Begin("Materials");

    // Search filter and count info (adaptable size)
    float available_width = ImGui::GetContentRegionAvail().x;
    float count_text_width = ImGui::CalcTextSize("27 materials loaded").x + 8.0f;
    ImGui::SetNextItemWidth(available_width - count_text_width);

    static char search_buf[128] = "";
    ImGui::InputTextWithHint("##search", "Search...", search_buf, sizeof(search_buf));
    ImGui::SameLine();
    ImGui::TextDisabled("%zu loaded", MaterialManager::instance().get_all_materials().size());

    ImGui::Separator();

    // Scroll region for the grid
    ImGui::BeginChild("##materials_scroll");

    // Calculate how many cards fit per row
    float card_width = m_thumbnail_size * 3 + m_card_padding * 4 + 16.0f;
    float panel_width = ImGui::GetContentRegionAvail().x;
    int   cards_per_row = std::max(1, (int)(panel_width / (card_width + m_card_padding)));

    int card_index = 0;
    for (auto& [mat_name, material] : MaterialManager::instance().get_all_materials())
    {
        // Apply search filter
        if (strlen(search_buf) > 0 &&
            mat_name.find(search_buf) == std::string::npos)
            continue;

        // New row
        if (card_index % cards_per_row != 0)
            ImGui::SameLine(0, m_card_padding);

        // Draw specific material textures thumbnails
        draw_material_card(mat_name, material.get());
        card_index++;
    }

    ImGui::EndChild();
    ImGui::End();
}

void UIManager::draw_material_card(const std::string& name, Material* material)
{
    // Check valid material
    if (!material)
    {
        SPDLOG_ERROR("Error trying to render material card: {}. No valid material", name);
        return;
    }

    ImGui::PushID(name.c_str());

    bool is_selected = (m_selected_material == material);
    float card_width = ImGui::GetContentRegionAvail().x - m_card_padding;
    GLuint white_id = TextureManager::instance().get_white_texture()->get_id();

    // Check if all textures are the default white one
    bool has_real_textures = false;
    for (auto& [tex_name, tex] : material->get_all_textures())
    {
        if (tex->get_id() != white_id)
        {
            has_real_textures = true;
            break;
        }
    }

    // Card height depending on has_real_textures. Materials with no textures will show in a thinner card
    float card_height = has_real_textures ? m_thumbnail_size + 52.0f : 42.0f;

    // Highlight if its selected
    if (is_selected)
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.25f, 0.35f, 0.50f, 1.0f));
    else
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.18f, 0.19f, 0.21f, 1.0f));

    ImGui::BeginChild("##card", ImVec2(card_width, card_height), true, ImGuiWindowFlags_NoScrollbar);

    // Material type badge
    std::string type_str = "Unknown";
    if (dynamic_cast<PBRMaterial*>(material))     type_str = "PBR";
    if (dynamic_cast<PhongMaterial*>(material))   type_str = "Phong";
    if (dynamic_cast<LambertMaterial*>(material)) type_str = "Lambert";

    // Name and type label
    ImGui::TextUnformatted(name.c_str());
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.75f, 1.0f, 1.0f));
    ImGui::Text("[%s]", type_str.c_str());
    ImGui::PopStyleColor();

    // Thumbnails only if there are textures
    if (has_real_textures)
    {
        ImGui::Separator();

        bool first = true;
        for (auto& [tex_name, tex] : material->get_all_textures())
        {
            if (tex->get_id() == white_id)
                continue;

            if (!first) 
                ImGui::SameLine(0, m_card_padding);

            first = false;

            ImGui::BeginGroup();

            ImGui::Image(
                (ImTextureID)(intptr_t)tex->get_id(),
                ImVec2(m_thumbnail_size, m_thumbnail_size),
                ImVec2(0, 1), ImVec2(1, 0)
            );

            // Show bigger image if an specific texture is hovered
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::Image(
                    (ImTextureID)(intptr_t)tex->get_id(),
                    ImVec2(128.0f, 128.0f),
                    ImVec2(0, 1), ImVec2(1, 0)
                );
                ImGui::Text("%s", tex_name.c_str());
                ImGui::EndTooltip();
            }

            // Adjust display name to fit space
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
            std::string short_name = tex_name.length() > 8
                ? tex_name.substr(0, 7) + "."
                : tex_name;
            ImGui::TextUnformatted(short_name.c_str());
            ImGui::PopStyleColor();

            ImGui::EndGroup();
        }
    }

    // Check click
    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0))
    {
        m_selected_material = material;
        m_selected_renderable = nullptr;
        m_selected_light = nullptr;
    }

    ImGui::EndChild();
    ImGui::PopStyleColor();
    ImGui::PopID();
}

void UIManager::draw_log_panel()
{
    ImGui::Begin("Console");

    // Clear console button
    if (ImGui::Button("Clear"))
        m_sink->clear();

    ImGui::SameLine();

    // Level filters
    auto toggle_button = [](const char* label, bool& active, ImVec4 color)
        {
            if (!active)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
            }
            else
            {
                ImGui::PushStyleColor(ImGuiCol_Button, color);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
            }

            if (ImGui::Button(label))
                active = !active;

            ImGui::PopStyleColor(3);
            ImGui::SameLine();
        };

    toggle_button("INFO", m_show_info, ImVec4(0.3f, 0.6f, 0.9f, 1.0f));
    toggle_button("WARN", m_show_warn, ImVec4(0.9f, 0.7f, 0.1f, 1.0f));
    toggle_button("ERROR", m_show_error, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
    toggle_button("DEBUG", m_show_debug, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));

    ImGui::SameLine();

    // Search bar
    float remaining = ImGui::GetContentRegionAvail().x;
    ImGui::SetNextItemWidth(remaining);
    ImGui::InputTextWithHint("##search", "Filter...", m_search_buf, sizeof(m_search_buf));

    // Message count
    auto& entries = m_sink->get_entries();
    int info_count = 0, warn_count = 0, error_count = 0, debug_count = 0;
    for (auto& e : entries)
    {
        if (e.level == spdlog::level::info)  info_count++;
        if (e.level == spdlog::level::warn)  warn_count++;
        if (e.level == spdlog::level::err)   error_count++;
        if (e.level == spdlog::level::debug) debug_count++;
    }

    ImGui::TextDisabled("Info: %d  Warn: %d  Err: %d  Debug: %d", info_count, warn_count, error_count, debug_count);

    ImGui::Separator();

    // Render messages
    ImGui::BeginChild("##console_scroll", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    for (auto& entry : entries)
    {
        // Filter by level
        if (entry.level == spdlog::level::info && !m_show_info)  continue;
        if (entry.level == spdlog::level::warn && !m_show_warn)  continue;
        if (entry.level == spdlog::level::err && !m_show_error) continue;
        if (entry.level == spdlog::level::debug && !m_show_debug) continue;

        // Filter by text
        if (strlen(m_search_buf) > 0 && entry.message.find(m_search_buf) == std::string::npos)
            continue;

        // If all filters pass, render message (white)
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImGui::TextUnformatted(entry.message.c_str());
        ImGui::PopStyleColor();
    }

    // Autoscroll
    if (m_autoscroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();

    ImGui::End();
}

void UIManager::draw_render_settings()
{
    ImGui::Begin("Render Settings");

    float widget_width = ImGui::GetContentRegionAvail().x - 90.0f;

    ImGui::SeparatorText("Passes");

    // Skybox
    bool skybox_enabled = m_skybox_pass && m_skybox_pass->get_enabled();
    if (ImGui::Checkbox("Skybox", &skybox_enabled) && m_skybox_pass)
        m_skybox_pass->set_enabled(skybox_enabled);

    ImGui::Spacing();
    ImGui::SeparatorText("Display");

    // Wireframe
    bool wireframe = Renderer::instance().get_wireframe();
    if (ImGui::Checkbox("Wireframe", &wireframe))
        Renderer::instance().set_wireframe(wireframe);

    // VSync
    bool vsync = Renderer::instance().get_vsync();
    if (ImGui::Checkbox("VSync", &vsync))
        Renderer::instance().set_vsync(vsync);

    ImGui::End();
}

void UIManager::draw_viewport()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport");

    ImVec2 size = ImGui::GetContentRegionAvail();
    int new_width = static_cast<int>(size.x);
    int new_height = static_cast<int>(size.y);

    // Resize FBOs if viewport size changed
    if (new_width != m_viewport_width || new_height != m_viewport_height)
    {
        m_viewport_width = new_width;
        m_viewport_height = new_height;
        FramebufferManager::instance().resize_all(m_viewport_width, m_viewport_height);
    }

    // Display resolved texture
    Framebuffer* resolve_fbo = FramebufferManager::instance().get_framebuffer("resolve");
    if (resolve_fbo)
    {
        GLuint tex_id = resolve_fbo->get_color_attachment(0);
        ImGui::Image(
            (ImTextureID)(intptr_t)tex_id,
            size,
            ImVec2(0, 1),  // UV flip vertical — OpenGL origin is bottom-left
            ImVec2(1, 0)
        );
    }

    // Update mouse state
    m_viewport_hovered = ImGui::IsWindowHovered();
    m_viewport_focused = ImGui::IsWindowFocused();

    ImGui::End();
    ImGui::PopStyleVar();
}
