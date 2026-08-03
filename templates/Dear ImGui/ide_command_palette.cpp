#include "imgui.h"

void RenderCommandPalette() {
    static bool show_palette = false;
    // Toggle logic should be outside (e.g. Ctrl+P)

    if (show_palette) {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(500, 300));

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
        if (ImGui::Begin("##CommandPalette", &show_palette, flags)) {
            static char search[128] = "";
            ImGui::SetNextItemWidth(-1);
            if (ImGui::InputTextWithHint("##Search", "Type a command or file name...", search, sizeof(search))) {
                // Filter logic
            }

            if (ImGui::IsWindowAppearing()) ImGui::SetKeyboardFocusHere();

            ImGui::Separator();

            if (ImGui::BeginChild("##Results")) {
                const char* commands[] = { "git: Commit Changes", "file: New C++ Class", "ide: Open Designer", "build: Build Project" };
                for (int i = 0; i < 4; i++) {
                    if (ImGui::Selectable(commands[i])) {
                        show_palette = false;
                    }
                }
            }
            ImGui::EndChild();
        }
        ImGui::End();
    }
}
