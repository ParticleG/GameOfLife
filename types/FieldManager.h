#pragma once

#include <shared_mutex>
#include <vector>

#include <types/common.h>

namespace types {
    class FieldManager {
    public:
        FieldManager(int height, int width);

        ~FieldManager() = default;

        [[nodiscard]] Field getField() const;

        void nextIteration();

        void updateCell(Point point, bool value);

        void randomize();

        void reset();

    private:
        mutable std::shared_mutex _fieldMutex;
        Field _field;
        Point _fieldSize;

    };
}
