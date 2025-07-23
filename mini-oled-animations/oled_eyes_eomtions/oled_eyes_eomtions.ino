#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <math.h>

#define I2C_ADDR 0x3C
#define SCR_W 128
#define SCR_H 64
#define OLED_RST -1

Adafruit_SH1106G display(SCR_W, SCR_H, &Wire, OLED_RST);

int refEyeH = 40;
int refEyeW = 40;
int refEyeGap = 10;
int refCornerR = 10;

int lEyeH = refEyeH;
int lEyeW = refEyeW;
int lEyeX = 32;
int lEyeY = 32;
int rEyeX = 32 + refEyeW + refEyeGap;
int rEyeY = 32;
int rEyeH = refEyeH;
int rEyeW = refEyeW;

void drawEyes(bool update = true) {
  display.clearDisplay();
  int x = lEyeX - lEyeW / 2;
  int y = lEyeY - lEyeH / 2;
  display.fillRoundRect(x, y, lEyeW, lEyeH, refCornerR, SH110X_WHITE);
  x = rEyeX - rEyeW / 2;
  y = rEyeY - rEyeH / 2;
  display.fillRoundRect(x, y, rEyeW, rEyeH, refCornerR, SH110X_WHITE);
  if (update) display.display();
}

void centerEyes(bool update = true) {
  lEyeH = refEyeH;
  lEyeW = refEyeW;
  rEyeH = refEyeH;
  rEyeW = refEyeW;
  lEyeX = SCR_W / 2 - refEyeW / 2 - refEyeGap / 2;
  lEyeY = SCR_H / 2;
  rEyeX = SCR_W / 2 + refEyeW / 2 + refEyeGap / 2;
  rEyeY = SCR_H / 2;
  drawEyes(update);
}

void blink(int spd = 12) {
  drawEyes();
  for (int i = 0; i < 3; i++) {
    lEyeH = max(1, lEyeH - spd);
    rEyeH = max(1, rEyeH - spd);
    drawEyes();
    delay(10);
  }
  for (int i = 0; i < 3; i++) {
    lEyeH = min(refEyeH, lEyeH + spd);
    rEyeH = min(refEyeH, rEyeH + spd);
    drawEyes();
    delay(10);
  }
}

void sleep() {
  lEyeH = 2;
  rEyeH = 2;
  drawEyes(true);
}

void wakeup() {
  sleep();
  for (int h = 0; h <= refEyeH; h += 2) {
    lEyeH = h;
    rEyeH = h;
    drawEyes(true);
  }
}

void happyEyes() {
  centerEyes(false);
  int offset = refEyeH / 2;
  for (int i = 0; i < 10; i++) {
    display.fillTriangle(lEyeX - lEyeW / 2 - 1, lEyeY + offset,
                         lEyeX + lEyeW / 2 + 1, lEyeY + 5 + offset,
                         lEyeX - lEyeW / 2 - 1, lEyeY + lEyeH + offset,
                         SH110X_BLACK);
    display.fillTriangle(rEyeX + rEyeW / 2 + 1, rEyeY + offset,
                         rEyeX - lEyeW / 2 - 1, rEyeY + 5 + offset,
                         rEyeX + rEyeW / 2 + 1, rEyeY + rEyeH + offset,
                         SH110X_BLACK);
    offset -= 2;
    display.display();
    delay(1);
  }
  display.display();
  delay(1000);
}

void dizzyEyes() {
  centerEyes(false);
  float angleOffset = 0;
  for (int frame = 0; frame < 40; frame++) {
    drawEyes(false);
    drawSpiral(lEyeX, lEyeY, 12, angleOffset);
    drawSpiral(rEyeX, rEyeY, 12, -angleOffset);
    display.display();
    delay(80);
    angleOffset += 0.3;
  }
}

void drawSpiral(int cx, int cy, float maxR, float angleShift) {
  float a = 1.0;
  float b = 1.5;
  float thetaStep = 0.3;
  float prevX = cx;
  float prevY = cy;
  for (float theta = 0; theta < 6.28 * 3; theta += thetaStep) {
    float r = a + b * theta;
    if (r > maxR) break;
    float x = cx + r * cos(theta + angleShift);
    float y = cy + r * sin(theta + angleShift);
    display.drawLine(prevX, prevY, x, y, SH110X_BLACK);
    display.fillCircle(x, y, 1, SH110X_BLACK);
    prevX = x;
    prevY = y;
  }
}

void sadEyes() {
  centerEyes(false);
  int offset = -refEyeH / 2;
  for (int i = 0; i < 10; i++) {
    display.fillTriangle(lEyeX - lEyeW / 2 - 1, lEyeY + offset,
                         lEyeX + lEyeW / 2 + 1, lEyeY - 5 + offset,
                         lEyeX - lEyeW / 2 - 1, lEyeY - lEyeH + offset,
                         SH110X_BLACK);
    display.fillTriangle(rEyeX + rEyeW / 2 + 1, rEyeY + offset,
                         rEyeX - rEyeW / 2 - 1, rEyeY - 5 + offset,
                         rEyeX + rEyeW / 2 + 1, rEyeY - rEyeH + offset,
                         SH110X_BLACK);
    offset += 2;
    display.display();
    delay(50);
  }
  display.display();
  delay(1000);
}

void angryEyes() {
  centerEyes(false);
  int offset = -refEyeH / 2;
  for (int i = 0; i < 10; i++) {
    display.fillTriangle(lEyeX + lEyeW / 2 + 1, lEyeY + offset,
                         lEyeX - lEyeW / 2 - 1, lEyeY - 10 + offset,
                         lEyeX + lEyeW / 2 + 1, lEyeY - lEyeH + offset,
                         SH110X_BLACK);
    display.fillTriangle(rEyeX - rEyeW / 2 - 1, rEyeY + offset,
                         rEyeX + rEyeW / 2 + 1, rEyeY - 10 + offset,
                         rEyeX - rEyeW / 2 - 1, rEyeY - rEyeH + offset,
                         SH110X_BLACK);
    offset += 2;
    display.display();
    delay(50);
  }
  display.display();
  delay(1000);
}

void saccade(int dx, int dy) {
  int moveX = 8, moveY = 6, blinkAmp = 8;
  for (int i = 0; i < 1; i++) {
    lEyeX += moveX * dx;
    rEyeX += moveX * dx;
    lEyeY += moveY * dy;
    rEyeY += moveY * dy;
    lEyeH -= blinkAmp;
    rEyeH -= blinkAmp;
    drawEyes();
    delay(1);
  }
  for (int i = 0; i < 1; i++) {
    lEyeX += moveX * dx;
    rEyeX += moveX * dx;
    lEyeY += moveY * dy;
    rEyeY += moveY * dy;
    lEyeH += blinkAmp;
    rEyeH += blinkAmp;
    drawEyes();
    delay(1);
  }
}

void moveBigEye(int dir) {
  int oversize = 1, moveAmp = 2, blinkAmp = 5;
  for (int i = 0; i < 3; i++) {
    lEyeX += moveAmp * dir;
    rEyeX += moveAmp * dir;
    lEyeH -= blinkAmp;
    rEyeH -= blinkAmp;
    if (dir > 0) {
      rEyeH += oversize;
      rEyeW += oversize;
    } else {
      lEyeH += oversize;
      lEyeW += oversize;
    }
    drawEyes();
    delay(1);
  }
  for (int i = 0; i < 3; i++) {
    lEyeX += moveAmp * dir;
    rEyeX += moveAmp * dir;
    lEyeH += blinkAmp;
    rEyeH += blinkAmp;
    if (dir > 0) {
      rEyeH += oversize;
      rEyeW += oversize;
    } else {
      lEyeH += oversize;
      lEyeW += oversize;
    }
    drawEyes();
    delay(1);
  }
  delay(1000);
  for (int i = 0; i < 3; i++) {
    lEyeX -= moveAmp * dir;
    rEyeX -= moveAmp * dir;
    lEyeH -= blinkAmp;
    rEyeH -= blinkAmp;
    if (dir > 0) {
      rEyeH -= oversize;
      rEyeW -= oversize;
    } else {
      lEyeH -= oversize;
      lEyeW -= oversize;
    }
    drawEyes();
    delay(1);
  }
  for (int i = 0; i < 3; i++) {
    lEyeX -= moveAmp * dir;
    rEyeX -= moveAmp * dir;
    lEyeH += blinkAmp;
    rEyeH += blinkAmp;
    if (dir > 0) {
      rEyeH -= oversize;
      rEyeW -= oversize;
    } else {
      lEyeH -= oversize;
      lEyeW -= oversize;
    }
    drawEyes();
    delay(1);
  }
  centerEyes();
}

void moveRightBigEye() { moveBigEye(1); }
void moveLeftBigEye() { moveBigEye(-1); }

void setup() {
  display.begin(I2C_ADDR, true);
  delay(250);
  display.clearDisplay();
  display.display();
  sleep();
}

void loop() {
  wakeup();
  delay(500);
  centerEyes(true);
  delay(500);
  moveRightBigEye();
  delay(500);
  moveLeftBigEye();
  delay(500);
  blink(10);
  delay(300);
  blink(20);
  delay(500);
  happyEyes();
  delay(500);
  sadEyes();
  delay(500);
  angryEyes();
  delay(500);
  dizzyEyes();
  delay(500);
}