#pragma once

#include <ftxui/component/component.hpp>

#include <types/FieldManager.h>

namespace types {
    class GameOfLife {
    public:
        GameOfLife(int height, int width);

        ~GameOfLife() = default;

        void run();

    private:
        FieldManager _fieldManager;
        std::atomic<Point> _mouse;

        ftxui::Element _createCellCanvas() const;

        void _handleNormalKeysEvent(const std::string& keys);

        void _handleMouseEvent(const ftxui::Mouse& mouseEvent);
    };
}
