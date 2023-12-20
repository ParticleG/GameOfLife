#pragma once

namespace types {
    using Field = std::vector<std::vector<bool>>;

    struct Point {
        int x, y;

        Point operator+(const Point& other) const {
            return {x + other.x, y + other.y};
        }
    };
}
