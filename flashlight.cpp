struct button_ripple_t {
    ImVec2 position;
    float time;
    float eased;
};

void menu::frame() {

    static ImVec2 position = { 125, 125 };

    auto blend_color = [&](ImColor first, ImColor second, float additive) -> ImColor {

        const auto r = std::lerp(first.Value.x, second.Value.x, additive);
        const auto g = std::lerp(first.Value.y, second.Value.y, additive);
        const auto b = std::lerp(first.Value.z, second.Value.z, additive);

        return { r, g, b };
    };

    static std::vector<button_ripple_t> ripples;

    auto circ_in = [&](float v) -> float {
        return (v * v);
    };

    auto button = [&](ImVec2 position, ImVec2 size, const std::string& label, const ImColor& bg_color, const ImColor& text_color) -> bool {

        auto hovering = ImGui::IsMouseHoveringRect(position, { position.x + size.x, position.y + size.y }, false);
        auto clicked = hovering && ImGui::IsMouseClicked(ImGuiMouseButton_Left);

        ImGui::PushFont(menu::small_font);

        const auto text_size = ImGui::CalcTextSize(label.data());
        ImGui::GetBackgroundDrawList()->AddRectFilled(position, { position.x + size.x, position.y + size.y }, bg_color, 3.f);
        ImGui::GetBackgroundDrawList()->PushClipRect(position, { position.x + size.x, position.y + size.y });

        if (!ripples.empty()) {
            auto it = ripples.begin();
            while (it != ripples.end()) {

                it->eased -= (1.f / 0.7f) * ImGui::GetIO().DeltaTime;
                it->time = circ_in(it->eased);

                ImGui::GetBackgroundDrawList()->AddCircleFilled(it->position, 180 * (1.f - it->time), ImColor(text_color.Value.x, text_color.Value.y, text_color.Value.z, it->time * 0.8f));
                ImGui::GetBackgroundDrawList()->AddCircle(it->position, 180 * (1.f - it->time), ImColor(text_color.Value.x, text_color.Value.y, text_color.Value.z, it->time));

                if (it->eased <= 0.f) {
                    it = ripples.erase(it);
                }
                else {
                    ++it;
                }
            }
        }
        ImGui::GetBackgroundDrawList()->PopClipRect();

        ImGui::GetBackgroundDrawList()->AddText({ position.x + size.x / 2 - text_size.x / 2, position.y + size.y / 2 - text_size.y / 2 }, text_color, label.data());

        ImGui::PopFont();

        if (clicked) {
            ripples.push_back({ ImGui::GetIO().MousePos, 1.f, 1.f });

            return clicked;
        }
    };

    auto begin_frame = [&](ImVec2 position, bool is_open) -> bool {
        if (!is_open) {
            return false;
        }

        static bool needs_power = true;
        if (GetAsyncKeyState(VK_END) & 1) needs_power = !needs_power;

        static float power = 0.f;

        if (power != needs_power) {
            power += (needs_power - power) * 0.1f;
        }

        const auto size = ImVec2(400, 500);
        const auto bg_color = blend_color(ImColor(0.f, 0.f, 0.f), ImColor(1.f, 1.f, 1.f), power);
        const auto text_color = blend_color(ImColor(1.f, 1.f, 1.f), ImColor(0.f, 0.f, 0.f), power);
        const auto text_size = ImGui::CalcTextSize("Flashlight");

        ImGui::GetBackgroundDrawList()->AddRectFilled(position, { position.x + size.x, position.y + size.y }, bg_color, 3.0f);
        ImGui::GetBackgroundDrawList()->AddRect({position.x - 1, position.y - 1}, {position.x + size.x + 1, position.y + size.y + 1}, text_color, 3.0f);
        ImGui::GetBackgroundDrawList()->AddText(ImVec2(position.x + size.x / 2 - text_size.x / 2, position.y + 15), text_color, "Flashlight");

        if (button({ position.x + 15, position.y + size.y - 15 - 40 }, { size.x - 30, 40 }, "Toggle flashlight", text_color, bg_color)) {
            needs_power = !needs_power;
        }

        return true;
    };


    if (begin_frame(position, true)) {

    }
}
