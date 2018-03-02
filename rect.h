#pragma once
#include "point.h"

namespace azbyn {
struct Rect {
    const int x = 0;
    const int y = 0;
    const int width = 0;
    const int height = 0;

    Rect(int x, int y, int width, int height)
        : x(x), y(y), width(width), height(height) {}
    Rect(Point p0, Point p1)
        : x(p0.x), y (p0.y), width(p1.x - p0.x + 1), height(p1.y - p0.y + 1) {}

    inline int X0() const { return x; }
    inline int Y0() const { return y; }
    inline int X1() const { return x + width - 1; }
    inline int Y1() const { return y + height - 1; }

    inline Point P0() const { return {X0(), Y0()}; }
    inline Point P1() const { return {X1(), Y1()}; }

    inline bool operator==(const Rect& rhs) const {
        return x == rhs.x &&
               y == rhs.y &&
               width == rhs.width &&
               height == rhs.height;
    }
};
} // namespace azbyn
