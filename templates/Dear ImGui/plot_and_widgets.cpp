#include "imgui.h"
#include <vector>

void RenderAdvancedWidgets() {
    ImGui::Begin("Dashboard");

    static float values[7] = { 0.2f, 0.1f, 1.0f, 0.5f, 0.9f, 2.2f, 0.7f };
    ImGui::PlotLines("Realtime Data", values, IM_ARRAYSIZE(values));

    static int item_current = 0;
    const char* items[] = { "Apple", "Banana", "Cherry", "Kiwi", "Mango" };
    ImGui::Combo("Select Fruit", &item_current, items, IM_ARRAYSIZE(items));

    static bool check = true;
    ImGui::Checkbox("Enable Processing", &check);

    ImGui::End();
}
