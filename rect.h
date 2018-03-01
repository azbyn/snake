#pragma once
#include "point.h"

namespace azbyn {
class Rect {
public:
    virtual int X() const = 0;
    virtual int Y() const = 0;
    virtual int Width() const = 0;
    virtual int Height() const = 0;
    virtual int X1() const = 0;
    virtual int Y1() const = 0;
    inline int X0() const { return X(); }
    inline int Y0() const { return Y(); }
    inline Point P0() const { return {X0(), Y0()}; }
    inline Point P1() const { return {X1(), Y1()}; }

    inline bool operator==(const Rect& rhs) const {
        return X0() == rhs.X0() &&
               Y0() == rhs.Y0() &&
               X1() == rhs.X1() &&
               Y1() == rhs.Y1();
    }
};

class RectWH : public Rect {
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;

public:
    constexpr RectWH(int x, int y, int width, int height)
        : x(x), y(y), w(width), h(height) {}
    inline int X() const override { return x; }
    inline int Y() const override { return y; }
    inline int Width() const override { return w; }
    inline int Height() const override { return h; }
    inline int X1() const override { return x + w - 1; }
    inline int Y1() const override { return y + h - 1; }
};
class Rect4 : public Rect {
    int x0 = 0;
    int y0 = 0;
    int x1 = 0;
    int y1 = 0;

public:
    constexpr Rect4(int x0, int y0, int x1, int y1)
        : x0(x0), y0(y0), x1(x1), y1(y1) {}
    constexpr Rect4(Point p0, Point p1)
        : x0(p0.x), y0(p0.y), x1(p1.x), y1(p1.y) {}
    inline int X() const override { return x0; }
    inline int Y() const override { return y0; }
    inline int Width() const override { return x1 - x0 + 1; }
    inline int Height() const override { return y1 - y0 + 1; }
    inline int X1() const override { return x1; }
    inline int Y1() const override { return y1; }
};

} // namespace azbyn
