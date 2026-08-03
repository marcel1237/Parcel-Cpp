#include "imgui.h"

void RenderModals() {
    ImGui::Begin("Action Center");

    if (ImGui::Button("Delete Project")) {
        ImGui::OpenPopup("Confirm Delete?");
    }

    if (ImGui::BeginPopupModal("Confirm Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("This operation is permanent!\nAre you sure you want to proceed?");
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120, 0))) {
            // Logic to delete
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::End();
}
