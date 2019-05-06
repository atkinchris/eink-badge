# E-Ink Badge for TTGO T5 v2.3

## Hardware

TTGO/LilyGo T5 v2.3. Purchased from Banggood, UK warehouse: [link](https://www.banggood.com/Wemos-T5-V2_0-TTGO-WiFi-Wireless-Module-bluetooth-Base-ESP-32-2_13-ePaper-Display-Development-Board-p-1332909.html). ESP32 with 2.13 inch e-paper display.

## Instructions

- Use VSCode, with PlatformIO extensions (will prompt for recommended extensions on opening folder).
- Install node dependencies with `npm install`.
- Build web project with `npm run build`.
- If first time, or web project updated, upload SPIFFS image, with "Platform IO Tasks -> PlatformIO: Upload File System Image"
- Flash code to ESP32 with PlatformIO
- Connect ESP32 to power/USB, and press the IO39 button once.
- Connect to advertised AP shown on screen, and visit [badge.local](http://badge.local)
- Follow instructions to choose and upload new image
