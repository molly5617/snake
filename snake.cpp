#include <ncurses.h>    // 引入 ncurses 庫，用於控制終端輸出
#include <chrono>       // 引入 chrono 库，用於時間管理
#include <queue>        // 引入 queue 库，用於使用雙端佇列
#include <utility>      // 引入 utility 库，用於使用 pair 類型
#include <random>       // 引入 random 库，用於生成隨機數
using namespace std;    // 使用標準命名空間
using namespace chrono; // 使用 chrono 命名空間

class SnakeGame // 定義 SnakeGame 類別，包含所有遊戲邏輯
{
private:
    static const int WIDTH = 40;  // 設定遊戲畫面的寬度為 40
    static const int HEIGHT = 30; // 設定遊戲畫面的高度為 30
    deque<pair<int, int>> snake;  // 使用 deque 存儲蛇的各個身體段，保存每一段的座標

    pair<int, int> food; // 存儲食物的位置
    int dx, dy;          // 蛇頭移動的方向 (dx: 水平方向，dy: 垂直方向)
    int score;           // 玩家得分
    bool gameOver;       // 遊戲是否結束的標誌
    mt19937 rng;         // 隨機數生成器，用於生成食物的隨機位置
    int gameSpeed;       // 遊戲的速度

    void initGame() // 初始化遊戲設置
    {
        initscr();             // 初始化 ncurses 模式，開始使用 ncurses 控制終端顯示
        noecho();              // 禁止自動顯示輸入的字符
        curs_set(0);           // 隱藏游標
        keypad(stdscr, TRUE);  // 啟用鍵盤擴展功能（例如：方向鍵、功能鍵等）
        nodelay(stdscr, TRUE); // 設置非阻塞模式，`getch()` 不會阻塞等待鍵盤輸入

        if (has_colors()) // 如果終端支持顏色
        {
            start_color();                           // 開始顏色模式
            init_pair(1, COLOR_GREEN, COLOR_BLACK);  // 定義顏色對 1：綠色字，黑色背景
            init_pair(2, COLOR_RED, COLOR_BLACK);    // 定義顏色對 2：紅色字，黑色背景
            init_pair(3, COLOR_CYAN, COLOR_BLACK);   // 定義顏色對 3：青色字，黑色背景
            init_pair(4, COLOR_YELLOW, COLOR_BLACK); // 定義顏色對 4：黃色字，黑色背景
        }
        snake.clear();                            // 清空蛇的佇列
        snake.push_back({WIDTH / 2, HEIGHT / 2}); // 添加蛇的初始位置
        snake.push_back({WIDTH / 2 - 1, HEIGHT / 2});
        snake.push_back({WIDTH / 2 - 2, HEIGHT / 2});

        dx = 1;           // 設置蛇初始的水平方向為向右
        dy = 0;           // 設置蛇初始的垂直方向為不動
        score = 0;        // 初始得分為 0
        gameOver = false; // 遊戲開始時遊戲未結束
        gameSpeed = 100;  // 初始遊戲速度為 100

        rng.seed(steady_clock::now().time_since_epoch().count()); // 用當前時間作為隨機數種子
        generateFood();                                           // 生成食物
    }
    void generateFood() // 生成食物的方法
    {
        do
        {
            food.first = rng() % WIDTH;   // 隨機生成食物的 x 坐標
            food.second = rng() % HEIGHT; // 隨機生成食物的 y 坐標
        } while (isSnake(food.first, food.second)); // 確保食物不在蛇身上
    }
    bool isSnake(int x, int y) // 檢查某個位置是否是蛇的身體部分
    {
        for (const auto &segment : snake) // 遍歷蛇的每一段
        {
            if (segment.first == x && segment.second == y) // 如果該段座標與給定座標一致
            {
                return true; // 返回 true，表示該位置是蛇的一部分
            }
        }
        return false; // 如果未找到該位置在蛇身上，返回 false
    }
    void processInput() // 處理玩家的輸入
    {
        int ch = getch(); // 使用 `getch()` 來獲取鍵盤輸入
        switch (ch)
        {
        case KEY_UP: // 如果按下上箭頭
        case 'w':
        case 'W':        // 或者 'w'/'W'
            if (dy != 1) // 如果現在的垂直方向不是向下
            {
                dx = 0;  // 改為水平方向不動
                dy = -1; // 垂直方向改為向上
            }
            break;
        case KEY_DOWN: // 如果按下下箭頭
        case 's':
        case 'S':         // 或者 's'/'S'
            if (dy != -1) // 如果現在的垂直方向不是向上
            {
                dx = 0; // 水平方向不動
                dy = 1; // 垂直方向改為向下
            }
            break;
        case KEY_LEFT: // 如果按下左箭頭
        case 'a':
        case 'A':        // 或者 'a'/'A'
            if (dx != 1) // 如果現在的水平方向不是向右
            {
                dx = -1; // 改為水平方向向左
                dy = 0;  // 垂直方向不動
            }
            break;
        case KEY_RIGHT: // 如果按下右箭頭
        case 'd':
        case 'D':         // 或者 'd'/'D'
            if (dx != -1) // 如果現在的水平方向不是向左
            {
                dx = 1; // 改為水平方向向右
                dy = 0; // 垂直方向不動
            }
            break;
        case 'q':            // 如果按下 'q'
        case 'Q':            // 或者 'Q'
            gameOver = true; // 設置遊戲結束標誌
            break;
        }
    }
    void update() // 更新遊戲狀態
    {
        if (gameOver) // 如果遊戲結束，則直接返回
        {
            return;
        }
        int newX = snake.front().first + dx;                                                // 根據當前蛇頭方向計算新蛇頭的 x 坐標
        int newY = snake.front().second + dy;                                               // 根據當前蛇頭方向計算新蛇頭的 y 坐標
        if (newX < 0 || newX >= WIDTH || newY < 0 || newY >= HEIGHT || isSnake(newX, newY)) // 如果蛇頭撞牆或者撞到自己
        {
            gameOver = true; // 設置遊戲結束
            return;
        }
        snake.push_front({newX, newY});                // 將新的蛇頭位置加到蛇身前面
        if (newX == food.first && newY == food.second) // 如果蛇吃到食物
        {
            score++;            // 增加分數
            generateFood();     // 重新生成食物
            if (gameSpeed > 50) // 遊戲速度隨著分數增加而加快
            {
                gameSpeed = 100 - score * 2;
            }
        }
        else
        {
            snake.pop_back(); // 如果沒有吃到食物，移除蛇尾
        }
    }
    void render() // 渲染遊戲畫面
    {
        clear();               // 清除屏幕上的舊內容
        attron(COLOR_PAIR(3)); // 啟用顏色對 3（青色）
        for (int i = 0; i < WIDTH + 2; i++)
        {
            mvaddch(0, i, '='); // 畫上邊界
            mvaddch(HEIGHT + 1, i, '=');
        }
        for (int i = 0; i < HEIGHT + 2; i++)
        {
            mvaddch(i, 0, '|');         // 畫左邊界
            mvaddch(i, WIDTH + 1, '|'); // 畫右邊界
        }
        attroff(COLOR_PAIR(3));                   // 禁用顏色對 3
        attron(COLOR_PAIR(1));                    // 啟用顏色對 1（綠色）
        for (size_t i = 0; i < snake.size(); i++) // 渲染蛇的每一段
        {
            mvaddch(snake[i].second + 1, snake[i].first + 1, 'o'); // 蛇的身體部分
        }
        attroff(COLOR_PAIR(1)); // 禁用顏色對 1
        attron(COLOR_PAIR(4));
        mvaddch(snake[0].second + 1, snake[0].first + 1, 'o');
        attroff(COLOR_PAIR(4));
        attron(COLOR_PAIR(2) | A_BOLD);                // 啟用顏色對 4（黃色）並加粗字體
        mvaddch(food.second + 1, food.first + 1, '*'); // 渲染食物
        attroff(COLOR_PAIR(2) | A_BOLD);               // 禁用顏色對 4 和加粗字體

        mvprintw(HEIGHT + 2, 0, "Score: %d Speed: %d", score, 100 - gameSpeed); // 顯示分數和遊戲速度
        mvprintw(HEIGHT + 3, 0, "WASD or arrow keys to move, Q to quit");       // 顯示操作提示

        if (gameOver) // 如果遊戲結束
        {
            attron(A_BOLD);                                                       // 啟用加粗字體
            mvprintw(HEIGHT / 2, WIDTH / 2 - 5, "game over!");                    // 顯示結束信息
            mvprintw(HEIGHT / 2 + 1, WIDTH / 2 - 8, "score: %d", score);          // 顯示最終得分
            mvprintw(HEIGHT / 2 + 2, WIDTH / 2 - 10, "Press any key to exit..."); // 顯示退出提示
            attroff(A_BOLD);                                                      // 禁用加粗字體
        }
        refresh(); // 刷新螢幕顯示
    }

public:
    void run() // 啟動遊戲
    {
        initGame();       // 初始化遊戲
        while (!gameOver) // 當遊戲未結束時循環
        {
            processInput();   // 處理用戶輸入
            update();         // 更新遊戲狀態
            render();         // 渲染遊戲畫面
            napms(gameSpeed); // 暫停，根據遊戲速度控制遊戲速度
        }
        nodelay(stdscr, FALSE); // 設置為阻塞模式，等待用戶輸入
        getch();                // 等待按鍵退出
        endwin();               // 結束 ncurses 模式
    }
};

int main() // 主函數
{
    SnakeGame game; // 創建一個 SnakeGame 物件
    game.run();     // 運行遊戲
    return 0;       // 返回 0，表示正常結束
}