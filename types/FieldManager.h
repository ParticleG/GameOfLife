#pragma once

#include <shared_mutex>

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

        [[nodiscard]] std::tuple<NeightborRule, NeightborRule> getRules();

        void nextIteration();

        void previousIteration();

        void randomize();

        void reset();

        void setAliveRule(int cellCount, bool value);

        void setCell(Point point, bool value);

        void setDeadRule(int cellCount, bool value);

        void setField(const Field& field);

        void setRules(const NeightborRule& aliveRule, const NeightborRule& deadRule);

        void setSize(Point size);

        void setPlaybackInterval(std::chrono::milliseconds interval = std::chrono::milliseconds(250));

    private:
        mutable std::shared_mutex _ruleMutex, _fieldMutex, _historyMutex;
        Field _field;
        NeightborRule _aliveRule{}, _deadRule{};
        std::vector<Field> _history;

        void _playbackThread();

        void _resetRules();
    };
}
