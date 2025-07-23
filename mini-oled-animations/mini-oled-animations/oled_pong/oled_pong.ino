#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define I2C_ADDR 0x3C
#define SCR_W 128
#define SCR_H 64
#define OLED_RST -1

Adafruit_SH1106G display(SCR_W, SCR_H, &Wire, OLED_RST);

float ballX = 64, ballY = 32;
float ballVx = 2.5, ballVy = 1.8;
int pad1Y = 28, pad2Y = 28;
int pad1Tgt = 28, pad2Tgt = 28;
int score1 = 0, score2 = 0;

const int padW = 3;
const int padH = 12;
const int padSpd = 2;
const int ballSz = 2;
const float maxBallSpd = 3.5;

struct Part { float x, y, vx, vy; int life; };
Part parts[8];
int activeParts = 0;

unsigned long lastUpdate = 0;
const int frameDelay = 33;

void setup() {
  display.begin(I2C_ADDR, true);
  delay(250);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(20, 20);
  display.println("AUTO");
  display.setCursor(20, 40);
  display.println("PONG");
  display.display();
  delay(2000);
  resetBall();
  randomSeed(analogRead(0));
}

void loop() {
  unsigned long now = millis();
  if (now - lastUpdate >= frameDelay) {
    updateGame();
    drawGame();
    lastUpdate = now;
  }
}

void updateGame() {
  ballX += ballVx;
  ballY += ballVy;
  if (ballY <= ballSz || ballY >= SCR_H - ballSz) {
    ballVy = -ballVy;
    createParts(ballX, ballY, 3);
  }
  if (ballX <= 8 && ballX >= 5 &&
      ballY >= pad1Y - ballSz && ballY <= pad1Y + padH + ballSz) {
    if (ballVx < 0) {
      ballVx = -ballVx;
      float hitPos = (ballY - pad1Y) / padH - 0.5;
      ballVy += hitPos * 1.5;
      limitBallSpd();
      createParts(ballX, ballY, 5);
    }
  }
  if (ballX >= 120 && ballX <= 123 &&
      ballY >= pad2Y - ballSz && ballY <= pad2Y + padH + ballSz) {
    if (ballVx > 0) {
      ballVx = -ballVx;
      float hitPos = (ballY - pad2Y) / padH - 0.5;
      ballVy += hitPos * 1.5;
      limitBallSpd();
      createParts(ballX, ballY, 5);
    }
  }
  if (ballX < 0) {
    score2++;
    createParts(10, ballY, 8);
    resetBall();
  } else if (ballX > SCR_W) {
    score1++;
    createParts(SCR_W - 10, ballY, 8);
    resetBall();
  }
  updatePadAI();
  updateParts();
}

void updatePadAI() {
  if (ballVx < 0) pad1Tgt = ballY - padH / 2;
  if (ballVx > 0) pad2Tgt = ballY - padH / 2;
  if (pad1Y < pad1Tgt) pad1Y = min(pad1Y + padSpd, pad1Tgt);
  else if (pad1Y > pad1Tgt) pad1Y = max(pad1Y - padSpd, pad1Tgt);
  if (pad2Y < pad2Tgt) pad2Y = min(pad2Y + padSpd, pad2Tgt);
  else if (pad2Y > pad2Tgt) pad2Y = max(pad2Y - padSpd, pad2Tgt);
  pad1Y = constrain(pad1Y, 0, SCR_H - padH);
  pad2Y = constrain(pad2Y, 0, SCR_H - padH);
}

void limitBallSpd() {
  float spd = sqrt(ballVx * ballVx + ballVy * ballVy);
  if (spd > maxBallSpd) {
    ballVx = (ballVx / spd) * maxBallSpd;
    ballVy = (ballVy / spd) * maxBallSpd;
  }
}

void resetBall() {
  ballX = SCR_W / 2;
  ballY = random(ballSz + 5, SCR_H - ballSz - 5);
  ballVx = random(0, 2) ? 2.5 : -2.5;
  ballVy = random(-200, 200) / 100.0;
  delay(500);
}

void createParts(float x, float y, int cnt) {
  for (int i = 0; i < cnt && activeParts < 8; i++) {
    parts[activeParts].x = x;
    parts[activeParts].y = y;
    parts[activeParts].vx = random(-300, 300) / 100.0;
    parts[activeParts].vy = random(-300, 300) / 100.0;
    parts[activeParts].life = random(15, 30);
    activeParts++;
  }
}

void updateParts() {
  for (int i = 0; i < activeParts; i++) {
    parts[i].x += parts[i].vx;
    parts[i].y += parts[i].vy;
    parts[i].vx *= 0.95;
    parts[i].vy *= 0.95;
    parts[i].life--;
    if (parts[i].life <= 0) {
      parts[i] = parts[activeParts - 1];
      activeParts--;
      i--;
    }
  }
}

void drawGame() {
  display.clearDisplay();
  for (int y = 0; y < SCR_H; y += 4)
    display.drawPixel(SCR_W / 2, y, SH110X_WHITE);
  display.fillRect(5, pad1Y, padW, padH, SH110X_WHITE);
  display.fillRect(SCR_W - 8, pad2Y, padW, padH, SH110X_WHITE);
  display.fillCircle(ballX, ballY, ballSz, SH110X_WHITE);
  for (int i = 0; i < activeParts; i++)
    display.drawPixel(parts[i].x, parts[i].y, SH110X_WHITE);
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(45, 2);
  display.print(score1);
  display.setCursor(75, 2);
  display.print(score2);
  float spd = sqrt(ballVx * ballVx + ballVy * ballVy);
  int spdBars = map(spd * 10, 15, 35, 1, 5);
  for (int i = 0; i < spdBars; i++)
    display.drawPixel(60 + i * 2, 55, SH110X_WHITE);
  display.display();
}