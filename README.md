# Overview
**MouldKingino** is an Arduino library for controlling Mould King Power Modules with ESP32 boards. 

# Supported hardware and Bluetooth libraries
Any ESP32 board can be used: chips both with Bluetooth LE (ESP32-S3) and Bluetooth classic (ESP-WROOM-32) are supported.

[NimBLE-Arduino](https://github.com/h2zero/NimBLE-Arduino) library is recommended as a quick start for simple Arduino sketches.

[BTstack](https://github.com/bluekitchen/btstack) is also supported for more advanced use cases (like [M5Bluepad](https://github.com/pink0D/M5Bluepad/))

Only Mould King MK 4.0 (4-channel) and MK 6.0 (6-channel) can be controlled - these have Power Functions connectors and use BLE advertising packets for communication. Other clones use different protocols and are not supported.

# License & credits
- MouldKingino is open source and licensed under the MIT License
- Mould King BLE protocol was originally implemented by **[@J0EK3R](https://github.com/J0EK3R)** and **[@vicocz](https://github.com/vicocz)** in [brickcontroller2](https://github.com/vicocz/brickcontroller2)

# Contacts
EMail: [pink0D.github@gmail.com](mailto:pink0d.github@gmail.com)
