#pragma once

#include <chrono>

#include <types/common.h>

namespace types {
    /// \class FieldSave
    /// \brief Class to manage the saving and loading of field states in a game or simulation.
    class FieldSave {
    public:
        /// \brief Default constructor.
        FieldSave() = default;

        /// \brief Default destructor.
        ~FieldSave() = default;

        /// \brief Changes the name of the save.
        void changeName(const std::string& name);

        /// \brief Returns the name of the save.
        [[nodiscard]] std::string getName() const;

        /// \brief Returns the time the save was created.
        [[nodiscard]] std::tuple<std::string, std::string> getSaveTime() const;

        /// \brief Checks if the save is valid.
        [[nodiscard]] bool isValid() const;

        /// \brief Loads the saved field state, alive rule, and dead rule.
        [[nodiscard]] std::tuple<Field, NeightborRule, NeightborRule> load() const;

        /// \brief Resets the save.
        void reset();

        /// \brief Saves the current field state, alive rule, and dead rule.
        void save(const Field& field, NeightborRule aliveRule, NeightborRule deadRule);

    private:
        /// \brief The time the save was created.
        std::chrono::system_clock::time_point _saveTime;
        /// \brief The name of the save.
        std::string _name;
        /// \brief The saved field state.
        Field _field;
        /// \brief The saved alive rule.
        NeightborRule _aliveRule{};
        /// \brief The saved dead rule.
        NeightborRule _deadRule{};
    };
}