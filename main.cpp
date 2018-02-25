#include "point.h"
using azbyn::Point;
#include "misc.h"
using azbyn::string_format;

#include <curses.h>
#include <stdint.h>

#include <chrono>
#include <list>
#include <thread>
#include <string>

#define COLOR_ORANGE 16
#define COLOR_BASE00 0
#define COLOR_BASE01 18
#define COLOR_BASE02 19
#define COLOR_BASE03 8
#define COLOR_BASE04 20
#define COLOR_BASE05 7
#define COLOR_BASE06 21
#define COLOR_BASE07 15

#define LEN(x) (sizeof(x) / sizeof(*x))

void waitAFrame() {
    std::this_thread::sleep_for(std::chrono::milliseconds(16));
}
constexpr Point BoardSize = {30, 18};

struct SnakePart : public Point {
    SnakePart* next = nullptr;
    SnakePart* prev = nullptr;
};

class Game {
    int score = 0;
    Point food = {4, 4};
    int level = 1; // between 1 and 10
public:
    Game() {
        
    }
    constexpr int Score() const { return score; }
    constexpr Point Food() const { return food; }
    constexpr float Speed() const { return (11 - level) * 0.1; }
    void IncreaseScore() { score += level; }
} game;

class Player {
    std::array<SnakePart, BoardSize.x * BoardSize.y> snakeArr = {};
    int snakeLen = 1;
    SnakePart* head;
    SnakePart* tailTip;
    std::chrono::time_point<std::chrono::system_clock> lastMove;
    enum Rotation {
        ROT_N,
        ROT_E,
        ROT_S,
        ROT_W,
    } rotation = ROT_W;
public:
    Player() {
        head = &snakeArr[0];
        head->x = BoardSize.x / 2;
        head->y = BoardSize.y / 2;
        tailTip = head;
        for (int i = 1; i < 5; ++i) {
            AddPiece(head->x + i, head->y);
        }
        lastMove = std::chrono::system_clock::now();
    }
    const SnakePart& Head() const { return *head; }
    const SnakePart* TailBegin() const { return head->next; }
    void Input() {
        switch (tolower(getch())) {
        case 'w':
        case KEY_UP:
            rotation = ROT_N;
        case KEY_DOWN:
        case 's':
            rotation = ROT_S;
            break;
        case KEY_LEFT:
        case 'a':
            rotation = ROT_W;
            break;
        case KEY_RIGHT:
        case 'd':
            rotation = ROT_E;
            break;
        case KEY_F(1):
        case 'q':
        case 27:
        case 'p':
            exit(0);
            break;
        }
    }
    void CheckMove() {
        auto now = std::chrono::system_clock::now();
        std::chrono::duration<float> d = now - lastMove;
        if (d.count() >= game.Speed()) {
            lastMove = now;
            Move();
        }
    }

private:
    void Move() {
        //to be implemented
    }
    void AddPiece(int x, int y) {
        tailTip->next = &snakeArr[snakeLen];
        tailTip = tailTip->next;
        tailTip->x = x;
        tailTip->y = y;
        ++snakeLen;
    }

} player;
class Graphics {
    enum Pairs {
        PAIR_TAIL = 1,
        PAIR_HEAD,
        PAIR_FOOD,
        PAIR_BG,
        PAIR_BORDER,
        PAIR_TEXT,
    };
    static void coladdstr(short col, const char* str) {
        attron(COLOR_PAIR(col));
        addstr(str);
    }
    static void mvcoladdstr(int y, int x, short col, const char* str) {
        attron(COLOR_PAIR(col));
        mvaddstr(y, x, str);
    }

    void InitColors() {
        start_color();
        constexpr short bgColor = COLOR_BLACK;
        auto addColor = [](int i, short col) { init_pair(i, col, col); };
        init_pair(PAIR_TAIL, COLOR_YELLOW, COLOR_GREEN);
        init_pair(PAIR_HEAD, COLOR_YELLOW, COLOR_YELLOW);
        addColor(PAIR_FOOD, COLOR_RED);
        addColor(PAIR_BG, bgColor);
        addColor(PAIR_BORDER, COLOR_WHITE);
        init_pair(PAIR_TEXT, COLOR_WHITE, bgColor);
    }

public:
    Graphics() {
        initscr(); /* initialize the curses library */
        keypad(stdscr, true); /* enable keyboard mapping */
        nonl(); /* tell curses not to do NL->CR/NL on output */
        cbreak(); /* take input chars one at a time, no wait for \n */
        noecho();
        nodelay(stdscr, true);
        meta(stdscr, true);
        curs_set(0);
        //putenv("ESCDELAY=25");
        if (COLS < 2 * BoardSize.x + 2 || LINES < BoardSize.y + 3) {
            throw std::runtime_error(
                string_format("terminal too small %dx%d", COLS, LINES));
        }
        if (has_colors())
            InitColors();
        DrawBegin();
    }
    void Restart() {
        clear();
        DrawBegin();
    }

    ~Graphics() {
        endwin();
    }
    void DrawBegin() {
        attron(COLOR_PAIR(PAIR_BORDER));
        const std::string verticalBar(BoardSize.x * 2 + 4, ' ');
        mvaddstr(1, 0, verticalBar.c_str());
        for (int y = 0; y < BoardSize.y; ++y) {
            mvaddstr(y + 2, 0, "  ");
            mvaddstr(y + 2, BoardSize.x * 2 + 2, "  ");
        }
        mvaddstr(2 + BoardSize.y, 0, verticalBar.c_str());
    }
    void DrawVal(int y, int x, const char* str, int num) {
        mvprintw(y, x, str);
        mvprintw(y + 1, x, "  %d", num);
    }

    void DrawInfo() {
        attron(COLOR_PAIR(PAIR_TEXT));
        mvprintw(0, 4, "Score: %d", game.Score());
    }
    void DrawBlock(Point p, const char* str) {
        mvprintw(p.y + 1, p.x * 2 + 2, str);
    }

    void DrawPlayer() {
        attron(COLOR_PAIR(PAIR_TAIL));
        for (auto it = player.TailBegin(); it; it = it->next) {
            DrawBlock(*it, "{}");
        }
        attron(COLOR_PAIR(PAIR_HEAD));
        DrawBlock(player.Head(), "()");
    }
    void DrawFood() {
        attron(COLOR_PAIR(PAIR_FOOD));
        DrawBlock(game.Food(), "[]");
    }

public:
    void Draw() {
        DrawInfo();
        DrawPlayer();
        DrawFood();
    }
    /*
    void DrawPause() {
        DrawScreenBase("Paused", true);
    }*/
    /*
    void DrawEndScreen() {
        DrawScreenBase(game.HasHighscore() ? "HIGH SCORE" : "GAME OVER", false);
    }*/
private:
    /*
    void DrawScreenBase(std::string title, bool isPause) {
        const std::string verticalBar = std::string(Width * 2, ' ');
        DrawAtMiddle(PAIR_BORDER, 3, verticalBar);
        for (int i = 4; i < 10; ++i)
            DrawAtMiddle(PAIR_TEXT, i, verticalBar);

        DrawAtMiddle(PAIR_BORDER, 10, verticalBar);
        DrawAtMiddle(PAIR_TEXT, 5, title);
        DrawAtMiddle(PAIR_TEXT, 7, isPause ?
                     "Quit      Resume" :
                     "Quit      Replay");
        DrawAtMiddle(PAIR_TEXT, 8, "  Q          R  ");
    }

    void DrawAtMiddle(short color, int y, std::string s) {
        mvcoladdstr(y, MatrixStartX + Width - (s.size() / 2), color, s.c_str());
    }*/
} graphics;

int main() {
    for (;;) {
        player.Input();
        player.CheckMove();
        graphics.Draw();
        waitAFrame();
    }
    return 0;
}
