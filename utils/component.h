#pragma once
#include "ftxui/component/component.hpp"

namespace utils::component {
    ftxui::Component makeButton(
        std::function<std::string()>&& dynamicLabel,
        std::function<void()>&& callback,
        ftxui::Color focusColor = ftxui::Color::White,
        ftxui::Color normalColor = ftxui::Color::White
    );

    ftxui::Component makeButton(
        std::string&& staticLabel,
        std::function<void()>&& callback,
        ftxui::Color focusColor = ftxui::Color::White,
        ftxui::Color normalColor = ftxui::Color::White
    );
}
