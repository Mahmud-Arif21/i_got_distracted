#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define I2C_ADDR 0x3C
#define SCR_W 128
#define SCR_H 64
#define OLED_RST -1

Adafruit_SH1106G display(SCR_W, SCR_H, &Wire, OLED_RST);

struct Planet {
  float orbitR;
  float angSpd;
  uint8_t sz;
  uint16_t color;
  float angOff;
};

Planet planets[] = {
  {20, 0.0015, 2, SH110X_WHITE, 0.0},
  {30, 0.0010, 3, SH110X_WHITE, PI / 4},
  {40, 0.0007, 4, SH110X_WHITE, PI / 2},
  {50, 0.0005, 3, SH110X_WHITE, PI}
};

const int numPlanets = sizeof(planets) / sizeof(planets[0]);
const int cx = SCR_W / 2;
const int cy = SCR_H / 2;

void setup() {
  display.begin(I2C_ADDR, true);
  delay(250);
  display.clearDisplay();
}

void loop() {
  unsigned long t = millis();
  display.clearDisplay();
  display.fillCircle(cx, cy, 6, SH110X_WHITE);
  for (int i = 0; i < numPlanets; i++) {
    Planet &p = planets[i];
    float angle = p.angOff + t * p.angSpd;
    int x = cx + cos(angle) * p.orbitR;
    int y = cy + sin(angle) * p.orbitR;
    display.drawCircle(cx, cy, p.orbitR, SH110X_WHITE);
    display.fillCircle(x, y, p.sz, SH110X_WHITE);
  }
  display.display();
  delay(20);
}