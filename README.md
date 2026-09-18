# Overview
**MouldKingino** is an Arduino library for controlling Mould King Power Modules with ESP32 boards. 

# Supported hardware and Bluetooth libraries
Any ESP32 board can be used: chips both with Bluetooth LE (ESP32-S3) and Bluetooth classic (ESP-WROOM-32) are supported.

[NimBLE-Arduino](https://github.com/h2zero/NimBLE-Arduino) library is recommended as a quick start for simple Arduino sketches.

[BTstack](https://github.com/bluekitchen/btstack) is also supported for more advanced use cases (like [M5Bluepad](https://github.com/pink0D/M5Bluepad/))

# Supported Mould King modules

MouldKingino supports power modules controlled with simple Bluetooth LE advertising.
Only MK-branded modules are supported - other clones use different protocols not yet implemented for Arduino.

| Module | View | Status | Description |
| :--- | :--- | :---- |  :---- |
| **Mould King 4.0 Power Module** <br/> <img width="50" alt="MK logo" src="https://github.com/user-attachments/assets/7d436cb4-5eb3-4f68-9fc1-404786991aca" /> | <img width="150" alt="mk40" src="https://github.com/user-attachments/assets/c3ec0708-9cdb-4308-a025-21dcd572bf07" /> | :white_check_mark: Supported | 4x Power Function channels, up to 3x modules can be controlled by a single ESP32 board |
| **Mould King 6.0 Power Module** <br/> <img width="50" alt="MK logo" src="https://github.com/user-attachments/assets/7d436cb4-5eb3-4f68-9fc1-404786991aca" /> | <img width="150" alt="mk60" src="https://github.com/user-attachments/assets/b88d586e-67f1-4b0c-a7f2-5ec4de91fed2" /> | :white_check_mark: Supported | 6x Power Function channels |
| **Mould King 3.0 Vehicle Module** <br/> <img width="50" alt="MK logo" src="https://github.com/user-attachments/assets/7d436cb4-5eb3-4f68-9fc1-404786991aca" /> | <img width="150" alt="mk30" src="https://github.com/user-attachments/assets/b88d586e-67f1-4b0c-a7f2-5ec4de91fed2" /> | :white_check_mark: Supported | 2x built-in motors: CH1 (MOTOR_A) steering, CH2 (MOTOR_B). CH3 and CH4 present in protocol but ignored by module |

# License & credits
- MouldKingino is open source and licensed under the MIT License
- Mould King BLE protocol was originally implemented by **[@J0EK3R](https://github.com/J0EK3R)** and **[@vicocz](https://github.com/vicocz)** in [brickcontroller2](https://github.com/vicocz/brickcontroller2)
- MK 3.0 protocol added by **[@matzem79](https://github.com/matzem79)**

# Contacts
Issues: [MouldKingino issues](https://github.com/pink0D/MouldKingino/issues)

Discussions: [MouldKingino discussions](https://github.com/pink0D/MouldKingino/discussions)
