#pragma once

#include <cstdint>
#include <vector>

#include <types/common.h>

namespace types {
    class GameOfLife {
    public:
        GameOfLife(uint64_t height, uint64_t width);

        void randomize();

        void reset();

        ~GameOfLife() = default;

        void run();

    private:
        Point _fieldSize, _mouse;
        std::vector<std::vector<bool>> _field;

        void _updateField();

        uint64_t _countNeighbours(uint64_t x, uint64_t y);
    };
}
