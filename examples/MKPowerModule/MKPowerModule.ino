#include <NimBLEDevice.h>
#include <MouldKingino.h>

MouldKing40 mk;   // MK 4.0 module
//MouldKing60 mk; // MK 6.0 is also supported

/*

One ESP32 can control up to 3x MK 4.0 modules or one MK 6.0 module:

MouldKing40 mk1(MODULE_1);
MouldKing40 mk2(MODULE_2);
MouldKing40 mk3(MODULE_2);

module number can be also set for MK6.0:

MouldKing60 mk6(MODULE_1); // or MODULE_2 or MODULE_3

*/


void setup() {
  Serial.begin(115200);

  NimBLEDevice::init("");

  // this call just initializes the device
  mk.begin();   

  // starts sending the connection message with default duration for 1000ms
  mk.connect(); 
}

void loop() {

  // increase motor speed gradually
  double v=0;
  while (v <= 1.0) {

    mk.updateMotorOutput(CHANNEL_A, v);
    mk.updateMotorOutput(CHANNEL_B, v);
    mk.updateMotorOutput(CHANNEL_C, v);
    mk.updateMotorOutput(CHANNEL_D, v);
    // CHANNEL_E and CHANNEL_F are also available for MK 6.0

    mk.applyUpdates(); // actually sends the updates 
    
    delay(500);
    v += 0.125;
  }

  delay(2000);

  // reverse motor speed gradually
  v=0;
  while(v >= -1.0) {
    mk.updateMotorOutput(CHANNEL_A, v);
    mk.updateMotorOutput(CHANNEL_B, v);
    mk.updateMotorOutput(CHANNEL_C, v);
    mk.updateMotorOutput(CHANNEL_D, v);
    // CHANNEL_E and CHANNEL_F are also available for MK 6.0

    mk.applyUpdates(); // actually sends the updates 
    
    delay(500);
    v -= 0.125;
  }

  delay(2000);
}
