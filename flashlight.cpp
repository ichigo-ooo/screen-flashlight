struct button_ripple_t {
    ImVec2 position; // position of where we clicked
    float time; // life time
    float eased; // other life time we use to ease our lifetime (pretty stupidly)
};

void menu::frame() {

    static ImVec2 position = { 125, 125 };

    const auto blend_color = [&](ImColor first, ImColor second, float additive) -> ImColor {

        const auto r = std::lerp(first.Value.x, second.Value.x, additive);
        const auto g = std::lerp(first.Value.y, second.Value.y, additive);
        const auto b = std::lerp(first.Value.z, second.Value.z, additive);

        return { r, g, b };
    };

    static std::vector<button_ripple_t> ripples;

    // https://easings.net/#easeInQuad
    const auto quad_ease_in = [&](float v) -> float {
        return (v * v);
    };

    const auto button = [&](ImVec2 position, ImVec2 size, const std::string& label, const ImColor& bg_color, const ImColor& text_color) -> bool {

        const auto hovering = ImGui::IsMouseHoveringRect(position, { position.x + size.x, position.y + size.y }, false);
        const auto clicked = hovering && ImGui::IsMouseClicked(ImGuiMouseButton_Left);

        ImGui::PushFont(menu::small_font);

        const auto text_size = ImGui::CalcTextSize(label.data());
        ImGui::GetBackgroundDrawList()->AddRectFilled(position, { position.x + size.x, position.y + size.y }, bg_color, 3.f);

        // push a clipped rect to keep our circles inside of the button
        ImGui::GetBackgroundDrawList()->PushClipRect(position, { position.x + size.x, position.y + size.y });
        // check if our ripple vector actually has a ripple
        if (!ripples.empty()) {
            // iterate through each ripple and animate the circle with our circ_in function above :D
            auto it = ripples.begin();
            while (it != ripples.end()) {

                // 700ms life time
                it->eased -= (1.f / 0.7f) * ImGui::GetIO().DeltaTime;
                it->time = quad_ease_in(it->eased);

                ImGui::GetBackgroundDrawList()->AddCircleFilled(it->position, 180 * (1.f - it->time), ImColor(text_color.Value.x, text_color.Value.y, text_color.Value.z, it->time * 0.8f));
                ImGui::GetBackgroundDrawList()->AddCircle(it->position, 180 * (1.f - it->time), ImColor(text_color.Value.x, text_color.Value.y, text_color.Value.z, it->time));

                // once you can't see the ripple, remove it.
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

        // if we clicked our button we want to push back where we clicked and also return true.
        if (clicked) {
            ripples.push_back({ ImGui::GetIO().MousePos, 1.f, 1.f });

            return clicked;
        }
    };

    const auto begin_frame = [&](ImVec2 position) -> bool {
        static bool needs_power = true;
        static float power = 0.f;

        // on and off magic https://en.wikipedia.org/wiki/Linear_interpolation
        if (power != needs_power) {
            power += (needs_power - power) * 0.1f;
        }

        const auto size = ImVec2(400, 500);
        const auto bg_color = blend_color(ImColor(0.f, 0.f, 0.f), ImColor(1.f, 1.f, 1.f), power);
        const auto text_color = blend_color(ImColor(1.f, 1.f, 1.f), ImColor(0.f, 0.f, 0.f), power);
        const auto text_size = ImGui::CalcTextSize("Flashlight");

        ImGui::GetBackgroundDrawList()->AddRectFilled(position, { position.x + size.x, position.y + size.y }, bg_color, 3.0f);
        ImGui::GetBackgroundDrawList()->AddRect({ position.x - 1, position.y - 1 }, { position.x + size.x + 1, position.y + size.y + 1 }, text_color, 3.0f);
        ImGui::GetBackgroundDrawList()->AddText(ImVec2(position.x + size.x / 2 - text_size.x / 2, position.y + 15), text_color, "Flashlight");

        // our toggle button
        if (button({ position.x + 15, position.y + size.y - 15 - 40 }, { size.x - 30, 40 }, "Toggle flashlight", text_color, bg_color)) {
            needs_power = !needs_power;
        }

        return true;
    };

    // render everything
    begin_frame(position);
}
