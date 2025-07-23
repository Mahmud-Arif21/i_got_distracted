#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define I2C_ADDR 0x3C
#define SCR_W 128
#define SCR_H 64
#define OLED_RST -1

Adafruit_SH1106G display(SCR_W, SCR_H, &Wire, OLED_RST);

const int fontW = 6;
const int fontH = 8;
const int cols = SCR_W / fontW;
const int rows = SCR_H / fontH;

int heads[cols];
int speeds[cols];
char chars[rows][cols];
unsigned long lastUpdate = 0;
const int frameDelay = 80;
const int tailLen = 4;

char randChar() {
  int r = random(0, 3);
  if (r == 0) return char(random('0', '9' + 1));
  if (r == 1) return char(random('A', 'Z' + 1));
  return char(random('a', 'z' + 1));
}

void setup() {
  display.begin(I2C_ADDR, true);
  delay(250);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  randomSeed(analogRead(0));
  for (int c = 0; c < cols; c++) {
    heads[c] = random(0, rows);
    speeds[c] = random(1, 3);
    for (int r = 0; r < rows; r++) {
      chars[r][c] = randChar();
    }
  }
}

void loop() {
  if (millis() - lastUpdate < frameDelay) return;
  lastUpdate = millis();

  for (int c = 0; c < cols; c++) {
    for (int s = 0; s < speeds[c]; s++) {
      heads[c] = (heads[c] + 1) % rows;
      chars[heads[c]][c] = randChar();
    }
  }

  display.clearDisplay();
  for (int c = 0; c < cols; c++) {
    int h = heads[c];
    for (int t = 0; t < tailLen; t++) {
      int row = (h - t + rows) % rows;
      int x = c * fontW;
      int y = row * fontH;
      if (t == 0) {
        display.setCursor(x, y);
        display.write(chars[row][c]);
      } else if (t < tailLen && (t % 2 == 0)) {
        display.setCursor(x, y);
        display.write(chars[row][c]);
      }
    }
  }
  display.display();
}