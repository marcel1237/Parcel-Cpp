#include "imgui.h"

void RenderNavigationRail() {
    static int selected_tab = 0;
    const float rail_width = 60.0f;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(ImVec2(rail_width, viewport->Size.y));

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.12f, 0.12f, 0.15f, 1.00f));
    ImGui::Begin("NavRail", nullptr, window_flags);

    auto NavItem = [&](const char* icon, int id) {
        bool is_selected = (selected_tab == id);
        if (is_selected) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.26f, 0.59f, 0.98f, 1.00f));

        ImGui::SetCursorPosX((rail_width - 30.0f) * 0.5f);
        if (ImGui::Button(icon, ImVec2(30, 30))) {
            selected_tab = id;
        }

        if (is_selected) ImGui::PopStyleColor();
    };

    ImGui::Spacing(); ImGui::Spacing();
    NavItem("P", 0); // Project
    ImGui::Spacing();
    NavItem("C", 1); // Code
    ImGui::Spacing();
    NavItem("D", 2); // Debug
    ImGui::Spacing();
    NavItem("G", 3); // Git

    ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 40.0f);
    NavItem("S", 4); // Settings

    ImGui::End();
    ImGui::PopStyleColor();
}
