#include "imgui.h"

void RenderMinimalWindow() {
    ImGui::Begin("Parcel Dear ImGui Template");

    ImGui::Text("Welcome to the Minimal Template!");
    ImGui::Separator();

    static float f = 0.0f;
    ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);

    if (ImGui::Button("Click Me")) {
        // Logic here
    }

    ImGui::End();
}
