#include <MouldKingino.h>

MouldKing40 mk;   // MK 4.0 module
//MouldKing60 mk; // MK 6.0 is also supported

void setup() {
  Serial.begin(115200);

  NimBLEDevice::init("");

  // this call just initializes the device
  mk.begin();   

  // starts sending the connection message with default duration for 1000ms
  mk.connect(); 
}

void loop() {

  // increase motor speed gradualy
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

  // reverse motor speed gradualy
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
