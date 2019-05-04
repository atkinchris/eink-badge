#include <GxEPD.h>
#include <GxGDE0213B72/GxGDE0213B72.h>
#include <Fonts/FreeMono9pt7b.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>

#include "pinout.h"

// GxIO_SPI(SPIClass& spi, int8_t cs, int8_t dc, int8_t rst = -1, int8_t bl = -1);
GxIO_Class io(SPI, ELINK_SS, ELINK_DC, ELINK_RESET);
// GxGDEP015OC1(GxIO& io, uint8_t rst = D4, uint8_t busy = D2);
GxEPD_Class display(io, ELINK_RESET, ELINK_BUSY);

void setup() {
  int16_t x, y, x1, y1;
  uint16_t w, h;
  Serial.begin(115200);
  delay(500);

  // init display
  display.init();
  display.eraseDisplay();
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeMono9pt7b);

  display.setRotation(1);
  display.fillScreen(GxEPD_WHITE);
  display.update();

  x = 0;
  y = 50;
  display.setTextSize(2);
  String str = "Hello,";
  display.setCursor(x, y);
  display.getTextBounds(str, x, y, &x1, &y1, &w, &h);
  display.setCursor(display.width() / 2 - ((w + x1) / 2), display.height() / 2 - ((h + y1) / 2));
  display.println(str);

  y = display.getCursorY() + 1;
  display.setCursor(0, y);
  display.println("I am Chris!");

  display.update();
}

void loop() {
  delay(1000);
}
