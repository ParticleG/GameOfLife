#include <algorithm>
#include <random>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include <types/FieldManager.h>

using namespace std;
using namespace types;

FieldManager::FieldManager(const int height, const int width)
    : playbackInterval(chrono::milliseconds(250)) {
    _field.resize(width);
    ranges::for_each(
        _field,
        [&](auto& sub) {
            sub.resize(height);
        }
    );
    _playbackThread();
}

Field FieldManager::getField() const {
    shared_lock lock(_fieldMutex);
    return _field;
}

uint64_t FieldManager::getIteration() const {
    shared_lock lock(_historyMutex);
    return _history.size();
}

void FieldManager::nextIteration() {
    vector<vector<int>> neighbourCountField;
    Field nextField; {
        shared_lock fieldLock(_fieldMutex);
        unique_lock historyLock(_historyMutex);
        nextField = _field;
        _history.push_back(_field);
    }
    neighbourCountField.resize(nextField.size());
    ranges::for_each(
        neighbourCountField,
        [&](auto& sub) {
            sub.resize(nextField[0].size());
        }
    );
    for (int x = 0; x < nextField.size(); ++x) {
        for (int y = 0; y < nextField[x].size(); ++y) {
            int neighbours{};
            for (int64_t dx = -1; dx <= 1; ++dx) {
                for (int64_t dy = -1; dy <= 1; ++dy) {
                    const auto xCoord = x + dx;
                    const auto yCoord = y + dy;

                    if (!(dx == 0 && dy == 0) &&
                        xCoord >= 0 && yCoord >= 0 &&
                        xCoord < static_cast<int64_t>(nextField.size()) &&
                        yCoord < static_cast<int64_t>(nextField[xCoord].size()) &&
                        _field[xCoord][yCoord]) {
                        ++neighbours;
                    }
                }
            }
            neighbourCountField[x][y] = neighbours;
        }
    }

    for (uint64_t x = 0; x < neighbourCountField.size(); ++x) {
        for (uint64_t y = 0; y < neighbourCountField[x].size(); ++y) {
            const auto& currentNeighbours = neighbourCountField[x][y];
            nextField[x][y] = (nextField[x][y] && (currentNeighbours == 2 || currentNeighbours == 3)) ||
                              (!nextField[x][y] && currentNeighbours == 3);
        }
    } {
        unique_lock lock(_fieldMutex);
        _field = nextField;
    }
}

void FieldManager::previousIteration() {
    unique_lock lock(_historyMutex);
    if (_history.empty()) {
        return;
    }
    _field = _history.back();
    _history.pop_back();
}

void FieldManager::randomize() {
    mt19937 gen(random_device{}());
    uniform_int_distribution distrib(0, 1); {
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
    unique_lock lock(_historyMutex);
    _history.clear();
}

void FieldManager::reset() { {
        unique_lock lock(_fieldMutex);
        ranges::for_each(
            _field,
            [&](auto& sub) {
                ranges::fill(sub, false);
            }
        );
    }
    unique_lock lock(_historyMutex);
    _history.clear();
}

void FieldManager::setCell(const Point point, const bool value) {
    try {
        {
            unique_lock lock(_fieldMutex);
            _field.at(point.x).at(point.y) = value;
        }
        unique_lock lock(_historyMutex);
        _history.clear();
    } catch (...) {}
}

void FieldManager::setField(const Point size) { {
        unique_lock lock(_fieldMutex);
        _field.resize(size.x);
        ranges::for_each(
            _field,
            [&](auto& sub) {
                sub.resize(size.y);
            }
        );
    }
    unique_lock lock(_historyMutex);
    _history.clear();
}

void FieldManager::setPlaybackInterval(const std::chrono::milliseconds interval) {
    playbackInterval.store(interval);
}

void FieldManager::_playbackThread() {
    thread([this] {
        while (true) {
            if (isPlaying.load()) {
                nextIteration();
            }
            this_thread::sleep_for(playbackInterval.load());
        }
    }).detach();
}
