#pragma once

namespace types {
    using Field = std::vector<std::vector<bool>>;
    using NeightborRule = std::array<bool, 9>;

    struct Point {
        int x, y;

        Point operator+(const Point& other) const {
            return {x + other.x, y + other.y};
        }
    };
}
