#pragma once
#include "point.h"

#include <curses.h>

#if defined _WIN32 || defined _WIN64
#define __WINDOWS__
#endif

namespace azbyn {
namespace prophanity {
constexpr unsigned COL_BRIGHT = 1 << 3;
enum Color : short {
    //clang-format off
    //Color,         //irgb
    COL_BLACK = COLOR_BLACK,
    COL_DARK_BLUE = COLOR_BLUE,
    COL_DARK_GREEN = COLOR_GREEN,
    COL_DARK_CYAN = COLOR_CYAN,
    COL_DARK_RED = COLOR_RED,
    COL_DARK_MAGENTA = COLOR_MAGENTA,
    COL_DARK_YELLOW = COLOR_YELLOW,
    COL_LIGHT_GRAY = COLOR_WHITE,
    COL_DARK_GRAY = COL_BRIGHT | COLOR_BLACK,
    COL_BLUE = COL_BRIGHT | COLOR_BLUE,
    COL_GREEN = COL_BRIGHT | COLOR_GREEN,
    COL_CYAN = COL_BRIGHT | COLOR_CYAN,
    COL_RED = COL_BRIGHT | COLOR_RED,
    COL_MAGENTA = COL_BRIGHT | COLOR_MAGENTA,
    COL_YELLOW = COL_BRIGHT | COLOR_YELLOW,
    COL_WHITE = COL_BRIGHT | COLOR_WHITE,
    //clang-format on
    COL_DARK_WHITE = COL_LIGHT_GRAY,
    COL_LIGHT_BLACK = COL_DARK_GRAY,
#ifdef __WINDOWS__
    COL_ORANGE = COL_DARK_YELLOW,
#else
    COL_ORANGE = 202
#endif
};

inline void coladdstr(short col, const char* str) {
    attron(COLOR_PAIR(col));
    addstr(str);
}
inline void mvcoladdstr(int y, int x, short col, const char* str) {
    attron(COLOR_PAIR(col));
    mvaddstr(y, x, str);
}

void drawBox(short outsideCol, short insideCol, int x0, int y0, int x1, int y1) {
    attron(COLOR_PAIR(outsideCol));
    mvhline(y0, x0, ' ', x1);
    mvhline(y0 + y1, x0, ' ', x1);

    for (int i = y0 + 1; i < y0 + y1; ++i) {
        attron(COLOR_PAIR(outsideCol));
        mvhline(i, x0, ' ', 2);
        mvhline(i, x0 + x1 - 2, ' ', 2);
        attron(COLOR_PAIR(insideCol));
        mvhline(i, x0 + 2, ' ', x1 - 4);
    }
}
inline void drawBox(short outsideCol, short insideCol, Point p0, Point p1) {
    drawBox(outsideCol, insideCol, p0.x, p0.y, p1.x, p1.y);
}
inline void drawLine(int y, int x, int len) {
    mvhline(y,x,' ', len);
}
inline void drawBlock(int y, int x) {
    mvhline(y,x,' ', 2);
}
} // namespace prophanity
} // namespace azbyn

#undef __WINDOWS__
