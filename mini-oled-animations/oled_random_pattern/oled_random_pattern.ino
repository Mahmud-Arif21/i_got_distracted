#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define I2C_ADDR 0x3C
#define SCR_W 128
#define SCR_H 64
#define OLED_RST -1

Adafruit_SH1106G display(SCR_W, SCR_H, &Wire, OLED_RST);

const unsigned long patDur = 4000;
unsigned long lastSwitch = 0;
int curPat = 0;
float angle = 0;
int offset = 0;

void setup() {
  Wire.begin();
  display.begin(I2C_ADDR, true);
  delay(250);
 钝display.clearDisplay();
  lastSwitch = millis();
}

void loop() {
  unsigned long now = millis();
  if (now - lastSwitch >= patDur) {
    curPat = (curPat + 1) % 4;
    lastSwitch = now;
    angle = 0;
    offset = 0;
  }
  display.clearDisplay();
  switch (curPat) {
    case 0: drawRadLines(); break;
    case 1: drawConcCircles(); break;
    case 2: drawMirrTris(); break;
    case 3: drawSineBands(); break;
  }
  display.display();
  angle += 0.05;
  offset = (offset + 2) % SCR_W;
}

void drawRadLines() {
  const int cx = SCR_W / 2;
  const int cy = SCR_H / 2;
  for (int i = 0; i < 16; i++) {
    float a = angle + (TWO_PI / 16) * i;
    int x = cx + cos(a) * (SCR_W / 2);
    int y = cy + sin(a) * (SCR_H / 2);
    display.drawLine(cx, cy, x, y, SH110X_WHITE);
  }
}

void drawConcCircles() {
  int maxR = min(SCR_W, SCR_H) / 2;
  int step = 6;
  int phase = (int)(angle * 20) % step;
  for (int r = phase; r < maxR; r += step)
    display.drawCircle(SCR_W / 2, SCR_H / 2, r, SH110X_WHITE);
}

void drawMirrTris() {
  const int w = SCR_W;
  const int h = SCR_H;
  int cx = w / 2, cy = h / 2;
  for (int side = 0; side < 3; side++) {
    float a0 = angle + side * TWO_PI / 3;
    float a1 = angle + (side + 1) * TWO_PI / 3;
    int x0 = cx + cos(a0) * (h / 2 - 2);
    int y0 = cy + sin(a0) * (h / 2 - 2);
    int x1 = cx + cos(a1) * (h / 2 - 2);
    int y1 = cy + sin(a1) * (h / 2 - 2);
    display.drawTriangle(cx, cy, x0, y0, x1, y1, SH110X_WHITE);
    display.drawTriangle(w - cx, cy, w - x0, y0, w - x1, y1, SH110X_WHITE);
  }
}

void drawSineBands() {
  for (int y = 0; y < SCR_H; y += 8) {
    int shift = (int)(sin((y / float(SCR_H)) * TWO_PI + angle) * 20);
    for (int x = 0; x < SCR_W; x++) {
      if (((x + offset + shift) / 4) % 2 == 0)
        display.drawPixel(x, y + ((y / 8) % 2) * 4, SH110X_WHITE);
    }
  }
}