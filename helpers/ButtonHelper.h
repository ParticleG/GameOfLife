#pragma once
#include <functional>
#include <string>

#include "ftxui/component/component.hpp"

namespace helpers {
    class ButtonHelper {
    public:
        ButtonHelper(
            std::function<std::string()> dynamicLabel,
            std::function<void()> callback
        );

        ButtonHelper(
            std::string label,
            std::function<void()> callback
        );

        [[nodiscard]] ftxui::Component& component();

        [[nodiscard]] ftxui::Element render() const;

        void setFocusedColor(ftxui::Color color);

        void setNormalColor(ftxui::Color color);

    private:
        ftxui::Color _focusedColor, _normalColor;
        ftxui::Component _button;
        std::function<std::string()> _dynamicLabel;
        std::function<void()> _callback;
        std::string _staticLabel;

        [[nodiscard]] ftxui::Component _createButton() const;
    };
}
