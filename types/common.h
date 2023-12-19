#pragma once

namespace types {
    struct Point {
        int x{}, y{};

        Point() = default;

        Point(const int x, const int y) : x(x), y(y) {}

        Point(const uint64_t x, const uint64_t y) : x(static_cast<int>(x)), y(static_cast<int>(y)) {}

        Point operator+(const Point& other) const {
            return {x + other.x, y + other.y};
        }
    };
}
