#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include <types/GameOfLife.h>

using namespace ftxui;
using namespace std;
using namespace types;

int main() {
    int mouse_x{}, mouse_y{};
    const auto renderer_plot_1 = Renderer([&] {
        auto c = Canvas(100, 100);

        std::vector<int> ys(100);
        for (int x = 0; x < 100; x++) {
            const auto dx = static_cast<float>(x - mouse_x);
            constexpr float dy = 50.f;
            ys[x] = static_cast<int>(dy + 20 * cos(dx * 0.14) + 10 * sin(dx * 0.42));
        }
        for (int x = 1; x < 99; x++)
            c.DrawPointLine(x, ys[x], x + 1, ys[x + 1]);

        return hflow({canvas(std::move(c))}) | border;
    });

    const auto container = Container::Horizontal({renderer_plot_1});
    auto tab_with_mouse = CatchEvent(container, [&](Event e) {
        if (e.is_mouse()) {
            mouse_x = (e.mouse().x - 1) * 2;
            mouse_y = (e.mouse().y - 1) * 4;
        }
        return false;
    });

    auto screen = ScreenInteractive::Fullscreen();
    screen.Loop(container);
    getchar();

    return 0;
}
