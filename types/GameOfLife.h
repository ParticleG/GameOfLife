#pragma once

#include <ftxui/component/component.hpp>

#include <types/FieldManager.h>
#include <types/FieldSave.h>

/// \namespace types
/// \brief Contains the GameOfLife class.
namespace types {
    /// \class GameOfLife
    /// \brief Represents a Game of Life simulation.
    ///
    /// This class manages the state of the game, including the field and the rules for cell survival and death.
    /// It also handles user input and updates the display.
    class GameOfLife {
    public:
        /// \fn GameOfLife(int height, int width)
        /// \brief Constructs a new Game of Life simulation with the specified field size.
        ///
        /// \param height The height of the field.
        /// \param width The width of the field.
        GameOfLife(int height, int width);

        /// \fn ~GameOfLife() = default;
        /// \brief Default destructor.
        ~GameOfLife() = default;

        /// \fn void run()
        /// \brief Starts the Game of Life simulation.
        void run();

    private:
        mutable std::shared_mutex _saveListMutex; ///< Mutex for thread-safe access to the save list.
        int _panelIndex{2}; ///< Index of the currently selected panel.
        std::atomic<Point> _currentCellPosition; ///< Current cell position.
        std::vector<FieldSave> _saveList{50}; ///< List of saved fields.
        FieldManager _fieldManager; ///< Manages the field state.
        NeightborRule _aliveRule{}, _deadRule{}; ///< Rules for cell survival and death.
        Point _fieldSize; ///< Size of the field.

        /// \fn ftxui::Component _createCellRenderer()
        /// \brief Creates a component for rendering cells.
        /// \return The created component.
        ftxui::Component _createCellRenderer();

        /// \fn ftxui::Component _createControlButtonContainer()
        /// \brief Creates a component for the control buttons.
        /// \return The created component.
        ftxui::Component _createControlButtonContainer();

        /// \fn std::tuple<ftxui::Component, ftxui::Component> _createFieldSizeInputs(std::string& heightString, std::string& widthString)
        /// \brief Creates input components for the field size.
        ///
        /// \param heightString String to store the height input.
        /// \param widthString String to store the width input.
        /// \return A tuple containing the height and width input components.
        std::tuple<ftxui::Component, ftxui::Component> _createFieldSizeInputs(
            std::string& heightString,
            std::string& widthString
        );

        /// \fn ftxui::Component _createPlaybackIntervalInput(std::string& playbackIntervalString)
        /// \brief Creates an input component for the playback interval.
        ///
        /// \param playbackIntervalString String to store the playback interval input.
        /// \return The created component.
        ftxui::Component _createPlaybackIntervalInput(std::string& playbackIntervalString);

        /// \fn void _handleNormalKeysEvent(const std::string& keys)
        /// \brief Handles normal key events.
        ///
        /// \param keys The keys that were pressed.
        void _handleNormalKeysEvent(const std::string& keys);

        /// \fn void _handleMouseEvent(const ftxui::Mouse& mouseEvent)
        /// \brief Handles mouse events.
        ///
        /// \param mouseEvent The mouse event to handle.
        void _handleMouseEvent(const ftxui::Mouse& mouseEvent);
    };
}