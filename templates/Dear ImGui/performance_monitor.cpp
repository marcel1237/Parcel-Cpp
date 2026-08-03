#include "imgui.h"
#include <math.h>

void RenderPerformanceMonitor() {
    ImGui::Begin("Performance");

    float fps = ImGui::GetIO().Framerate;
    char fps_text[32];
    sprintf(fps_text, "%.1f FPS", fps);

    // Progress bar for FPS (max 144)
    ImGui::Text("Frame Rate");
    ImGui::ProgressBar(fps / 144.0f, ImVec2(-1.0f, 0.0f), fps_text);

    static float values[90] = { 0 };
    static int values_offset = 0;
    static double refresh_time = 0.0;
    if (refresh_time == 0.0) refresh_time = ImGui::GetTime();
    while (refresh_time < ImGui::GetTime()) {
        values[values_offset] = fps;
        values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
        refresh_time += 1.0f / 30.0f;
    }

    ImGui::PlotHistogram("##FPS", values, IM_ARRAYSIZE(values), values_offset, "History", 0.0f, 160.0f, ImVec2(-1, 80));

    ImGui::Separator();
    ImGui::Text("Memory Usage: 240 MB");
    ImGui::Text("GPU Temp: 54°C");

    ImGui::End();
}
