#include "imgui.h"

void LoadFonts() {
    ImGuiIO& io = ImGui::GetIO();

    // Load default font
    io.Fonts->AddFontDefault();

    // Load custom font (Ensure the file exists in your resources)
    // ImFont* font = io.Fonts->AddFontFromFileTTF("path/to/font.ttf", 18.0f);
    // IM_ASSERT(font != NULL);
}
