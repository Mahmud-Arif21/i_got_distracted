#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define I2C_ADDR 0x3C
#define SCR_W 128
#define SCR_H 64
#define OLED_RST -1

Adafruit_SH1106G display(SCR_W, SCR_H, &Wire, OLED_RST);

const uint16_t bpm = 75;
const float beatMs = 60000.0f / bpm;
const int ecgW = SCR_W;
int16_t ecgBuf[ecgW];
uint16_t ecgPos = 0;
uint32_t startMs, lastFrame;

const float pStart = 0.10f, pEnd = 0.18f;
const float qrsStart = 0.20f, qrsEnd = 0.28f;
const float tStart = 0.40f, tEnd = 0.58f;

const float rCtr = 0.5f * (qrsStart + qrsEnd);
const float tCtr = 0.5f * (tStart + tEnd);
const float sigma = (qrsEnd - qrsStart) * 0.5f;
const float primAmp = 0.5f;
const float secAmp = 0.25f;

float ecgVal(float phi) {
  if (phi >= pStart && phi < pEnd) {
    float x = (phi - pStart) / (pEnd - pStart);
    return 10 * sinf(PI * x);
  }
  if (phi >= qrsStart && phi < qrsEnd) {
    float x = (phi - qrsStart) / (qrsEnd - qrsStart);
    if (x < 0.25f) return -40 * (x / 0.25f);
    else if (x < 0.50f) return 120 * ((x - 0.25f) / 0.25f);
    else if (x < 0.75f) return -60 * ((x - 0.50f) / 0.25f);
    else return 0;
  }
  if (phi >= tStart && phi < tEnd) {
    float x = (phi - tStart) / (tEnd - tStart);
    return 30 * sinf(PI * x);
  }
  return 0;
}

void drawHeart(float phi) {
  float d1 = phi - rCtr;
  if (d1 < -0.5f) d1 += 1.0f;
  if (d1 > 0.5f) d1 -= 1.0f;
  float p1 = expf(-0.5f * (d1 * d1) / (sigma * sigma));

  float d2 = phi - tCtr;
  if (d2 < -0.5f) d2 += 1.0f;
  if (d2 > 0.5f) d2 -= 1.0f;
  float p2 = expf(-0.5f * (d2 * d2) / (sigma * sigma));

  float scale = 1.0f + primAmp * p1 + secAmp * p2;

  int cx = 20, cy = 20;
  int r = int(6 * scale);
  display.fillCircle(cx - r / 2, cy - r / 3, r / 2, SH110X_WHITE);
  display.fillCircle(cx + r / 2, cy - r / 3, r / 2, SH110X_WHITE);
  display.fillTriangle(cx - r, cy - r / 6, cx + r, cy - r / 6, cx, cy + r, SH110X_WHITE);
}

void setup() {
  display.begin(I2C_ADDR, true);
  delay(250);
  display.clearDisplay();
  startMs = millis();
  lastFrame = startMs;
  for (int i = 0; i < ecgW; i++) ecgBuf[i] = 0;
}

void loop() {
  uint32_t ms = millis();
  if (ms - lastFrame < 20) return;
  lastFrame = ms;

  float t = float(ms - startMs);
  float phi = fmodf(t / beatMs, 1.0f);

  float val = ecgVal(phi);
  ecgBuf[ecgPos] = int16_t(val);
  ecgPos = (ecgPos + 1) % ecgW;

  display.clearDisplay();
  drawHeart(phi);

  char buf[8];
  sprintf(buf, "BPM:%d", bpm);
  display.setCursor(SCR_W - 6 * strlen(buf) - 2, 2);
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.print(buf);

  int baseY = SCR_H - 12;
  for (int x = 0; x < ecgW - 1; x++) {
    int i1 = (ecgPos + x) % ecgW;
    int i2 = (ecgPos + x + 1) % ecgW;
    int y1 = baseY - int(ecgBuf[i1] * 0.3f);
    int y2 = baseY - int(ecgBuf[i2] * 0.3f);
    display.drawLine(x, y1, x + 1, y2, SH110X_WHITE);
  }

  display.display();
}