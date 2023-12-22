#pragma once

#include <array>
#include <vector>

/// \namespace types
/// \brief Contains type definitions and structures used in the project.
namespace types {
    /// \typedef Field
    /// \brief Represents a 2D field of boolean values.
    using Field = std::vector<std::vector<bool>>;

    /// \typedef NeightborRule
    /// \brief Represents the rules for cell survival and death in the Game of Life.
    using NeightborRule = std::array<bool, 9>;

    /// \struct Point
    /// \brief Represents a point in 2D space.
    struct Point {
        int x, y; ///< The x and y coordinates of the point.

        /// \fn Point operator+(const Point& other) const
        /// \brief Adds the coordinates of two points.
        ///
        /// \param other The other point to add.
        /// \return A new point with the added coordinates.
        Point operator+(const Point& other) const {
            return {x + other.x, y + other.y};
        }
    };
}
