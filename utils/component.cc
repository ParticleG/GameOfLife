#include <utils/component.h>

using namespace ftxui;
using namespace std;
using namespace utils;

Component component::makeButton(
    function<string()>&& dynamicLabel,
    function<void()>&& callback,
    const Color focusColor,
    const Color normalColor
) {
    ButtonOption option;
    option.transform = [_dynamicLabel = move(dynamicLabel)](const EntryState& s) {
        auto element = text(_dynamicLabel()) | center | border;
        if (s.active) {
            element |= bold;
        }
        return element;
    };
    option.animated_colors.foreground.Set(normalColor, focusColor);
    option.animated_colors.background.Set(Color::Default, Color::Default);

    return Button(
        {},
        move(callback),
        move(option)
    );
}

Component component::makeButton(
    string&& staticLabel,
    function<void()>&& callback,
    const Color focusColor,
    const Color normalColor
) {
    ButtonOption option;
    option.transform = [](const EntryState& s) {
        auto element = text(s.label) | center | border;
        if (s.active) {
            element |= bold;
        }
        return element;
    };
    option.animated_colors.foreground.Set(normalColor, focusColor);
    option.animated_colors.background.Set(Color::Default, Color::Default);

    return Button(
        move(staticLabel),
        move(callback),
        move(option)
    );
}
