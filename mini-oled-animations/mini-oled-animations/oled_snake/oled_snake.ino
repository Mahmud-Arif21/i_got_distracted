#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define I2C_ADDR 0x3C
#define SCR_W 128
#define SCR_H 64
#define OLED_RST -1

Adafruit_SH1106G display(SCR_W, SCR_H, &Wire, OLED_RST);

const int GRID_SZ = 4;
const int GRID_W = SCR_W / GRID_SZ;
const int GRID_H = SCR_H / GRID_SZ;
const int MAX_SNAKE = 100;

struct Point { int x, y; };

Point snake[MAX_SNAKE];
int snakeLen = 3;
Point food;
Point dir = {1, 0};
int score = 0;
int gameSpd = 200;
bool gameOver = false;
int foodFlash = 0;
int gameOverTmr = 0;
unsigned long lastMove = 0;
unsigned long lastFlash = 0;

void setup() {
  display.begin(I2C_ADDR, true);
  delay(250);
  showTitle();
  initGame();
  randomSeed(analogRead(A0));
}

void loop() {
  unsigned long now = millis();
  if (now - lastFlash >= 100) {
    foodFlash = (foodFlash + 1) % 4;
    lastFlash = now;
  }
  if (gameOver) {
    handleGameOver();
    return;
  }
  if (now - lastMove >= gameSpd) {
    updateAI();
    moveSnake();
    checkCollisions();
    lastMove = now;
  }
  drawGame();
}

void showTitle() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(15, 15);
  display.println("AUTO");
  display.setCursor(10, 35);
  display.println("SNAKE");
  display.display();
  delay(2000);
}

void initGame() {
  snakeLen = 3;
  snake[0] = {GRID_W / 2, GRID_H / 2};
  snake[1] = {GRID_W / 2 - 1, GRID_H / 2};
  snake[2] = {GRID_W / 2 - 2, GRID_H / 2};
  dir = {1, 0};
  score = 0;
  gameSpd = 200;
  gameOver = false;
  gameOverTmr = 0;
  spawnFood();
}

void spawnFood() {
  bool valid = false;
  int attempts = 0;
  while (!valid && attempts < 50) {
    food.x = random(1, GRID_W - 1);
    food.y = random(1, GRID_H - 1);
    valid = true;
    for (int i = 0; i < snakeLen; i++) {
      if (snake[i].x == food.x && snake[i].y == food.y) {
        valid = false;
        break;
      }
    }
    attempts++;
  }
}

void updateAI() {
  Point head = snake[0];
  Point next = {head.x + dir.x, head.y + dir.y};
  bool safe = isPosSafe(next);
  if (!safe || random(0, 10) < 3) {
    Point bestDir = findBestDir();
    if (bestDir.x != 0 || bestDir.y != 0) dir = bestDir;
  }
}

Point findBestDir() {
  Point head = snake[0];
  Point dirs[] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};
  Point bestDir = {0, 0};
  float bestScore = -1000;
  for (int i = 0; i < 4; i++) {
    Point newDir = dirs[i];
    if (newDir.x == -dir.x && newDir.y == -dir.y) continue;
    Point newHead = {head.x + newDir.x, head.y + newDir.y};
    if (isPosSafe(newHead)) {
      float score = calcMoveScore(newHead, newDir);
      if (score > bestScore) {
        bestScore = score;
        bestDir = newDir;
      }
    }
  }
  return bestDir;
}

float calcMoveScore(Point pos, Point d) {
  float score = 0;
  float foodDist = abs(pos.x - food.x) + abs(pos.y - food.y);
  score -= foodDist * 10;
  int wallDist = min(min(pos.x, GRID_W - 1 - pos.x), min(pos.y, GRID_H - 1 - pos.y));
  score += wallDist * 2;
  for (int i = 1; i < snakeLen; i++) {
    int bodyDist = abs(pos.x - snake[i].x) + abs(pos.y - snake[i].y);
    if (bodyDist < 3) score -= (3 - bodyDist) * 20;
  }
  if (d.x == dir.x && d.y == dir.y) score += 1;
  return score;
}

bool isPosSafe(Point pos) {
  if (pos.x < 0 || pos.x >= GRID_W || pos.y < 0 || pos.y >= GRID_H) return false;
  for (int i = 0; i < snakeLen; i++) {
    if (snake[i].x == pos.x && snake[i].y == pos.y) return false;
  }
  return true;
}

void moveSnake() {
  for (int i = snakeLen - 1; i > 0; i--) snake[i] = snake[i - 1];
  snake[0].x += dir.x;
  snake[0].y += dir.y;
}

void checkCollisions() {
  Point head = snake[0];
  if (head.x < 0 || head.x >= GRID_W || head.y < 0 || head.y >= GRID_H) {
    gameOver = true;
    return;
  }
  for (int i = 1; i < snakeLen; i++) {
    if (snake[i].x == head.x && snake[i].y == head.y) {
      gameOver = true;
      return;
    }
  }
  if (head.x == food.x && head.y == food.y) {
    score++;
    snakeLen++;
    if (gameSpd > 80) gameSpd -= 5;
    spawnFood();
    foodFlash = 0;
  }
}

void handleGameOver() {
  gameOverTmr++;
  if (gameOverTmr < 30) {
    if (gameOverTmr % 4 < 2) drawGame();
    else {
      display.clearDisplay();
      display.display();
    }
  } else if (gameOverTmr < 80) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(35, 20);
    display.println("GAME OVER");
    display.setCursor(30, 35);
    display.print("Score: ");
    display.println(score);
    display.display();
  } else {
    initGame();
  }
}

void drawGame() {
  display.clearDisplay();
  display.drawRect(0, 0, SCR_W, SCR_H, SH110X_WHITE);
  for (int i = 0; i < snakeLen; i++) {
    int x = snake[i].x * GRID_SZ + 1;
    int y = snake[i].y * GRID_SZ + 1;
    if (i == 0) {
      display.fillRect(x, y, GRID_SZ - 1, GRID_SZ - 1, SH110X_WHITE);
      display.drawPixel(x + 1, y + 1, SH110X_BLACK);
      display.drawPixel(x + GRID_SZ - 3, y + 1, SH110X_BLACK);
    } else {
      display.fillRect(x + 1, y + 1, GRID_SZ - 3, GRID_SZ - 3, SH110X_WHITE);
    }
  }
  int foodX = food.x * GRID_SZ + 1;
  int foodY = food.y * GRID_SZ + 1;
  if (foodFlash < 2) {
    display.fillRect(foodX, foodY, GRID_SZ - 1, GRID_SZ - 1, SH110X_WHITE);
    display.drawPixel(foodX + GRID_SZ / 2, foodY + GRID_SZ / 2, SH110X_BLACK);
  } else {
    display.drawRect(foodX, foodY, GRID_SZ - 1, GRID_SZ - 1, SH110X_WHITE);
  }
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(2, 2);
  display.print(score);
  display.setCursor(100, 2);
  display.print("L:");
  display.print(snakeLen);
  display.display();
}