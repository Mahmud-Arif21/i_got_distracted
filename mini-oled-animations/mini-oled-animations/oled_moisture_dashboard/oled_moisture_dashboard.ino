#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define I2C_ADDR 0x3C
#define SCR_W 128
#define SCR_H 64
#define OLED_RST -1

Adafruit_SH1106G display(SCR_W, SCR_H, &Wire, OLED_RST);

bool waterOn = false;

void setup() {
  display.begin(I2C_ADDR, true);
  delay(250);
  display.clearDisplay();
}

int readMoist() {
  unsigned long t = millis();
  return 20 + 30 * (0.5 + 0.5 * sin(t * 0.001));
}

void updateWater(int moist) {
  waterOn = (moist < 40);
}

void drawDrop(int x, int y, uint8_t sz, uint16_t color) {
  int r = sz / 2;
  int tipLen = sz;
  int cx = x;
  int cy = y + r / 2;
  int tx = x;
  int ty = y - r - tipLen;
  int t1x = cx - r;
  int t1y = cy;
  int t2x = cx + r;
  int t2y = cy;
  display.fillCircle(cx, cy, r, color);
  display.fillTriangle(cx, cy, t1x, t1y, tx, ty, color);
  display.fillTriangle(cx, cy, t2x, t2y, tx, ty, color);
}

void drawBars(int x, int y, int val) {
  int h = map(val, 20, 80, 5, 25);
  int w = 4;
  int sp = 2;
  for (int i = 0; i < 5; i++) {
    int ht = h * (i + 1) / 5;
    display.fillRect(x + i * (w + sp), y - ht, w, ht, SH110X_WHITE);
  }
}

void drawX(int x, int y, uint8_t sz, uint16_t color) {
  display.drawLine(x - sz / 4, y - sz / 4, x + sz / 4, y + sz / 4, color);
  display.drawLine(x - sz / 4, y + sz / 4, x + sz / 4, y - sz / 4, color);
}

void drawTick(int x, int y, uint8_t sz, uint16_t color) {
  display.drawLine(x - sz / 3, y, x, y + sz / 3, color);
  display.drawLine(x, y + sz / 3, x + sz / 2, y - sz / 3, color);
}

void drawLeft(int moist) {
  drawDrop(10, 5, 4, SH110X_WHITE);
  drawBars(20, 10, moist);
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(5, 15);
  display.print("Moisture:");
  display.setTextSize(2);
  display.setCursor(5, 35);
  display.print(moist);
  display.print("%");
}

void drawRight() {
  int rx = 90;
  int ry = 5;
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  if (waterOn) drawTick(rx, ry, 10, SH110X_WHITE);
  else drawX(rx, ry, 10, SH110X_WHITE);
  display.setCursor(85, 15);
  display.print("Water");
  display.setTextSize(2);
  display.setCursor(85, 35);
  display.print(waterOn ? "ON" : "OFF");
}

void loop() {
  display.clearDisplay();
  int moist = readMoist();
  updateWater(moist);
  drawLeft(moist);
  display.drawLine(64, 0, 64, SCR_H, SH110X_WHITE);
  drawRight();
  display.display();
  delay(50);
}