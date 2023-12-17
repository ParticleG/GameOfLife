#include <algorithm>
#include <iostream>
#include <random>

#include <types/GameOfLife.h>

using namespace std;
using namespace types;

GameOfLife::GameOfLife(const uint64_t height, const uint64_t width) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(0, 1);

    for (uint64_t i = 0; i < height; ++i) {
        // Insert random values into the field
        _field.emplace_back(width);
        ranges::generate(
            _field[i],
            [&distrib, &gen] {
                return distrib(gen);
            }
        );
    }
}

void GameOfLife::run() {
    _printField();
    _updateField();
}

void GameOfLife::_resetField() {
    ranges::for_each(
        _field,
        [](auto& sub) {
            fill(sub.begin(), sub.end(), false);
        }
    );
}

void GameOfLife::_printField() const {
    for (const auto& sub: _field) {
        for (const auto& cell: sub) {
            cout << (cell ? "+" : ".");
        }
        cout << endl;
    }
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
