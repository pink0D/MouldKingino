# Overview
**MouldKingino** is an Arduino library for controlling Mould King Power Modules with ESP32 boards. 

# Supported hardware and Bluetooth libraries
Any ESP32 board can be used: chips both with Bluetooth LE (ESP32-S3) and Bluetooth classic (ESP-WROOM-32) are supported.

[NimBLE-Arduino](https://github.com/h2zero/NimBLE-Arduino) library is recommended as a quick start for simple Arduino sketches.

[BTstack](https://github.com/bluekitchen/btstack) is also supported for more advanced use cases (like [M5Bluepad](https://github.com/pink0D/M5Bluepad/))

MouldKingino supports Mould King MK 4.0 (4-channel), MK 6.0 (6-channel), and the newer High Performance Power Module described below. These modules use BLE advertising packets for communication. Other clones may use different protocols and are not supported.

### High Performance Power Module

Support for the newer Mould King **High Performance Power Module** has been
tested with the module included in **Mould King set 15075 (Robot Dog)**.

The module uses a different command protocol from the MK 4.0 modules while
reusing the same BLE advertising and encryption mechanism.

Confirmed channel assignments for set 15075:

- Channel 1 / Motor A: steering (left / neutral / right)
- Channel 2 / Motor B: drive (forward / stop / reverse)
- Channels 3 and 4: function not yet identified

Drive and steering can be controlled simultaneously. The transmitter ID does
not need to be fixed; the existing MouldKingino transmitter ID generation can
be used.

Example:

    MouldKingHighPerformance car(MODULE_1, true);

    car.connect(2000);
    car.updateMotorOutput(MOTOR_B, 1.0);  // forward
    car.updateMotorOutput(MOTOR_A, 1.0);  // left

# License & credits
- MouldKingino is open source and licensed under the MIT License
- Mould King BLE protocol was originally implemented by **[@J0EK3R](https://github.com/J0EK3R)** and **[@vicocz](https://github.com/vicocz)** in [brickcontroller2](https://github.com/vicocz/brickcontroller2)

# Contacts
EMail: [pink0D.github@gmail.com](mailto:pink0d.github@gmail.com)
