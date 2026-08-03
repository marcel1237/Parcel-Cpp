#include "imgui.h"

void RenderPropertyInspector() {
    ImGui::Begin("Inspector");

    static float intensity = 1.0f;
    static char name[128] = "Main Component";
    static bool visible = true;
    static ImVec4 color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

    if (ImGui::BeginTable("Properties", 2, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_Resizable)) {
        ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Value");
        ImGui::TableHeadersRow();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0); ImGui::Text("Name");
        ImGui::TableSetColumnIndex(1); ImGui::InputText("##Name", name, sizeof(name));

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0); ImGui::Text("Intensity");
        ImGui::TableSetColumnIndex(1); ImGui::SliderFloat("##Intensity", &intensity, 0.0f, 2.0f);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0); ImGui::Text("Visible");
        ImGui::TableSetColumnIndex(1); ImGui::Checkbox("##Visible", &visible);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0); ImGui::Text("Color");
        ImGui::TableSetColumnIndex(1); ImGui::ColorEdit4("##Color", (float*)&color);

        ImGui::EndTable();
    }

    ImGui::End();
}
