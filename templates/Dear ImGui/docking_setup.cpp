#include "imgui.h"

void SetupDocking() {
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    }
}

// Use ConfigFlags |= ImGuiConfigFlags_DockingEnable in initialization.
