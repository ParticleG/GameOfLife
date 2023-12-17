#include <iostream>

#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

#include <types/GameOfLife.h>

#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"

using namespace ftxui;
using namespace std;
using namespace types;

int main() {
    using namespace ftxui;
    auto make_box = [](int dimx, int dimy) {
        std::string title = std::to_string(dimx) + "x" + std::to_string(dimy);
        return window(text(title) | hcenter | bold,
                      text("content") | hcenter | dim) |
               size(WIDTH, EQUAL, dimx) | size(HEIGHT, EQUAL, dimy);
    };

    auto style = size(WIDTH, GREATER_THAN, 20) | border |
                 size(HEIGHT, GREATER_THAN, 30);

    auto document = hflow({
                        make_box(7, 7),
                        make_box(7, 5),
                        make_box(5, 7),
                        make_box(10, 4),
                        make_box(10, 4),
                        make_box(10, 4),
                        make_box(10, 4),
                        make_box(11, 4),
                        make_box(11, 4),
                        make_box(11, 4),
                        make_box(11, 4),
                        make_box(12, 4),
                        make_box(12, 5),
                        make_box(12, 4),
                        make_box(13, 4),
                        make_box(13, 3),
                        make_box(13, 3),
                        make_box(10, 3),
                    }) |
                    style;

    auto screen = ScreenInteractive::Fullscreen();
    // auto screen = Screen::Create(Dimension::Full(), Dimension::Full());
    // Render(screen, document);
    screen.Loop(Renderer([&]() {
        return document;
    }));
    getchar();

    return 0;
}
