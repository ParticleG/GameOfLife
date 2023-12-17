#pragma once

namespace types {
    class GameOfLife {
    public:
        GameOfLife(uint64_t height, uint64_t width) = default;
        ~GameOfLife() = default;

        void run();
    };
}
