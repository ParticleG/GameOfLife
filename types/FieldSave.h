#pragma once

#include <chrono>

#include <types/common.h>

namespace types {
    class FieldSave {
    public:
        FieldSave() = default;

        ~FieldSave() = default;

        void changeName(const std::string& name);

        [[nodiscard]] std::string getName() const;

        [[nodiscard]] std::tuple<std::string, std::string> getSaveTime() const;

        [[nodiscard]] bool isValid() const;

        [[nodiscard]] std::tuple<Field, NeightborRule, NeightborRule> load() const;

        void reset();

        void save(const Field& field, NeightborRule aliveRule, NeightborRule deadRule);

    private:
        std::chrono::system_clock::time_point _saveTime;
        std::string _name;
        Field _field;
        NeightborRule _aliveRule{}, _deadRule{};
    };
}
