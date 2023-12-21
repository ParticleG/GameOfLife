#pragma once
#include <functional>
#include <string>

#include "ftxui/component/component.hpp"

namespace helpers {
    class ButtonHelper {
    public:
        ButtonHelper(
            std::function<std::string()> dynamicLabel,
            std::function<void()> callback,
            ftxui::Color focusColor = ftxui::Color::Default,
            ftxui::Color normalColor = ftxui::Color::Default
        );

        ButtonHelper(
            std::string label,
            std::function<void()> callback,
            ftxui::Color focusColor = ftxui::Color::Default,
            ftxui::Color normalColor = ftxui::Color::Default
        );

        [[nodiscard]] ftxui::Component& component();

        [[nodiscard]] ftxui::Element render() const;

    private:
        ftxui::Component _button;
        std::function<std::string()> _dynamicLabel;
        std::function<void()> _callback;
        std::string _staticLabel;

        [[nodiscard]] ftxui::Component _createButton(
            ftxui::Color focusColor,
            ftxui::Color normalColor
        ) const;
    };
}
