#include <helpers/ButtonHelper.h>

using namespace ftxui;
using namespace helpers;
using namespace std;

ButtonHelper::ButtonHelper(function<string()> dynamicLabel, function<void()> callback)
    : _dynamicLabel(move(dynamicLabel)), _callback(move(callback)) {
    _button = _createButton();
}

ButtonHelper::ButtonHelper(string label, function<void()> callback)
    : _callback(move(callback)), _staticLabel(move(label)) {
    _button = _createButton();
}

Component& ButtonHelper::component() {
    return _button;
}

Element ButtonHelper::render() const {
    return _button->Render();
}

void ButtonHelper::setFocusedColor(const Color color) {
    _focusedColor = color;
    _button = _createButton();
}

void ButtonHelper::setNormalColor(const Color color) {
    _normalColor = color;
    _button = _createButton();
}

Component ButtonHelper::_createButton() const {
    return Button(
        _staticLabel,
        _callback,
        [this] {
            ButtonOption option;
            option.transform = [this](const EntryState& s) {
                auto element = text(_staticLabel.empty() ? _dynamicLabel() : s.label) | center | border;
                if (s.active) {
                    element |= bold;
                }
                return element;
            };
            option.animated_colors.foreground.Set(_normalColor, _focusedColor);
            option.animated_colors.background.Set(Color::Default, Color::Default);
            return option;
        }()
    );
}
