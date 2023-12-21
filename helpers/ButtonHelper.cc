#include <helpers/ButtonHelper.h>

using namespace ftxui;
using namespace helpers;
using namespace std;

ButtonHelper::ButtonHelper(
    function<string()> dynamicLabel,
    function<void()> callback,
    const Color focusColor,
    const Color normalColor
): _dynamicLabel(move(dynamicLabel)), _callback(move(callback)) {
    _button = _createButton(focusColor, normalColor);
}

ButtonHelper::ButtonHelper(
    string label,
    function<void()> callback,
    const Color focusColor,
    const Color normalColor
): _callback(move(callback)), _staticLabel(move(label)) {
    _button = _createButton(focusColor, normalColor);
}

Component& ButtonHelper::component() {
    return _button;
}

Element ButtonHelper::render() const {
    return _button->Render();
}

Component ButtonHelper::_createButton(const Color focusColor, const Color normalColor) const {
    return Button(
        _staticLabel,
        _callback,
        [this, focusColor, normalColor] {
            ButtonOption option;
            option.transform = [this](const EntryState& s) {
                auto element = text(_staticLabel.empty() ? _dynamicLabel() : s.label) | center | border;
                if (s.active) {
                    element |= bold;
                }
                return element;
            };
            option.animated_colors.foreground.Set(normalColor, focusColor);
            option.animated_colors.background.Set(Color::Default, Color::Default);
            return option;
        }()
    );
}
