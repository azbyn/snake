#pragma once

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
