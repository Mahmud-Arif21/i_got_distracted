#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define I2C_ADDR 0x3C
#define SCR_W 128
#define SCR_H 64
#define OLED_RST -1

Adafruit_SH1106G display(SCR_W, SCR_H, &Wire, OLED_RST);

#define padW 16
#define padH 3
#define ballSz 2
#define brkW 14
#define brkH 4
#define brkRows 4
#define brkCols 8
#define brkGap 2
#define maxBonus 2
#define maxParts 10

const float angFact = 0.15;

struct Ball { float x, y, vx, vy; bool active; };
struct Paddle { float x, y; };
struct Bonus { float x, y, vy; bool active; int type; };
struct Particle { float x, y, vx, vy; int life; };

Ball ball;
Paddle paddle;
int bricks[brkRows][brkCols];
Bonus bonuses[maxBonus];
Particle particles[maxParts];
int score = 0, lives = 3;
bool gameOver = false;
int gameOverTimer = 0;
unsigned long lastUpdate = 0;
const int frameTime = 33;
const float padSpd = 3.0;

void setup() {
  display.begin(I2C_ADDR, true);
  delay(250);
  randomSeed(analogRead(A0));
  showTitle();
  initGame();
}

void loop() {
  if (millis() - lastUpdate >= frameTime) {
    if (!gameOver) updateGame();
    else handleGameOver();
    drawGame();
    lastUpdate = millis();
  }
}

void showTitle() {
  for (int i = 0; i < 3; i++) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(30, 20);
    display.println("BREAKOUT");
    display.setCursor(20, 35);
    display.println("Autoplay AI");
    display.display();
    delay(600);
  }
}

void initGame() {
  score = 0;
  lives = 3;
  gameOver = false;
  gameOverTimer = 0;
  resetLevel();
}

void resetLevel() {
  paddle.x = (SCR_W - padW) / 2;
  paddle.y = SCR_H - padH - 2;
  ball.x = SCR_W / 2;
  ball.y = paddle.y - ballSz - 2;
  ball.vx = random(-150, 150) / 100.0;
  ball.vy = -2.0;
  ball.active = true;
  for (int r = 0; r < brkRows; r++)
    for (int c = 0; c < brkCols; c++)
      bricks[r][c] = (random(10) < 2) ? 2 : 1;
  for (auto &b : bonuses) b.active = false;
  for (auto &p : particles) p.life = 0;
}

float predictBallX() {
  float bx = ball.x, by = ball.y;
  float vx = ball.vx, vy = ball.vy;
  while (by < paddle.y) {
    bx += vx;
    by += vy;
    if (bx <= 1 || bx >= SCR_W - 1) vx = -vx;
    if (by <= 1) vy = -vy;
  }
  return bx;
}

void updateGame() {
  float sumX = 0;
  int count = 0;
  for (int r = 0; r < brkRows; r++) {
    for (int c = 0; c < brkCols; c++) {
      if (bricks[r][c] > 0) {
        float cx = c * (brkW + brkGap) + brkGap + brkW / 2.0;
        sumX += cx;
        count++;
      }
    }
  }
  float brkCtrX = (count > 0) ? sumX / count : SCR_W / 2.0;

  float targetX;
  if (ball.vy > 0) {
    float dropX = predictBallX();
    float diff = brkCtrX - dropX;
    float offset = constrain(diff * angFact, -padW / 2, padW / 2);
    targetX = dropX - padW / 2 + offset;
  } else {
    targetX = (SCR_W - padW) / 2;
  }

  if (paddle.x < targetX) paddle.x += min(padSpd, targetX - paddle.x);
  else if (paddle.x > targetX) paddle.x += max(-padSpd, targetX - paddle.x);
  paddle.x = constrain(paddle.x, 0, SCR_W - padW);

  if (ball.active) {
    ball.x += ball.vx;
    ball.y += ball.vy;
    if (ball.x <= 1 || ball.x >= SCR_W - 1) {
      ball.vx = -ball.vx;
      ball.x = constrain(ball.x, 1, SCR_W - 1);
    }
    if (ball.y <= 1) {
      ball.vy = -ball.vy;
      ball.y = 1;
    }
    if (ball.y >= paddle.y - ballSz &&
        ball.y <= paddle.y + padH &&
        ball.x >= paddle.x && ball.x <= paddle.x + padW) {
      ball.vy = -ball.vy;
      float hitNorm = (ball.x - paddle.x) / float(padW);
      ball.vx = (hitNorm - 0.5) * 4;
      ball.y = paddle.y - ballSz;
      for (int i = 0; i < 3; i++) {
        for (auto &p : particles) {
          if (p.life <= 0) {
            p.x = ball.x;
            p.y = ball.y;
            p.vx = random(-50, 50) / 100.0;
            p.vy = random(-50, 50) / 100.0;
            p.life = random(5, 10);
            break;
          }
        }
      }
    }
    for (int r = 0; r < brkRows; r++) {
      for (int c = 0; c < brkCols; c++) {
        if (bricks[r][c] > 0) {
          float bx = c * (brkW + brkGap) + brkGap;
          float by = r * (brkH + brkGap) + brkGap + 5;
          if (ball.x >= bx && ball.x <= bx + brkW &&
              ball.y >= by && ball.y <= by + brkH) {
            int t = bricks[r][c];
            bricks[r][c] = 0;
            ball.vy = -ball.vy;
            score += 10;
            if (t == 2) {
              for (auto &b : bonuses) {
                if (!b.active) {
                  b.active = true;
                  b.x = bx + brkW / 2;
                  b.y = by + brkH / 2;
                  b.vy = 1.0;
                  b.type = 0;
                  break;
                }
              }
            }
            for (int i = 0; i < 5; i++) {
              for (auto &p : particles) {
                if (p.life <= 0) {
                  p.x = ball.x;
                  p.y = ball.y;
                  p.vx = random(-100, 100) / 100.0;
                  p.vy = random(-100, 100) / 100.0;
                  p.life = random(10, 20);
                  break;
                }
              }
            }
            bool empty = true;
            for (int rr = 0; rr < brkRows; rr++)
              for (int cc = 0; cc < brkCols; cc++)
                if (bricks[rr][cc] > 0) empty = false;
            if (empty) resetLevel();
            goto doneBrkChk;
          }
        }
      }
    }
    doneBrkChk:;

    if (ball.y > SCR_H) {
      lives--;
      if (lives <= 0) gameOver = true;
      else {
        ball.x = SCR_W / 2;
        ball.y = paddle.y - ballSz - 2;
        ball.vx = random(-150, 150) / 100.0;
        ball.vy = -2.0;
      }
    }
  }

  for (auto &b : bonuses) {
    if (b.active) {
      b.y += b.vy;
      if (b.y > SCR_H) b.active = false;
      else if (b.y >= paddle.y - 2 && b.y <= paddle.y + padH + 2 &&
               b.x >= paddle.x && b.x <= paddle.x + padW) {
        score += 50;
        b.active = false;
      }
    }
  }

  for (auto &p : particles) {
    if (p.life > 0) {
      p.x += p.vx;
      p.y += p.vy;
      p.life--;
    }
  }
}

void handleGameOver() {
  if (++gameOverTimer > 90) {
    initGame();
    gameOverTimer = 0;
  }
}

void drawGame() {
  display.clearDisplay();
  display.fillRect(paddle.x, paddle.y, padW, padH, SH110X_WHITE);
  if (ball.active)
    display.fillRect(ball.x - ballSz / 2, ball.y - ballSz / 2, ballSz, ballSz, SH110X_WHITE);
  for (int r = 0; r < brkRows; r++)
    for (int c = 0; c < brkCols; c++)
      if (bricks[r][c] > 0) {
        int bx = c * (brkW + brkGap) + brkGap;
        int by = r * (brkH + brkGap) + brkGap + 5;
        display.fillRect(bx, by, brkW, brkH, SH110X_WHITE);
      }
  for (auto &b : bonuses)
    if (b.active)
      display.fillRect(b.x - 2, b.y - 2, 4, 4, SH110X_WHITE);
  for (auto &p : particles)
    if (p.life > 0)
      display.drawPixel(p.x, p.y, SH110X_WHITE);

  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(2, 0);
  display.print("S:");
  display.print(score);
  display.setCursor(80, 0);
  display.print("L:");
  display.print(lives);

  if (gameOver) {
    display.fillRect(20, 20, 88, 24, SH110X_BLACK);
    display.drawRect(20, 20, 88, 24, SH110X_WHITE);
    display.setCursor(35, 25);
    display.println("GAME OVER");
    display.setCursor(25, 35);
    display.print("Score: ");
    display.print(score);
  }

  display.display();
}