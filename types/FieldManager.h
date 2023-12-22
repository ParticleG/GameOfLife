#pragma once

#include <shared_mutex>
#include <types/common.h>

namespace types {
    /// \class FieldManager
    /// \brief Class to manage the field in a game or simulation.
    class FieldManager {
    public:
        /// \brief Indicates whether the game or simulation is currently playing.
        std::atomic<bool> isPlaying;
        /// \brief The interval between each playback in the game or simulation.
        std::atomic<std::chrono::milliseconds> playbackInterval;

        /// \brief Constructor that initializes the field with the given height and width.
        FieldManager(int height, int width);

        /// \brief Default destructor.
        ~FieldManager() = default;

        /// \brief Returns the total number of cells in the field.
        [[nodiscard]] uint64_t getCellCount() const;

        /// \brief Returns the current state of the field.
        [[nodiscard]] Field getField() const;

        /// \brief Returns the current iteration of the game or simulation.
        [[nodiscard]] uint64_t getIteration() const;

        /// \brief Returns the rules for alive and dead cells.
        [[nodiscard]] std::tuple<NeightborRule, NeightborRule> getRules();

        /// \brief Advances the game or simulation to the next iteration.
        void nextIteration();

        /// \brief Reverts the game or simulation to the previous iteration.
        void previousIteration();

        /// \brief Randomizes the state of the field.
        void randomize();

        /// \brief Resets the state of the field.
        void reset();

        /// \brief Sets the rule for when a cell should be alive based on the number of neighboring cells.
        void setAliveRule(int cellCount, bool value);

        /// \brief Sets the state of a cell at a specific point in the field.
        void setCell(Point point, bool value);

        /// \brief Sets the rule for when a cell should be dead based on the number of neighboring cells.
        void setDeadRule(int cellCount, bool value);

        /// \brief Sets the state of the field.
        void setField(const Field& field);

        /// \brief Sets the rules for alive and dead cells.
        void setRules(const NeightborRule& aliveRule, const NeightborRule& deadRule);

        /// \brief Sets the size of the field.
        void setSize(Point size);

        /// \brief Sets the interval between each playback in the game or simulation.
        void setPlaybackInterval(std::chrono::milliseconds interval = std::chrono::milliseconds(250));

    private:
        /// \brief Mutexes for thread-safe access to the rules, field, and history.
        mutable std::shared_mutex _ruleMutex, _fieldMutex, _historyMutex;
        /// \brief The current state of the field.
        Field _field;
        /// \brief The rules for when a cell should be alive or dead.
        NeightborRule _aliveRule{}, _deadRule{};
        /// \brief The history of the field's state.
        std::vector<Field> _history;

        /// \brief Thread for handling the playback of the game or simulation.
        void _playbackThread();

        /// \brief Resets the rules for when a cell should be alive or dead.
        void _resetRules();
    };
}