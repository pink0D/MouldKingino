#include <NimBLEDevice.h>
#include <MouldKingino.h>

// MK 3.0 vehicle module. Tested with the module included in Mould King set 15075 (Robot Dog)
// immediateUpdate=true sends each changed channel state immediately.
MouldKing30 car(MODULE_1, true);

void setup() {
  Serial.begin(115200);
  NimBLEDevice::init("");

  Serial.println("Mould King 3.0 test");
  Serial.println("Switch module on; connecting in 5 seconds...");
  delay(5000);

  // No fixed TxId is required. MKBLEAdvertiser derives one from the ESP32
  // Bluetooth MAC and uses it consistently for CONNECT and UPDATE packets.
  car.connect(2000);

  Serial.println("Commands:");
  Serial.println("v=forward r=reverse s=drive stop");
  Serial.println("l=left e=right g=straight x=all neutral");
}

void loop() {
  if (!Serial.available()) return;

  char c = Serial.read();

  switch (c) {
    case 'v': car.updateMotorOutput(MOTOR_B,  1.0f); break;
    case 'r': car.updateMotorOutput(MOTOR_B, -1.0f); break;
    case 's': car.updateMotorOutput(MOTOR_B,  0.0f); break;

    case 'l': car.updateMotorOutput(MOTOR_A,  1.0f); break;
    case 'e': car.updateMotorOutput(MOTOR_A, -1.0f); break;
    case 'g': car.updateMotorOutput(MOTOR_A,  0.0f); break;

    case 'x': car.resetChannels(); break;
  }
}
