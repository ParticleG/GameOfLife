#include <algorithm>
#include <random>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include <types/GameOfLife.h>

using namespace std;
using namespace types;

namespace {
    constexpr auto cell = "██";

    void drawCell(ftxui::Canvas& c, const Point& mouse, const ftxui::Color color) {
        c.DrawText(4 * mouse.x, 4 * mouse.y, cell, color);
    }
}

GameOfLife::GameOfLife(const uint64_t height, const uint64_t width) : _fieldSize(width, height) {
    _field.resize(height);
    ranges::for_each(
        _field,
        [&](auto& sub) {
            sub.resize(width);
        }
    );
}

void GameOfLife::randomize() {
    mt19937 gen(random_device{}());
    uniform_int_distribution distrib(0, 1);
    ranges::for_each(
        _field,
        [&](auto& sub) {
            ranges::generate(sub.begin(), sub.end(), [&] {
                return distrib(gen);
            });
        }
    );
}

void GameOfLife::reset() {
    ranges::for_each(
        _field,
        [&](auto& sub) {
            ranges::fill(sub, false);
        }
    );
}

void GameOfLife::run() {
    const auto cellRenderArea = ftxui::Renderer([this] {
        auto c = ftxui::Canvas(_fieldSize.x * 4, _fieldSize.y * 4);
        for (int row = 0; row < _fieldSize.y; ++row) {
            for (int column = 0; column < _fieldSize.x; ++column) {
                drawCell(
                    c,
                    {column, row},
                    row == _mouse.y && column == _mouse.x
                        ? ftxui::Color{ftxui::Color::Blue}
                        : _field[row][column]
                              ? ftxui::Color{ftxui::Color::White}
                              : ftxui::Color{ftxui::Color::Grey11}
                );
            }
        }
        return canvas(std::move(c));
    });

    const auto cellRenderAreaEventHandler = CatchEvent(cellRenderArea, [this](ftxui::Event e) {
        if (e.is_mouse()) {
            const auto& [button, motion, shift, meta, control, x, y] = e.mouse();
            // Minus 1 for the border.
            _mouse = {(x - 1) / 2, y - 1};
            if (_mouse.x >= 0 && _mouse.x < _fieldSize.x &&
                _mouse.y >= 0 && _mouse.y < _fieldSize.y) {
                switch (button) {
                    case ftxui::Mouse::Left: {
                        _field[_mouse.y][_mouse.x] = true;
                        break;
                    }
                    case ftxui::Mouse::Right: {
                        _field[_mouse.y][_mouse.x] = false;
                        break;
                    }
                    default: {
                        break;
                    }
                }
            }
        } else if (e.is_character()) {
            switch (e.character()[0]) {
                case 'c': {
                    reset();
                    break;
                }
                case 'r': {
                    randomize();
                    break;
                }
                case ' ': {
                    _updateField();
                }
                default: {
                    break;
                }
            }
        }
        return false;
    });

    auto screen = ftxui::ScreenInteractive::FitComponent();
    screen.Loop(cellRenderAreaEventHandler | ftxui::border);
}

void GameOfLife::_updateField() {
    auto newField(_field);

    for (uint64_t i = 0; i < _field.size(); ++i) {
        for (uint64_t j = 0; j < _field[i].size(); ++j) {
            const auto neighbours = _countNeighbours(i, j);

            if (_field[i][j]) {
                if (neighbours < 2 || neighbours > 3) {
                    newField[i][j] = false;
                }
            } else {
                if (neighbours == 3) {
                    newField[i][j] = true;
                }
            }
        }
    }

    _field = newField;
}

uint64_t GameOfLife::_countNeighbours(const uint64_t x, const uint64_t y) {
    uint64_t count = 0;

    for (int64_t i = -1; i <= 1; ++i) {
        for (int64_t j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) {
                continue;
            }

            const auto xCoord = static_cast<int64_t>(x) + i;
            const auto yCoord = static_cast<int64_t>(y) + j;

            if (xCoord < 0 || yCoord < 0) {
                continue;
            }

            if (xCoord >= static_cast<int64_t>(_field.size()) ||
                yCoord >= static_cast<int64_t>(_field[xCoord].size())) {
                continue;
            }

            if (_field[xCoord][yCoord]) {
                ++count;
            }
        }
    }

    return count;
}
