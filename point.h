#pragma once
#include <string>

namespace azbyn {
struct Point {
    int x = 0;
    int y = 0;

    constexpr Point() = default;
    constexpr Point(int x, int y)
            : x(x), y(y) {}
    constexpr bool operator==(const Point& rhs) const {
        return x == rhs.x && y == rhs.y;
    }
    constexpr bool operator!=(const Point& rhs) const {
        return x != rhs.x || y != rhs.y;
    }
    constexpr Point operator+(const Point& rhs) const {
        return Point(x + rhs.x, y + rhs.y);
    }
    constexpr Point operator-(const Point& rhs) const {
        return Point(x - rhs.x, y - rhs.y);
    }
    constexpr Point operator- () const {
        return Point(-x, -y);
    }
    constexpr Point operator+ () const {
        return *this;
    }
    constexpr int RectArea() const {
        return x * y;
    }
    std::string ToString() const {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }
    constexpr bool IsInBounds(Point min, Point max) const {
        return x >= min.x && x < max.x &&
               y >= min.y && y < max.y;
    }


    Point& operator+=(const Point& rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }
    Point& operator-=(const Point& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }
};

} // namespace azbyn
