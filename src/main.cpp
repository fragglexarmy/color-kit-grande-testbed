#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "settings.h"

static LGFX display;

int32_t width, height;

static volatile uint32_t touchInterruptCount = 0;

void IRAM_ATTR onTouchInterrupt(void) {
  touchInterruptCount++;
}

void setup(void) {
  Serial.begin(115200);

  display.init();

  pinMode(TOUCH_INT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(TOUCH_INT), onTouchInterrupt, FALLING);
  log_i("TOUCH_INT interrupt logging enabled on GPIO %d", TOUCH_INT);

  width = display.width();
  height = display.height();

  log_i("Display width: %d, height: %d", width, height);
  log_i("Rotation: %d", display.getRotation());
  log_i("RGB color order: %s", (display.panel()->config().rgb_order == 0) ? "RGB" : "BGR");

  // square side length
  int8_t l = 10;

  display.fillRect(0, 0, l, l, TFT_WHITE);
  display.fillRect(width - l, 0, l, l, TFT_RED);
  display.fillRect(width - l, height - l, l, l, TFT_GREEN);
  display.fillRect(0, height - l, l, l, TFT_BLUE);

  display.setTextColor(TFT_WHITE);
  display.setFont(&fonts::FreeSans9pt7b);
  // top left
  display.drawString("0", 20, 20);
  display.drawString("white", 20, 40);
  // top right
  display.drawRightString(String(width) + "/0", width - 20, 20);
  display.drawRightString("red", width -20, 40);
  // bottom right
  display.drawRightString(String(width) + "/" + String(height), width - 20, height - 40);
  display.drawRightString("green", width - 20, height - 60);
  // bottom left
  display.drawString("0/" + String(height), 20, height - 40);
  display.drawString("blue", 20, height - 60);
}

void loop(void) {
  uint32_t interruptCount = 0;
  noInterrupts();
  interruptCount = touchInterruptCount;
  touchInterruptCount = 0;
  interrupts();

  if (interruptCount > 0) {
    log_i("TOUCH_INT interrupt triggered (%u pending)", interruptCount);
  }

  int32_t x = 0, y = 0;
  if (display.getTouch(&x, &y) && x > -1 && x < width && y > -1) {
    log_i("Touch detected at %d/%d", x, y);
    display.fillRect(x, y, 5, 5, TFT_ORANGE);
  }
}
