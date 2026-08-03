#include "imgui.h"
#include <vector>
#include <string>

struct Toast {
    std::string title;
    std::string message;
    float lifetime;
    ImVec4 color;
};

void RenderToastNotifications() {
    static std::vector<Toast> toasts;
    // For demo purposes:
    if (ImGui::IsKeyPressed(ImGuiKey_F1))
        toasts.push_back({"Success", "Project built successfully!", 5.0f, ImVec4(0.2f, 0.8f, 0.2f, 1.0f)});

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 work_pos = viewport->WorkPos;
    ImVec2 work_size = viewport->WorkSize;

    for (size_t i = 0; i < toasts.size(); i++) {
        Toast& t = toasts[i];
        t.lifetime -= ImGui::GetIO().DeltaTime;
        if (t.lifetime <= 0) {
            toasts.erase(toasts.begin() + i);
            i--;
            continue;
        }

        ImGui::SetNextWindowPos(ImVec2(work_pos.x + work_size.x - 310, work_pos.y + 40 + (i * 90)));
        ImGui::SetNextWindowSize(ImVec2(300, 80));
        ImGui::SetNextWindowBgAlpha(0.9f);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

        char id[32]; sprintf(id, "##toast_%zu", i);
        if (ImGui::Begin(id, nullptr, flags)) {
            ImGui::TextColored(t.color, "● %s", t.title.c_str());
            ImGui::Separator();
            ImGui::TextWrapped("%s", t.message.c_str());
        }
        ImGui::End();
    }
}
