#include "imgui.h"

void RenderHexEditor(unsigned char* data, size_t size) {
    ImGui::Begin("Hex Editor");

    ImGui::BeginChild("##HexScrolling", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);

    for (size_t i = 0; i < size; i += 16) {
        ImGui::Text("%08X: ", (unsigned int)i);
        ImGui::SameLine();

        // Hex values
        for (int j = 0; j < 16; j++) {
            if (i + j < size)
                ImGui::Text("%02X ", data[i + j]);
            else
                ImGui::Text("   ");
            ImGui::SameLine();
        }

        ImGui::Text(" | ");
        ImGui::SameLine();

        // ASCII values
        for (int j = 0; j < 16; j++) {
            if (i + j < size) {
                unsigned char c = data[i + j];
                ImGui::Text("%c", (c >= 32 && c <= 126) ? c : '.');
            }
            ImGui::SameLine();
        }
        ImGui::NewLine();
    }

    ImGui::EndChild();
    ImGui::End();
}
