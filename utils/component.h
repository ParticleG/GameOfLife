#pragma once
#include "ftxui/component/component.hpp"

/// \namespace utils::component
/// \brief Contains utility functions for creating UI components.
namespace utils::component {
    /// \fn ftxui::Component makeButton(std::function<std::string()>&& dynamicLabel, std::function<void()>&& callback, ftxui::Color focusColor = ftxui::Color::White, ftxui::Color normalColor = ftxui::Color::White)
    /// \brief Creates a button with a dynamic label.
    ///
    /// \param dynamicLabel A function object that returns the label of the button.
    /// \param callback A function object that is called when the button is clicked.
    /// \param focusColor The color of the button when it is focused.
    /// \param normalColor The color of the button when it is not focused.
    /// \return A Component object representing the created button.
    ftxui::Component makeButton(
        std::function<std::string()>&& dynamicLabel,
        std::function<void()>&& callback,
        ftxui::Color focusColor = ftxui::Color::White,
        ftxui::Color normalColor = ftxui::Color::White
    );

    /// \fn ftxui::Component makeButton(std::string&& staticLabel, std::function<void()>&& callback, ftxui::Color focusColor = ftxui::Color::White, ftxui::Color normalColor = ftxui::Color::White)
    /// \brief Creates a button with a static label.
    ///
    /// \param staticLabel The static label of the button.
    /// \param callback A function object that is called when the button is clicked.
    /// \param focusColor The color of the button when it is focused.
    /// \param normalColor The color of the button when it is not focused.
    /// \return A Component object representing the created button.
    ftxui::Component makeButton(
        std::string&& staticLabel,
        std::function<void()>&& callback,
        ftxui::Color focusColor = ftxui::Color::White,
        ftxui::Color normalColor = ftxui::Color::White
    );
}
