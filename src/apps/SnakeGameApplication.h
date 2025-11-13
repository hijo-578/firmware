#ifndef SNAKE_GAME_APPLICATION_H
#define SNAKE_GAME_APPLICATION_H

#include "Application.h"
#include "graphics/bitmaps.h"

class SnakeGameApplication : public Application {
private:
    bool running;
    bool gameOver;
    bool gameStarted;
    
    // Позиция змейки (массив сегментов)
    static const int MAX_SNAKE_LENGTH = 50;
    struct Point {
        int x;
        int y;
        Point() : x(0), y(0) {}
        Point(int x, int y) : x(x), y(y) {}
    };
    Point snake[MAX_SNAKE_LENGTH];
    int snakeLength;
    
    // Направление движения
    enum Direction {
        DIR_UP,
        DIR_DOWN,
        DIR_LEFT,
        DIR_RIGHT
    };
    Direction currentDirection;
    Direction nextDirection;
    
    // Позиция еды
    Point food;
    
    // Счет
    int score;
    
    // Скорость игры (в миллисекундах)
    int gameSpeed;
    unsigned long lastMoveTime;
    
public:
    SnakeGameApplication();
    void initialize() override;
    void update() override;
    void render(Display* display) override;
    void cleanup() override;
    
    // Обработчики навигации
    void onUpButton() override;
    void onDownButton() override;
    void onLeftButton() override;
    void onRightButton() override;
    void onOkButton() override;
    void onBackButton() override;
    void onFunctionButton() override;
    void onBackLongPress() override;
    
    // Методы метаданных
    const char* getName() override;
    bool isRunning() override;
    
    // Игровые методы
    void resetGame();
    void moveSnake();
    void generateFood();
    bool checkCollision();
    bool checkFoodCollision();
    void growSnake();
    void startGame();
};

#endif // SNAKE_GAME_APPLICATION_H