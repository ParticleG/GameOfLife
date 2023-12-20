#pragma once

#include <chrono>

#include <types/common.h>

namespace types {
    class FieldSave {
    public:
        FieldSave() = default;

        FieldSave(std::string name, Field field);

        ~FieldSave() = default;

        void changeName(const std::string& name);

        [[nodiscard]] std::string getName() const;

        [[nodiscard]] std::tuple<std::string, std::string> getSaveTime() const;

        [[nodiscard]] Field load() const;

        void save(const Field& field);

    private:
        std::chrono::system_clock::time_point _saveTime;
        std::string _name;
        Field _field;
    };
}
