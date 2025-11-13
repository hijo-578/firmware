#include "SnakeGameApplication.h"
#include "core/Display.h"
#include "config.h"
#include <Arduino.h>

SnakeGameApplication::SnakeGameApplication() {
    running = false;
    gameOver = false;
    gameStarted = false;
    snakeLength = 3;
    currentDirection = DIR_RIGHT;
    nextDirection = DIR_RIGHT;
    score = 0;
    gameSpeed = 300; // milliseconds
    lastMoveTime = 0;
}

void SnakeGameApplication::initialize() {
    running = true;
    resetGame();
}

void SnakeGameApplication::update() {
    if (!running) return;
    
    if (gameStarted && !gameOver) {
        // Проверяем, пора ли двигать змейку
        if (millis() - lastMoveTime > gameSpeed) {
            // Обновляем направление
            currentDirection = nextDirection;
            
            // Двигаем змейку
            moveSnake();
            
            // Проверяем столкновения
            if (checkCollision()) {
                gameOver = true;
            }
            
            // Проверяем, съела ли змейка еду
            if (checkFoodCollision()) {
                growSnake();
                score += 10;
                generateFood();
                
                // Увеличиваем скорость каждые 50 очков
                if (score % 50 == 0 && gameSpeed > 100) {
                    gameSpeed -= 20;
                }
            }
            
            lastMoveTime = millis();
        }
    }
}

void SnakeGameApplication::render(Display* display) {
    if (!running) return;
    
    display->clear();
    
    if (!gameStarted) {
        // Отображаем экран приветствия
        display->drawString(20, 10, "SNAKE GAME");
        display->drawString(10, 30, "Press OK to start");
        display->drawString(10, 40, "Use arrows to play");
        display->drawString(10, 50, "BACK to exit");
    } else if (gameOver) {
        // Отображаем экран окончания игры
        display->drawString(20, 10, "GAME OVER");
        display->drawString(10, 30, "Score: " + String(score));
        display->drawString(10, 40, "Press OK to restart");
        display->drawString(10, 50, "BACK to exit");
    } else {
        // Рисуем игровое поле
        // Рисуем змейку
        for (int i = 0; i < snakeLength; i++) {
            display->fillRect(snake[i].x * 4, snake[i].y * 4, 4, 4, 1);
        }
        
        // Рисуем еду
        display->fillRect(food.x * 4, food.y * 4, 4, 4, 1);
        
        // Отображаем счет
        display->drawString(0, 0, "Score: " + String(score));
    }
    
    display->update();
}

void SnakeGameApplication::cleanup() {
    running = false;
    gameOver = false;
    gameStarted = false;
}

// Обработчики навигации
void SnakeGameApplication::onUpButton() {
    if (gameStarted && !gameOver) {
        if (currentDirection != DIR_DOWN) {
            nextDirection = DIR_UP;
        }
    }
}

void SnakeGameApplication::onDownButton() {
    if (gameStarted && !gameOver) {
        if (currentDirection != DIR_UP) {
            nextDirection = DIR_DOWN;
        }
    }
}

void SnakeGameApplication::onLeftButton() {
    if (gameStarted && !gameOver) {
        if (currentDirection != DIR_RIGHT) {
            nextDirection = DIR_LEFT;
        }
    }
}

void SnakeGameApplication::onRightButton() {
    if (gameStarted && !gameOver) {
        if (currentDirection != DIR_LEFT) {
            nextDirection = DIR_RIGHT;
        }
    }
}

void SnakeGameApplication::onOkButton() {
    if (!gameStarted) {
        startGame();
    } else if (gameOver) {
        resetGame();
    }
}

void SnakeGameApplication::onBackButton() {
    // Возвращаемся в меню
    running = false;
}

void SnakeGameApplication::onFunctionButton() {
    // Не используется в этой игре
}

void SnakeGameApplication::onBackLongPress() {
    // Возвращаемся в меню
    running = false;
}

// Методы метаданных
const char* SnakeGameApplication::getName() {
    return "Snake Game";
}

bool SnakeGameApplication::isRunning() {
    return running;
}

// Игровые методы
void SnakeGameApplication::resetGame() {
    gameOver = false;
    gameStarted = false;
    snakeLength = 3;
    currentDirection = DIR_RIGHT;
    nextDirection = DIR_RIGHT;
    score = 0;
    gameSpeed = 300;
    
    // Инициализируем змейку по центру экрана
    int startX = OLED_WIDTH / 8; // 128 / 8 = 16
    int startY = OLED_HEIGHT / 8; // 64 / 8 = 8
    
    for (int i = 0; i < snakeLength; i++) {
        snake[i].x = startX - i;
        snake[i].y = startY;
    }
    
    // Генерируем первую еду
    generateFood();
}

void SnakeGameApplication::startGame() {
    gameStarted = true;
    gameOver = false;
}

void SnakeGameApplication::moveSnake() {
    // Двигаем тело змейки
    for (int i = snakeLength - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }
    
    // Двигаем голову в зависимости от направления
    switch (currentDirection) {
        case DIR_UP:
            snake[0].y--;
            break;
        case DIR_DOWN:
            snake[0].y++;
            break;
        case DIR_LEFT:
            snake[0].x--;
            break;
        case DIR_RIGHT:
            snake[0].x++;
            break;
    }
}

void SnakeGameApplication::generateFood() {
    bool validPosition = false;
    
    while (!validPosition) {
        // Генерируем случайную позицию для еды
        food.x = random(0, OLED_WIDTH / 4); // 0-31
        food.y = random(0, OLED_HEIGHT / 4); // 0-15
        
        // Проверяем, что еда не появилась на змейке
        validPosition = true;
        for (int i = 0; i < snakeLength; i++) {
            if (snake[i].x == food.x && snake[i].y == food.y) {
                validPosition = false;
                break;
            }
        }
    }
}

bool SnakeGameApplication::checkCollision() {
    // Проверяем столкновение с границами экрана
    if (snake[0].x < 0 || snake[0].x >= OLED_WIDTH / 4 || 
        snake[0].y < 0 || snake[0].y >= OLED_HEIGHT / 4) {
        return true;
    }
    
    // Проверяем столкновение с самим собой
    for (int i = 1; i < snakeLength; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            return true;
        }
    }
    
    return false;
}

bool SnakeGameApplication::checkFoodCollision() {
    return (snake[0].x == food.x && snake[0].y == food.y);
}

void SnakeGameApplication::growSnake() {
    if (snakeLength < MAX_SNAKE_LENGTH) {
        snake[snakeLength] = snake[snakeLength - 1];
        snakeLength++;
    }
}