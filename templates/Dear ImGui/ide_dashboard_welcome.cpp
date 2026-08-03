#include "imgui.h"

void RenderProjectDashboard() {
    ImGui::Begin("Welcome to Parcel C++");

    ImGui::TextDisabled("Parcel C++ v2.0 - Ultimate Edition");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Columns(2, "DashboardColumns", false);
    ImGui::SetColumnWidth(0, 300);

    // Left Side: Recent Projects
    ImGui::Text("Recent Projects");
    ImGui::Separator();
    for (int n = 0; n < 5; n++) {
        char buf[64];
        sprintf(buf, "Project Alpha %d", n);
        if (ImGui::Selectable(buf)) {}
        ImGui::TextDisabled("Last modified 2h ago");
        ImGui::Spacing();
    }

    ImGui::NextColumn();

    // Right Side: Quick Actions
    ImGui::Text("Quick Start");
    ImGui::Separator();

    auto QuickButton = [](const char* label, const char* desc) {
        if (ImGui::Button(label, ImVec2(200, 40))) {}
        ImGui::SameLine();
        ImGui::TextDisabled("%s", desc);
    };

    QuickButton("New Project", "Create a new C++ project from template");
    QuickButton("Open Folder", "Browse your local files");
    QuickButton("Clone Repository", "Import from GitHub or GitLab");

    ImGui::Spacing(); ImGui::Spacing();

    // Bottom: News/Tips
    ImGui::BeginChild("Tips", ImVec2(0, 100), true);
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Tip of the day:");
    ImGui::TextWrapped("You can use Ctrl+Alt+F to automatically fix code style issues detected by the AI agent.");
    ImGui::EndChild();

    ImGui::Columns(1);
    ImGui::End();
}
