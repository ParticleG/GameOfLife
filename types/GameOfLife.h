#pragma once

#include <cstdint>
#include <vector>

namespace types {
    class GameOfLife {
    public:
        GameOfLife(uint64_t height, uint64_t width);

        ~GameOfLife() = default;

        void run();

    private:
        std::vector<std::vector<bool>> _field;

        void _resetField();

        void _printField() const;

        void _updateField();

        uint64_t _countNeighbours(uint64_t x, uint64_t y);
    };
}
