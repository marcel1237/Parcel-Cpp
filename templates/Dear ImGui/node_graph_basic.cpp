#include "imgui.h"

void RenderSimpleNodeGraph() {
    ImGui::Begin("Node Graph Workspace");

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
    ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
    if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
    if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
    ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

    // Draw background grid
    draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(30, 30, 40, 255));
    for (float x = 0; x < canvas_sz.x; x += 40.0f)
        draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(50, 50, 60, 255));
    for (float y = 0; y < canvas_sz.y; y += 40.0f)
        draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(50, 50, 60, 255));

    // Example Node
    ImVec2 node_pos = ImVec2(canvas_p0.x + 100, canvas_p0.y + 100);
    ImVec2 node_sz = ImVec2(150, 80);
    draw_list->AddRectFilled(node_pos, ImVec2(node_pos.x + node_sz.x, node_pos.y + node_sz.y), IM_COL32(60, 60, 70, 255), 5.0f);
    draw_list->AddRect(node_pos, ImVec2(node_pos.x + node_sz.x, node_pos.y + node_sz.y), IM_COL32(100, 100, 255, 255), 5.0f, 0, 2.0f);

    ImGui::SetCursorScreenPos(ImVec2(node_pos.x + 10, node_pos.y + 5));
    ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "Logic Node");

    ImGui::End();
}
