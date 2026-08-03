#include "imgui.h"
#include <vector>
#include <string>

struct LogEntry {
    ImVec4 color;
    std::string message;
};

void RenderColoredConsole() {
    static std::vector<LogEntry> logs;
    ImGui::Begin("System Console");

    if (ImGui::Button("Clear")) logs.clear();
    ImGui::SameLine();
    if (ImGui::Button("Add Info")) logs.push_back({ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "System initialized successfully."});
    ImGui::SameLine();
    if (ImGui::Button("Add Error")) logs.push_back({ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Critical error at offset 0x442."});

    ImGui::Separator();

    const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);

    for (const auto& entry : logs) {
        ImGui::TextColored(entry.color, "%s", entry.message.c_str());
    }

    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();
    ImGui::End();
}
