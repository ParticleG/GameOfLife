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

        [[nodiscard]] Field getField() const;

        [[nodiscard]] uint64_t getIteration() const;

        void nextIteration();

        void previousIteration();

        void randomize();

        void reset();

        void setCell(Point point, bool value);

        void setField(Point size);

        void setPlaybackInterval(std::chrono::milliseconds interval = std::chrono::milliseconds(250));

    private:
        mutable std::shared_mutex _fieldMutex, _historyMutex;
        Field _field;
        std::vector<Field> _history;

        void _playbackThread();
    };
}
