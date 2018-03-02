#pragma once
#include "point.h"
#include "rect.h"

#include <curses.h>

namespace azbyn {

namespace profanity {
constexpr unsigned COL_BRIGHT = 1 << 3;
enum Color : short {
    // clang-format off
    //Color,         //irgb
    COL_BLACK        = COLOR_BLACK,
    COL_DARK_BLUE    = COLOR_BLUE,
    COL_DARK_GREEN   = COLOR_GREEN,
    COL_DARK_CYAN    = COLOR_CYAN,
    COL_DARK_RED     = COLOR_RED,
    COL_DARK_MAGENTA = COLOR_MAGENTA,
    COL_DARK_YELLOW  = COLOR_YELLOW,
    COL_LIGHT_GRAY   = COLOR_WHITE,
    COL_DARK_GRAY    = COL_BRIGHT | COLOR_BLACK,
    COL_BLUE         = COL_BRIGHT | COLOR_BLUE,
    COL_GREEN        = COL_BRIGHT | COLOR_GREEN,
    COL_CYAN         = COL_BRIGHT | COLOR_CYAN,
    COL_RED          = COL_BRIGHT | COLOR_RED,
    COL_MAGENTA      = COL_BRIGHT | COLOR_MAGENTA,
    COL_YELLOW       = COL_BRIGHT | COLOR_YELLOW,
    COL_WHITE        = COL_BRIGHT | COLOR_WHITE,
    // clang-format on
    COL_DARK_WHITE = COL_LIGHT_GRAY,
    COL_LIGHT_BLACK = COL_DARK_GRAY,
#if defined _WIN32 || defined _WIN64
    COL_ORANGE = COL_DARK_YELLOW,
#elif defined BASE16
    COL_ORANGE = 16,
#else
    COL_ORANGE = 202,
#endif
};
inline void setcol(short c) {
    attron(COLOR_PAIR(c));
}
inline void coladdstr(short col, const char* str) {
    setcol(col);
    addstr(str);
}
inline void mvcoladdstr(int y, int x, short col, const char* str) {
    setcol(col);
    mvaddstr(y, x, str);
}

inline void addline(int y, int x, int len) {
    mvhline(y, x, ' ', len);
}
inline void coladdline(short col, int y, int x, int len) {
    setcol(col);
    addline(y, x, len);
}
inline void addblock(int y, int x) {
    mvhline(y, x, ' ', 2);
}
inline void coladdblock(short col, int y, int x) {
    setcol(col);
    addblock(y, x);
}
void fill(Rect r) {
    for (int y = r.Y0(); y <= r.Y1(); ++y)
        addline(y, r.x, r.width);
}
inline void colfill(short col, Rect r) {
    setcol(col);
    fill(r);
}
void addvline(int y, int x, int len) {
    for (int i = y; i < y + len; ++i)
        addblock(i, x);
}
inline void coladdvline(short col, int y, int x, int len) {
    setcol(col);
    addvline(y, x, len);
}
void addborder(Rect r) {
    addline(r.Y0() - 1, r.X0() - 2, r.width + 4);
    addline(r.Y1() + 1, r.X0() - 2, r.width + 4);
    addvline(r.Y0(), r.X0() - 2, r.height + 1);
    addvline(r.Y0(), r.X1() + 1, r.height + 1);
}
inline void coladdborder(short col, Rect r) {
    setcol(col);
    addborder(r);
}
void addbox(short borderCol, short insideCol, Rect r) {
    coladdborder(borderCol, r);
    colfill(insideCol, r);
}

} // namespace profanity
} // namespace azbyn
