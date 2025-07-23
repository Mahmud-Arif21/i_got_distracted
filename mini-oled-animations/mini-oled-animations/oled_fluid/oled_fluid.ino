#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define I2C_ADDR 0x3C
#define SCR_W 128
#define SCR_H 64
#define OLED_RST -1

Adafruit_SH1106G display(SCR_W, SCR_H, &Wire, OLED_RST);

const int numParts = 100;
const float grav = 0.04;
const float damp = 0.9;
const float angVel = 0.01;
const float halfSz = 20.0;
const float cx = SCR_W / 2;
const float cy = SCR_H / 2;

struct Part { float x, y, vx, vy; } parts[numParts];
float angle = 0;

void setup() {
  display.begin(I2C_ADDR, true);
  delay(250);
  display.clearDisplay();
  randomSeed(analogRead(A0));
  for (int i = 0; i < numParts; i++) {
    parts[i].x = cx + random(-halfSz + 2, halfSz - 2);
    parts[i].y = cy + random(-halfSz + 2, halfSz - 2);
    parts[i].vx = parts[i].vy = 0;
  }
}

void loop() {
  display.clearDisplay();
  float cs = cos(angle), sn = sin(angle);
  float gx = 0, gy = grav;

  float corners[4][2] = {
    {-halfSz, -halfSz},
    {halfSz, -halfSz},
    {halfSz, halfSz},
    {-halfSz, halfSz}
  };
  int X[5], Y[5];
  for (int i = 0; i < 4; i++) {
    float lx = corners[i][0], ly = corners[i][1];
    X[i] = cx + lx * cs - ly * sn;
    Y[i] = cy + lx * sn + ly * cs;
  }
  X[4] = X[0];
  Y[4] = Y[0];
  for (int i = 0; i < 4; i++)
    display.drawLine(X[i], Y[i], X[i + 1], Y[i + 1], SH110X_WHITE);

  for (int i = 0; i < numParts; i++) {
    float dx = parts[i].x - cx, dy = parts[i].y - cy;
    float lx = dx * cs + dy * sn;
    float ly = -dx * sn + dy * cs;
    float glx = gx * cs + gy * sn;
    float gly = -gx * sn + gy * cs;
    parts[i].vx += glx;
    parts[i].vy += gly;
    lx += parts[i].vx;
    ly += parts[i].vy;
    if (lx < -halfSz + 1) {
      lx = -halfSz + 1;
      parts[i].vx = -parts[i].vx * damp;
    } else if (lx > halfSz - 1) {
      lx = halfSz - 1;
      parts[i].vx = -parts[i].vx * damp;
    }
    if (ly < -halfSz + 1) {
      ly = -halfSz + 1;
      parts[i].vy = -parts[i].vy * damp;
    } else if (ly > halfSz - 1) {
      ly = halfSz - 1;
      parts[i].vy = -parts[i].vy * damp;
    }
    parts[i].x = cx + lx * cs - ly * sn;
    parts[i].y = cy + lx * sn + ly * cs;
    display.drawPixel(int(parts[i].x), int(parts[i].y), SH110X_WHITE);
  }

  int visCnt = 0;
  for (int i = 0; i < numParts; i++) {
    if (parts[i].x >= 0 && parts[i].x < SCR_W && parts[i].y >= 0 && parts[i].y < SCR_H) {
      visCnt++;
    }
  }
  display.setCursor(0, 0);
  display.setTextColor(SH110X_WHITE);
  display.print("Particles: ");
  display.print(visCnt);

  display.display();
  angle += angVel;
  if (angle > TWO_PI) angle -= TWO_PI;
}