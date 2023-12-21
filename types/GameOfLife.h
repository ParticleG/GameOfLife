#pragma once

#include <ftxui/component/component.hpp>

#include <types/FieldManager.h>
#include <types/FieldSave.h>

namespace types {
    class GameOfLife {
    public:
        GameOfLife(int height, int width);

        ~GameOfLife() = default;

        void run();

    private:
        int _panelIndex{2};
        mutable std::shared_mutex _saveListMutex;
        std::atomic<Point> _mouse;
        std::vector<FieldSave> _saveList{50};
        FieldManager _fieldManager;
        Point _fieldSize;

        ftxui::Component _createCellRenderer();

        std::tuple<ftxui::Component, ftxui::Component> _createFieldSizeInputs(
            std::string& heightString,
            std::string& widthString
        );

        ftxui::Component _createPlaybackIntervalInput(std::string& playbackIntervalString);

        void _handleNormalKeysEvent(const std::string& keys);

        void _handleMouseEvent(const ftxui::Mouse& mouseEvent);
    };
}
