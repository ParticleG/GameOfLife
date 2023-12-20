#include <algorithm>
#include <random>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include <types/FieldManager.h>

using namespace std;
using namespace types;

FieldManager::FieldManager(const int height, const int width)
    : _fieldSize(width, height), _playbackInterval(chrono::milliseconds(250)) {
    _field.resize(width);
    ranges::for_each(
        _field,
        [&](auto& sub) {
            sub.resize(height);
        }
    );
    _history.emplace_back(_field);
    _playbackThread();
}

Field FieldManager::getField() const {
    shared_lock lock(_fieldMutex);
    return _field;
}

void FieldManager::nextIteration() {
    Field newField; {
        shared_lock lock(_fieldMutex);
        newField = _field;
        // TODO: Simplify this
        for (uint64_t x = 0; x < _field.size(); ++x) {
            for (uint64_t y = 0; y < _field[x].size(); ++y) {
                uint64_t neighbours{};
                for (int64_t i = -1; i <= 1; ++i) {
                    for (int64_t j = -1; j <= 1; ++j) {
                        const auto xCoord = static_cast<int64_t>(x) + i;
                        const auto yCoord = static_cast<int64_t>(y) + j;

                        if (!(i == 0 && j == 0) &&
                            xCoord >= 0 && yCoord >= 0 &&
                            xCoord < static_cast<int64_t>(_field.size()) &&
                            yCoord < static_cast<int64_t>(_field[xCoord].size()) &&
                            _field[xCoord][yCoord]) {
                            ++neighbours;
                        }
                    }
                }
                // TODO: Make this customizable
                newField[x][y] = (_field[x][y] && (neighbours == 2 || neighbours == 3)) ||
                                 (!_field[x][y] && neighbours == 3);
            }
        }
    } {
        unique_lock fieldLock(_fieldMutex);
        unique_lock historyLock(_historyMutex);
        _field = newField;
        _history.emplace_back(move(newField));
    }
}

void FieldManager::pause() {
    _isPlaying.store(false);
}

void FieldManager::play() {
    _isPlaying.store(true);
}

void FieldManager::previousIteration() {}

void FieldManager::randomize() {
    mt19937 gen(random_device{}());
    uniform_int_distribution distrib(0, 1);
    {
        unique_lock lock(_fieldMutex);
        ranges::for_each(
            _field,
            [&](auto& sub) {
                ranges::generate(sub.begin(), sub.end(), [&] {
                    return distrib(gen);
                });
            }
        );
    }
    _resetHistory();
}

void FieldManager::reset() {
    {
        unique_lock lock(_fieldMutex);
        ranges::for_each(
            _field,
            [&](auto& sub) {
                ranges::fill(sub, false);
            }
        );
    }
    _resetHistory();
}

void FieldManager::setCell(const Point point, const bool value) {
    try {
        {
            unique_lock lock(_fieldMutex);
            _field.at(point.x).at(point.y) = value;
        }
        _resetHistory();
    } catch (...) {}
}

void FieldManager::setPlaybackInterval(const std::chrono::milliseconds interval) {
    _playbackInterval.store(interval);
}

void FieldManager::togglePlayPause() {
    _isPlaying.store(!_isPlaying.load());
}

void FieldManager::_playbackThread() {
    thread([this] {
        while (true) {
            if (_isPlaying.load()) {
                nextIteration();
            }
            this_thread::sleep_for(_playbackInterval.load());
        }
    }).detach();
}

void FieldManager::_resetHistory() {
    bool needClearHistory = false; {
        shared_lock lock(_historyMutex);
        if (_history.size() > 1) {
            needClearHistory = true;
        }
    }
    shared_lock fieldLock(_fieldMutex);
    unique_lock historyLock(_historyMutex);
    if (needClearHistory) {
        _history.clear();
        _history.emplace_back(_field);
    } else {
        _history.at(0) = _field;
    }
}
