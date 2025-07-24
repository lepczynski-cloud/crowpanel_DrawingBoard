#include <Wire.h>
#include "LovyanGFX_Driver.h"  // LGFX driver provided by CrowPanel

LGFX gfx;  // Display object

// Last touch position
int16_t last_x = -1000;
int16_t last_y = -1000;

// Current drawing color
uint16_t currentColor = TFT_BLUE;

// Color button dimensions and positions
#define COLOR_BUTTON_SIZE 40
#define BUTTON_SPACING    10
#define BUTTON_Y          10
#define RED_BUTTON_X      (gfx.width() - 3 * COLOR_BUTTON_SIZE - 2 * BUTTON_SPACING)
#define GREEN_BUTTON_X    (gfx.width() - 2 * COLOR_BUTTON_SIZE - BUTTON_SPACING)
#define BLUE_BUTTON_X     (gfx.width() - 1 * COLOR_BUTTON_SIZE)

// Draw color selection buttons
void drawColorButtons() {
  gfx.fillRect(RED_BUTTON_X, BUTTON_Y, COLOR_BUTTON_SIZE, COLOR_BUTTON_SIZE, TFT_RED);
  gfx.fillRect(GREEN_BUTTON_X, BUTTON_Y, COLOR_BUTTON_SIZE, COLOR_BUTTON_SIZE, TFT_GREEN);
  gfx.fillRect(BLUE_BUTTON_X, BUTTON_Y, COLOR_BUTTON_SIZE, COLOR_BUTTON_SIZE, TFT_BLUE);
}

// Check if touch is within a color button area
bool isInside(int16_t tx, int16_t ty, int16_t bx, int16_t by, int16_t bsize) {
  return tx >= bx && tx < (bx + bsize) && ty >= by && ty < (by + bsize);
}

// I2C command to control backlight
void sendI2CCommand(uint8_t cmd) {
  Wire.beginTransmission(0x30);
  Wire.write(cmd);
  Wire.endTransmission();
}

// Reset GT911 touch controller
void resetGT911() {
  pinMode(1, OUTPUT);
  digitalWrite(1, LOW);
  delay(20);
  digitalWrite(1, HIGH);
  delay(100);
  pinMode(1, INPUT);  // floating
}

void setup() {
  Serial.begin(115200);
  Serial.println("CrowPanel 7\" v1.2 Init Start");

  Wire.begin(15, 16);  // SDA = GPIO15, SCL = GPIO16
  delay(100);

  // Enable backlight
  for (int i = 0; i < 5; ++i) {
    sendI2CCommand(0x10);
    delay(100);
  }
  Serial.println("Backlight ON");

  resetGT911();
  Serial.println("GT911 Reset OK");

  gfx.init();
  gfx.setRotation(0);
  gfx.fillScreen(TFT_BLACK);
  Serial.println("Display OK");

  gfx.setTextColor(TFT_WHITE);
  gfx.setTextSize(2);
  gfx.setCursor(50, 50);
  gfx.print("Touch the screen!");

  drawColorButtons();
}

void loop() {
  uint16_t x, y;

  if (gfx.getTouch(&x, &y)) {
    // Check if the touch is on any color button
    if (y >= BUTTON_Y && y < BUTTON_Y + COLOR_BUTTON_SIZE) {
      if (isInside(x, y, RED_BUTTON_X, BUTTON_Y, COLOR_BUTTON_SIZE)) {
        currentColor = TFT_RED;
        Serial.println("Changed color to RED");
        delay(200);
        return;
      } else if (isInside(x, y, GREEN_BUTTON_X, BUTTON_Y, COLOR_BUTTON_SIZE)) {
        currentColor = TFT_GREEN;
        Serial.println("Changed color to GREEN");
        delay(200);
        return;
      } else if (isInside(x, y, BLUE_BUTTON_X, BUTTON_Y, COLOR_BUTTON_SIZE)) {
        currentColor = TFT_BLUE;
        Serial.println("Changed color to BLUE");
        delay(200);
        return;
      }
    }

    // Ignore drawing in color button area
    if (y < BUTTON_Y + COLOR_BUTTON_SIZE + 10) return;

    // Drawing lines
    if (last_x == -1000 && last_y == -1000) {
      last_x = x;
      last_y = y;
    } else {
      gfx.drawLine(last_x, last_y, x, y, currentColor);
      last_x = x;
      last_y = y;
    }
  } else {
    last_x = -1000;
    last_y = -1000;
  }

  delay(5);  // Smoother drawing
}
