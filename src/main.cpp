#include <GxEPD.h>
#include <GxGDE0213B72/GxGDE0213B72.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>

#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>

#include "pinout.h"

#define WIFI_SSID ""
#define WIFI_PASSWORD ""

GxIO_Class io(SPI, ELINK_SS, ELINK_DC, ELINK_RESET);
GxEPD_Class display(io, ELINK_RESET, ELINK_BUSY);
AsyncWebServer server(80);
char message[32];

String connectToWifi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.print(".");
    esp_restart();
  }

  Serial.println(F("WiFi connected"));
  Serial.println(WiFi.localIP());
  return WiFi.localIP().toString();
}

void printTextToString(String text, const GFXfont *font)
{
  int16_t x1, y1;
  uint16_t w, h;

  display.setFont(font);
  display.fillScreen(GxEPD_WHITE);
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  display.setCursor(display.width() / 2 - ((w + x1) / 2), display.height() / 2 - ((h + y1) / 2));
  display.println(text);
  display.update();
}

void startServer()
{
  if (MDNS.begin("ttgo"))
  {
    Serial.println("MDNS responder started");
  }

  server.on("/message", HTTP_POST, [](AsyncWebServerRequest *request) {
    for (int i = 0; i < request->params(); i++)
    {
      String key = request->getParam(i)->name();
      String value = request->getParam(i)->value();
      Serial.println(key + " : " + value);

      if (key == "message")
      {
        strlcpy(message, value.c_str(), sizeof(message));
      }
      else if (key == "fontSize")
      {
        display.setTextSize(value.toInt());
      }
    }

    printTextToString(message, &FreeSansBold9pt7b);
    request->send(200, "text/plain", message);
  });

  server.onNotFound([](AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
  });

  MDNS.addService("http", "tcp", 80);

  server.begin();
}

void setup()
{
  Serial.begin(115200);
  delay(500);

  // init display
  display.init();
  display.eraseDisplay();
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeMono9pt7b);
  display.setRotation(1);
  display.setTextSize(1);

  String ipAddress = connectToWifi();
  startServer();

  printTextToString(ipAddress, &FreeMono9pt7b);
}

void loop()
{
  delay(1000);
}
