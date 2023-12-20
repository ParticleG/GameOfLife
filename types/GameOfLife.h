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
        Point _fieldSize;
        int _panelIndex{1};
        std::atomic<Point> _mouse;

        ftxui::Component _createCellRenderer();

        std::tuple<ftxui::Component, ftxui::Component> _createFieldSizeInputs(
            std::string& heightString,
            std::string& widthString
        );

        ftxui::Component _createPlaybackIntervalInput(std::string& playbackIntervalString);

        std::tuple<ftxui::Component, ftxui::Component, ftxui::Component> _createControlButtons();

        ftxui::Component _createRandomizeButton();

        ftxui::Component _createResetButton();

        void _handleNormalKeysEvent(const std::string& keys);

        void _handleMouseEvent(const ftxui::Mouse& mouseEvent);
    };
}
