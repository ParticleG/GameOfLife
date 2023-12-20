#pragma once

#include <shared_mutex>
#include <stack>
#include <vector>

#include <types/common.h>

namespace types {
    class FieldManager {
    public:
        FieldManager(int height, int width);

        ~FieldManager() = default;

        [[nodiscard]] Field getField() const;

        void nextIteration();

        void pause();

        void play();

        void previousIteration();

        void randomize();

        void reset();

        void setCell(Point point, bool value);

        void setPlaybackInterval(std::chrono::milliseconds interval = std::chrono::milliseconds(250));

        void togglePlayPause();

    private:
        mutable std::shared_mutex _fieldMutex, _historyMutex;
        Field _field;
        Point _fieldSize;
        std::atomic<bool> _isPlaying;
        std::atomic<std::chrono::milliseconds> _playbackInterval;
        std::vector<Field> _history;

        void _playbackThread();

        void _resetHistory();
    };
}
