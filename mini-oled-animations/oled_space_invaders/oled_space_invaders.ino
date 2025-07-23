#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define I2C_ADDR 0x3C
#define SCR_W 128
#define SCR_H 64
#define OLED_RST -1

Adafruit_SH1106G display(SCR_W, SCR_H, &Wire, OLED_RST);

#define shipW 12
#define shipH 4
#define alienW 8
#define alienH 6
#define alienGap 4
#define shelterW 12
#define shelterH 6
#define shelterCnt 4
#define bulletW 2
#define bulletH 4

uint8_t alienRows = 4;
uint8_t alienCols = 6;
uint16_t alienMoveMs = 500;
uint8_t alienStepX = 4;
uint8_t alienStepY = 4;
uint8_t shipSpd = 3;
uint8_t bulletSpd = 4;
float descendSpd = 0.25;
const uint16_t frameMs = 30;
const uint16_t gameOverMs = 2000;

struct Bullet { float x, y; bool active; } shipBullet;
struct Alien { float x, y; bool alive; } aliens[8][8];
struct Shelter { int8_t x, y; uint8_t hp; } shelters[shelterCnt];

float shipX;
int alienDir;
uint32_t lastAlienMove, lastFrame, gameOverTime;
uint16_t score;
uint8_t level;
bool gameOver;

void setup() {
  display.begin(I2C_ADDR, true);
  delay(250);
  randomSeed(analogRead(A0));
  resetGame();
}

void loop() {
  uint32_t now = millis();
  if (now - lastFrame < frameMs) return;
  lastFrame = now;
  if (!gameOver) {
    aiControl();
    updateBullet();
    for (uint8_t r = 0; r < alienRows; r++)
      for (uint8_t c = 0; c < alienCols; c++)
        if (aliens[r][c].alive)
          aliens[r][c].y += descendSpd;
    if (now - lastAlienMove >= alienMoveMs) {
      moveAliens();
      lastAlienMove = now;
    }
    checkLevel();
  } else {
    if (now - gameOverTime >= gameOverMs) resetGame();
  }
  drawFrame();
}

void resetGame() {
  shipX = (SCR_W - shipW) / 2;
  shipBullet.active = false;
  alienDir = 1;
  score = 0;
  level = 1;
  gameOver = false;
  spawnAliens();
  spawnShelters();
  lastAlienMove = millis();
  lastFrame = millis();
}

void spawnAliens() {
  float startY = -(alienRows * (alienH + alienGap));
  for (uint8_t r = 0; r < alienRows; r++)
    for (uint8_t c = 0; c < alienCols; c++) {
      aliens[r][c].alive = true;
      aliens[r][c].x = c * (alienW + alienGap) + alienGap;
      aliens[r][c].y = startY + r * (alienH + alienGap);
    }
}

void spawnShelters() {
  for (uint8_t i = 0; i < shelterCnt; i++) {
    shelters[i].hp = shelterH;
    shelters[i].x = map(i, 0, shelterCnt - 1, 16, SCR_W - 16 - shelterW);
    shelters[i].y = SCR_H - 24;
  }
}

void aiControl() {
  float tx = shipX;
  bool found = false;
  float maxY = -1;
  for (uint8_t r = 0; r < alienRows; r++)
    for (uint8_t c = 0; c < alienCols; c++)
      if (aliens[r][c].alive && aliens[r][c].y > maxY) {
        maxY = aliens[r][c].y;
        tx = aliens[r][c].x + (alienW - shipW) / 2.0;
        found = true;
      }
  if (found) {
    if (shipX < tx) shipX += shipSpd;
    else if (shipX > tx) shipX -= shipSpd;
    shipX = constrain(shipX, 0, SCR_W - shipW);
  }
  if (!shipBullet.active && found) {
    shipBullet.active = true;
    shipBullet.x = shipX + (shipW - bulletW) / 2.0;
    shipBullet.y = SCR_H - shipH - bulletH - 2;
  }
}

void updateBullet() {
  if (!shipBullet.active) return;
  shipBullet.y -= bulletSpd;
  if (shipBullet.y < 0) {
    shipBullet.active = false;
    return;
  }
  for (uint8_t r = 0; r < alienRows; r++)
    for (uint8_t c = 0; c < alienCols; c++) {
      if (!aliens[r][c].alive) continue;
      if (shipBullet.x >= aliens[r][c].x &&
          shipBullet.x <= aliens[r][c].x + alienW &&
          shipBullet.y >= aliens[r][c].y &&
          shipBullet.y <= aliens[r][c].y + alienH) {
        aliens[r][c].alive = false;
        shipBullet.active = false;
        score += 10;
        return;
      }
    }
  for (uint8_t i = 0; i < shelterCnt; i++) {
    auto &s = shelters[i];
    if (s.hp == 0) continue;
    if (shipBullet.x >= s.x &&
        shipBullet.x <= s.x + shelterW &&
        shipBullet.y >= s.y &&
        shipBullet.y <= s.y + s.hp) {
      s.hp--;
      shipBullet.active = false;
      return;
    }
  }
}

void moveAliens() {
  bool edge = false;
  for (uint8_t r = 0; r < alienRows && !edge; r++)
    for (uint8_t c = 0; c < alienCols; c++) {
      auto &a = aliens[r][c];
      if (!a.alive) continue;
      float nx = a.x + alienDir * alienStepX;
      if (nx < 0 || nx + alienW > SCR_W) {
        edge = true;
        break;
      }
    }
  for (uint8_t r = 0; r < alienRows; r++)
    for (uint8_t c = 0; c < alienCols; c++) {
      auto &a = aliens[r][c];
      if (!a.alive) continue;
      if (edge) a.y += alienStepY;
      else a.x += alienDir * alienStepX;
    }
  if (edge) alienDir = -alienDir;
}

void checkLevel() {
  bool any = false;
  for (uint8_t r = 0; r < alienRows; r++)
    for (uint8_t c = 0; c < alienCols; c++)
      if (aliens[r][c].alive) any = true;
  if (!any) {
    level++;
    if (alienMoveMs > 100) alienMoveMs -= 50;
    spawnAliens();
    spawnShelters();
  }
  for (uint8_t r = 0; r < alienRows; r++)
    for (uint8_t c = 0; c < alienCols; c++)
      if (aliens[r][c].alive &&
          aliens[r][c].y + alienH >= SCR_H - shipH - 2) {
        if (!gameOver) {
          gameOver = true;
          gameOverTime = millis();
        }
      }
}

void drawFrame() {
  display.clearDisplay();
  int shipY = SCR_H - shipH - 1;
  display.fillRect(shipX, shipY, shipW, shipH, SH110X_WHITE);
  display.drawLine(shipX, shipY, shipX + shipW / 2, shipY - shipH, SH110X_WHITE);
  display.drawLine(shipX + shipW, shipY, shipX + shipW / 2, shipY - shipH, SH110X_WHITE);
  for (uint8_t r = 0; r < alienRows; r++)
    for (uint8_t c = 0; c < alienCols; c++) {
      auto &a = aliens[r][c];
      if (!a.alive) continue;
      display.drawRect(a.x, a.y, alienW, alienH, SH110X_WHITE);
    }
  for (uint8_t i = 0; i < shelterCnt; i++) {
    auto &s = shelters[i];
    if (s.hp == 0) continue;
    display.fillRect(s.x, s.y, shelterW, s.hp, SH110X_WHITE);
  }
  if (shipBullet.active)
    display.fillRect(shipBullet.x, shipBullet.y, bulletW, bulletH, SH110X_WHITE);
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(2, 0);
  display.print("S:");
  display.print(score);
  display.setCursor(80, 0);
  display.print("L:");
  display.print(level);
  if (gameOver) {
    display.setCursor(30, SCR_H / 2 - 4);
    display.println("GAME OVER");
  }
  display.display();
}