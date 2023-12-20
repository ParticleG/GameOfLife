#include <ranges>

#include <ftxui/component/screen_interactive.hpp>

#include <types/common.h>
#include <types/GameOfLife.h>
#include <utils/console.h>

using namespace std;
using namespace types;
using namespace utils;

namespace {
    constexpr auto cell = "██";

    void drawCell(ftxui::Canvas& c, const Point& mouse, const ftxui::Color color) {
        c.DrawText(4 * mouse.x, 4 * mouse.y, cell, color);
    }
}

GameOfLife::GameOfLife(const int height, const int width) : _fieldManager(height, width) {
    console::setConsoleSize(height + 2, width + 2);
}

void GameOfLife::run() {
    const auto cellRenderArea = ftxui::Renderer([this] {
        return _createCellCanvas();
    });

    const auto cellRenderAreaEventHandler = CatchEvent(cellRenderArea, [this](ftxui::Event e) {
        if (e == ftxui::Event::ArrowRight) {
            _fieldManager.nextIteration();
        } else if (e == ftxui::Event::ArrowLeft) {
            _fieldManager.previousIteration();
        } else if (e.is_character()) {
            _handleNormalKeysEvent(e.character());
        } else if (e.is_mouse()) {
            _handleMouseEvent(e.mouse());
        }
        return false;
    });

    auto screen = ftxui::ScreenInteractive::FitComponent();
    thread([&] {
        while (true) {
            this_thread::sleep_for(chrono::nanoseconds(16666666));
            screen.PostEvent(ftxui::Event::Custom);
        }
    }).detach();
    screen.Loop(cellRenderAreaEventHandler | ftxui::border);
}

ftxui::Element GameOfLife::_createCellCanvas() const {
    const auto field = _fieldManager.getField();
    const auto width = static_cast<int>(field.size());
    const auto height = static_cast<int>(field[0].size());
    const auto mouse = _mouse.load();
    auto c = ftxui::Canvas(width * 4, height * 4);
    for (const auto& [x,y]: views::cartesian_product(
             views::iota(0, width),
             views::iota(0, height))) {
        drawCell(
            c,
            {x, y},
            x == mouse.x && y == mouse.y
                ? ftxui::Color{ftxui::Color::Grey50}
                : field[x][y]
                      ? ftxui::Color{ftxui::Color::White}
                      : ftxui::Color{ftxui::Color::Grey11}
        );
    }

    // // The IDE doesn't like this, but it compiles and works.
    // // Related issue: [CPP-36762](https://youtrack.jetbrains.com/issue/CPP-36762)
    // ranges::for_each(
    //     ranges::views::cartesian_product(
    //         ranges::views::iota(0, _fieldSize.x),
    //         ranges::views::iota(0, _fieldSize.y)
    //     ),
    //     [&](const auto& pair) {
    //         const auto& [x, y] = pair;
    //         drawCell(
    //             c,
    //             {x, y},
    //             x == _mouse.x && y == _mouse.y
    //                 ? ftxui::Color{ftxui::Color::Grey50}
    //                 : _field[x][y]
    //                       ? ftxui::Color{ftxui::Color::White}
    //                       : ftxui::Color{ftxui::Color::Grey11}
    //         );
    //     }
    // );
    return canvas(move(c));
}

void GameOfLife::_handleNormalKeysEvent(const string& keys) {
    switch (keys[0]) {
        case 'c': {
            _fieldManager.reset();
            break;
        }
        case 'r': {
            _fieldManager.randomize();
            break;
        }
        case ' ': {
            _fieldManager.togglePlayPause();
        }
        default: {
            break;
        }
    }
}

void GameOfLife::_handleMouseEvent(const ftxui::Mouse& mouseEvent) {
    const auto& [button, motion, shift, meta, control, x, y] = mouseEvent;
    // Minus 1 for the border.
    const auto mouseInCanvas = Point{(x - 1) / 2, y - 1};
    switch (button) {
        case ftxui::Mouse::Left: {
            _fieldManager.setCell(mouseInCanvas, true);
            break;
        }
        case ftxui::Mouse::Right: {
            _fieldManager.setCell(mouseInCanvas, false);
            break;
        }
        default: {
            break;
        }
    }
    _mouse.store(mouseInCanvas);
}
