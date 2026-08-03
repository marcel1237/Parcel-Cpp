#include "imgui.h"

void ApplyGlassNeonStyle() {
    ImGuiStyle& style = ImGui::GetStyle();

    // Transparency settings
    style.Alpha = 0.95f;
    style.WindowRounding = 12.0f;
    style.ChildRounding = 8.0f;
    style.FrameRounding = 6.0f;
    style.GrabRounding = 12.0f;
    style.PopupRounding = 8.0f;

    style.WindowPadding = ImVec2(15, 15);
    style.FramePadding = ImVec2(10, 6);
    style.ItemSpacing = ImVec2(12, 8);

    ImVec4* colors = style.Colors;

    // Glass Backgrounds
    colors[ImGuiCol_WindowBg]             = ImVec4(0.08f, 0.08f, 0.12f, 0.85f);
    colors[ImGuiCol_ChildBg]              = ImVec4(0.12f, 0.12f, 0.16f, 0.40f);

    // Neon Accents (Cyan/Blue)
    colors[ImGuiCol_Header]               = ImVec4(0.00f, 0.75f, 1.00f, 0.30f);
    colors[ImGuiCol_HeaderHovered]        = ImVec4(0.00f, 0.75f, 1.00f, 0.50f);
    colors[ImGuiCol_HeaderActive]         = ImVec4(0.00f, 0.75f, 1.00f, 0.80f);

    colors[ImGuiCol_Button]               = ImVec4(0.15f, 0.15f, 0.25f, 0.60f);
    colors[ImGuiCol_ButtonHovered]        = ImVec4(0.00f, 0.75f, 1.00f, 1.00f);
    colors[ImGuiCol_ButtonActive]         = ImVec4(0.00f, 0.55f, 0.80f, 1.00f);

    colors[ImGuiCol_CheckMark]            = ImVec4(0.00f, 1.00f, 0.80f, 1.00f);
    colors[ImGuiCol_SliderGrab]           = ImVec4(0.00f, 1.00f, 0.80f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.00f, 0.75f, 1.00f, 1.00f);

    // Borders
    colors[ImGuiCol_Border]               = ImVec4(0.20f, 0.20f, 0.30f, 0.50f);
    colors[ImGuiCol_Separator]            = ImVec4(0.20f, 0.20f, 0.30f, 0.50f);
}
