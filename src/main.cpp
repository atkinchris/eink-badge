#include <GxEPD.h>
#include <GxGDE0213B72/GxGDE0213B72.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>

#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>

#include "pinout.h"

#define FILESYSTEM SPIFFS

GxIO_Class io(SPI, ELINK_SS, ELINK_DC, ELINK_RESET);
GxEPD_Class display(io, ELINK_RESET, ELINK_BUSY);
AsyncWebServer server(80);
char message[32];

bool serverStarted = false;
int buttonState = 0;
int lastButtonState = 0;

void printTextToString(String text, const GFXfont *font = &FreeMono9pt7b)
{
  int16_t x1, y1;
  uint16_t w, h;

  display.setFont(font);
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  display.drawRect(x1, y1, w, h, GxEPD_WHITE);
  display.setCursor(display.width() / 2 - ((w + x1) / 2), display.height() / 2 - ((h + y1) / 2));
  display.println(text);
  display.update();
}

void startAP()
{
  String apName = "BadgeAP";
  IPAddress apIP = IPAddress(192, 168, 1, 1);

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

  if (!WiFi.softAP(apName.c_str(), NULL, 1, 0, 1))
  {
    Serial.println("AP Config failed.");
    printTextToString("AP Config failed.");
    return;
  }
  else
  {
    Serial.println("AP Config Success.");
    printTextToString("Connect to: " + apName);
  }
}

void startServer()
{
  if (!FILESYSTEM.begin())
  {
    Serial.println("FILESYSTEM is not database");
    Serial.println("Please use Arduino ESP32 Sketch data Upload files");
    while (1)
    {
      delay(1000);
    }
  }

  if (MDNS.begin("badge"))
  {
    Serial.println("MDNS responder started");
  }

  server.serveStatic("/", FILESYSTEM, "/").setDefaultFile("index.html");

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

  server.on("/pixels", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (
        !(
            request->hasParam("data", true) &&
            request->hasParam("width", true) &&
            request->hasParam("height", true) &&
            request->hasParam("x", true) &&
            request->hasParam("y", true)))
    {
      request->send(400, "text/plain", "Invalid params");
      return;
    }

    try
    {
      String data = request->getParam("data", true)->value();
      int width = request->getParam("width", true)->value().toInt();
      int height = request->getParam("height", true)->value().toInt();
      int x = request->getParam("x", true)->value().toInt();
      int y = request->getParam("y", true)->value().toInt();

      if (x * y > data.length())
      {
        request->send(400, "text/plain", "Data size mismatch");
        return;
      }

      for (size_t iY = 0; iY < height; iY++)
      {
        for (size_t iX = 0; iX < width; iX++)
        {
          uint16_t color = data.charAt((iY * width) + iX) == '1' ? GxEPD_BLACK : GxEPD_WHITE;
          display.drawPixel(x + iX, y + iY, color);
        }
      }

      display.update();

      request->send(200);
    }
    catch (...)
    {
      request->send(500);
    }
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

  pinMode(BUTTON_3, INPUT);

  // init display
  display.init();
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeMono9pt7b);
  display.setRotation(1);
  display.setTextSize(1);
}

void loop()
{
  buttonState = digitalRead(BUTTON_3);
  if (!serverStarted && (buttonState != lastButtonState) && buttonState == LOW)
  {
    startAP();
    startServer();
    serverStarted = true;
    delay(50);
  }
  lastButtonState = buttonState;
}
