#pragma once

#include <shared_mutex>
#include <vector>

#include <types/common.h>

namespace types {
    class FieldManager {
    public:
        std::atomic<bool> isPlaying;
        std::atomic<std::chrono::milliseconds> playbackInterval;

        FieldManager(int height, int width);

        ~FieldManager() = default;

        [[nodiscard]] uint64_t getCellCount() const;

        [[nodiscard]] Field getField() const;

        [[nodiscard]] uint64_t getIteration() const;

        void nextIteration();

        void previousIteration();

        void randomize();

        void reset();

        void setAliveRule(int cellCount, bool value);

        void setCell(Point point, bool value);

        void setDeadRule(int cellCount, bool value);

        void setField(const Field& field);

        void setSize(Point size);

        void setPlaybackInterval(std::chrono::milliseconds interval = std::chrono::milliseconds(250));

    private:
        mutable std::shared_mutex _ruleMutex, _fieldMutex, _historyMutex;
        Field _field;
        std::array<bool, 9> _aliveRule{}, _deadRule{};
        std::vector<Field> _history;

        void _playbackThread();

        void _resetRules();
    };
}
