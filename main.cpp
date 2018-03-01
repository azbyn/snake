#include "point.h"
using azbyn::Point;
#include "misc.h"
using azbyn::Callback;
using azbyn::string_format;
#include "prophanity.h"
using namespace azbyn::prophanity;

#include <curses.h>
#include <signal.h>
#include <stdint.h>

#include <fstream>
#include <chrono>
#include <queue>
#include <random>
#include <string>
#include <thread>

void waitAFrame() { //120 fps
    std::this_thread::sleep_for(std::chrono::milliseconds(8));
}
void keyChoice(int a, Callback cbA, int b, Callback cbB) {
    for (;;) {
        auto c = tolower(getch());
        if (c == a) {
            cbA();
            return;
        }
        if (c == b) {
            cbB();
            return;
        }
        waitAFrame();
    }
}
constexpr Point BoardSize = {30, 18};

struct SnakePart : public Point {
    SnakePart* next = nullptr;
    SnakePart* prev = nullptr;
    void SetPos(Point p) {
        x = p.x;
        y = p.y;
    }
    void SetPos(int x, int y) {
        this->x = x;
        this->y = y;
    }
    Point& Pos() { return *this; }
};
const std::string HighscoresPath = "highscores";
const std::string ConfigPath = "config";
class Game {
    int score = 0;
    bool wrap = true;
    int highscore = 0;
    Point food = {4, 4};
    int difficulty = 6;
    std::random_device rd;
    std::mt19937 gen;
    bool running = true;
    bool won = false;
    Callback pause;

    void ReadConfig() {
        std::ifstream f(ConfigPath);
        std::string s;
        f >> difficulty >> s;
        if (difficulty < 1 && difficulty > 20)
            difficulty = 10;

        wrap = s == "wrap";
    }
    void ReadHighscore() {
        std::ifstream f(HighscoresPath);
        f >> highscore;
    }

public:
    Game() : rd(), gen(rd()) {
        Restart();
        ReadHighscore();
        ReadConfig();
    }
    ~Game() {
        std::ofstream f(HighscoresPath);
        f << highscore << "\n";
    }
    void Restart() {
        score = 0;
        running = true;
        won = false;
    }
    inline void Init(Callback pause) { this->pause = pause; }
    constexpr bool Wrap() const { return wrap; }
    constexpr int Score() const { return score; }
    constexpr Point Food() const { return food; }
    constexpr float Speed() const { return (21 - difficulty) * 0.02; }
    constexpr bool Running() const { return running; }
    constexpr bool HasHighscore() const { return highscore == score; }
    constexpr int Highscore() const { return highscore; }
    constexpr bool Won() const { return won; }
    void IncreaseScore() {
        score += difficulty;
        if (score > highscore) {
            highscore = score;
        }
    }

    template <class Predicate>
    void PlaceFood(const SnakePart* sp, Predicate isOccupied) {
        for (int i = 0; i < 200; ++i) {
            food = {(int)(gen() % BoardSize.x), (int)(gen() % BoardSize.y)};
            if (!isOccupied(sp, food))
                return;
        }
        for (food.x = 0; food.x < BoardSize.x; ++food.x) {
            for (food.y = 0; food.y < BoardSize.y; ++food.y) {
                if (!isOccupied(sp, food))
                    return;
            }
        }
    }
    void Pause() const {
        pause();
    }
    void End() {
        running = false;
    }
    void Win() {
        running = false;
        won = true;
    }
} game;

class Player {
    std::array<SnakePart, BoardSize.x* BoardSize.y> snakeArr = {};
    int snakeLen;
    SnakePart* head;
    SnakePart* tailTip;
    std::chrono::time_point<std::chrono::system_clock> lastMove;
    std::queue<Point> inputQueue;
    void InputQueuePush(int x, int y) {
        if (inputQueue.size() > 2) return; // prevent the snake lagging behind too much
        Point b = inputQueue.back();
        if ((b.x && x) || (b.y && y)) return; //prevent going backwards and forwards
        inputQueue.emplace(x, y);
    }
    static bool isOccupied(const SnakePart* sp, Point pt) {
        for (auto* it = sp; it; it = it->next) {
            if (it->x == pt.x && it->y == pt.y)
                return true;
        }
        return false;
    }
    bool IsTail(Point pt) { return isOccupied(head->next, pt); }
    void PlaceFood() { game.PlaceFood(head, &Player::isOccupied); }

public:
    Player() {
        Restart();
    }
    void Restart() {
        snakeArr.fill({});
        std::queue<Point> empty;
        std::swap(inputQueue, empty);
        snakeLen = 1;

        head = &snakeArr[0];
        head->x = BoardSize.x / 2;
        head->y = BoardSize.y / 2;
        tailTip = head;
        for (int i = 1; i < 5 + 5; ++i) {
            AddPiece(head->x + i, head->y);
        }
        inputQueue.emplace(-1, 0);
        lastMove = std::chrono::system_clock::now();
        PlaceFood();
    }
    const SnakePart& Head() const { return *head; }
    const SnakePart* TailBegin() const { return head->next; }
    void Input() {
        switch (tolower(getch())) {
        case 'w':
        case KEY_UP:
            InputQueuePush(0, -1);
            break;
        case KEY_DOWN:
        case 's':
            InputQueuePush(0, 1);
            break;
        case KEY_LEFT:
        case 'a':
            InputQueuePush(-1, 0);
            break;
        case KEY_RIGHT:
        case 'd':
            InputQueuePush(1, 0);
            break;
        case KEY_F(1):
        case 'q':
        case 27:
        case 'p':
            game.Pause();
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
        auto front = inputQueue.front();
        auto* newHead = tailTip;
        tailTip = tailTip->prev;
        tailTip->next = nullptr;

        head->prev = newHead;
        newHead->SetPos(*head + front);
        if (inputQueue.size() > 1)
            inputQueue.pop();
        newHead->prev = nullptr;
        newHead->next = head;
        head = newHead;
        if (head->Pos() == game.Food()) {
            AddPiece(tailTip->Pos());
            PlaceFood();
            game.IncreaseScore();
        }
        else if (IsTail(head->Pos())) {
            game.End();
        }
        else if (!head->IsInBounds({0, 0}, BoardSize)) {
            if (!game.Wrap()) {
                game.End();
                return;
            }
            if (head->x < 0)
                head->x = BoardSize.x - 1;
            else if (head->x >= BoardSize.x)
                head->x = 0;

            if (head->y < 0)
                head->y = BoardSize.y - 1;
            else if (head->y >= BoardSize.y)
                head->y = 0;
        }
    }
    void AddPiece(int x, int y) {
        auto* newTailTip = &snakeArr[snakeLen];
        newTailTip->prev = tailTip;
        newTailTip->SetPos(x, y);
        tailTip->next = newTailTip;
        tailTip = newTailTip;
        ++snakeLen;
        if (snakeLen == BoardSize.RectArea())
            game.Win();
    }
    void AddPiece(Point p) { AddPiece(p.x, p.y); }
    void Debug() {
        for (auto it = head; it; it = it->next) {
            fprintf(stdout, "(@%ld p%ld n%ld)",
                    it - &snakeArr[0],
                    !it->prev ? -1 : it->prev - &snakeArr[0],
                    !it->next ? -1 : it->next - &snakeArr[0]);
        }
        fprintf(stdout, "<end>");
    }

} player;

class Graphics {
    enum Pair {
        PAIR_TAIL = 1,
        PAIR_HEAD,
        PAIR_FOOD,
        PAIR_BG,
        PAIR_BORDER,
        PAIR_TEXT,
    };

    void InitColors() {
        start_color();
        constexpr short bgColor = COL_BLACK;
        auto addColor = [](int i, short col) { init_pair(i, col, col); };
        init_pair(PAIR_TAIL, COL_YELLOW, COL_GREEN);
        init_pair(PAIR_HEAD, COL_YELLOW, COL_YELLOW);
        addColor(PAIR_FOOD, COL_RED);
        addColor(PAIR_BG, bgColor);
        addColor(PAIR_BORDER, game.Wrap() ? COL_DARK_GRAY : COL_LIGHT_GRAY);
        init_pair(PAIR_TEXT, COL_DARK_WHITE, bgColor);
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
        constexpr int x = BoardSize.x * 2 + 2;
        drawLine(1, 0, x + 2);
        for (int y = 0; y < BoardSize.y; ++y) {
            drawBlock(y + 2, 0);
            drawBlock(y + 2, x);
        }
        drawLine(2 + BoardSize.y, 0,  x + 2);
    }
    void DrawVal(int y, int x, const char* str, int num) {
        mvprintw(y, x, str);
        mvprintw(y + 1, x, "  %d", num);
    }

    void DrawInfo() {
        attron(COLOR_PAIR(PAIR_TEXT));
        mvprintw(0, 4, "Score: %d", game.Score());
        mvprintw(0, 30, "Highscore: %d", game.Highscore());

    }
    void DrawBlock(Point p, const char* str) {
        mvprintw(p.y + 2, p.x * 2 + 2, str);
    }
    void DrawField() {
        attron(COLOR_PAIR(PAIR_BG));
        for (int y = 0; y < BoardSize.y; ++y)
            drawLine(y + 2, 2, BoardSize.x * 2);
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
        DrawField();
        DrawInfo();
        DrawPlayer();
        DrawFood();
    }
    void DrawPause() {
        DrawScreenBase("Paused", true);
    }
    void DrawEndScreen() {
        DrawScreenBase(game.HasHighscore() ? "HIGH SCORE" :
                       (game.Won() ? "YOU WON!" : "GAME OVER"), false);
    }

private:
    void DrawScreenBase(std::string title, bool isPause) {
        constexpr int y = 2 + (BoardSize.y / 2) - 4;
        constexpr int x = BoardSize.x + 2 - 15;
        drawBox(PAIR_BORDER, PAIR_TEXT, x, y, 30, 7);

        DrawAtMiddle(PAIR_TEXT, y + 2, title);
        DrawAtMiddle(PAIR_TEXT, y + 4, isPause ?
                     "Quit      Resume" :
                     "Quit      Replay");
        DrawAtMiddle(PAIR_TEXT, y + 5, "  Q          R  ");
    }

    void DrawAtMiddle(short color, int y, std::string s) {
        mvcoladdstr(y, BoardSize.x + 2 - (s.size() / 2), color, s.c_str());
    }
} graphics;
int main() {
    signal(SIGINT, [](int) { game.End(); exit(0); });
    atexit([] { game.End(); });
    game.Init([] {
        graphics.DrawPause();
        keyChoice('q', [] { exit(0); },
                  'r', [] { /* don't do anything */ });
    });
    for (;;) {
        while (game.Running()) {
            player.Input();
            player.CheckMove();
            graphics.Draw();
            waitAFrame();
        }
        graphics.DrawEndScreen();
        keyChoice('q', [] { exit(0); },
                  'r', [] {
                      game.Restart();
                      player.Restart();
                      graphics.Restart(); });
    }

    return 0;
}
