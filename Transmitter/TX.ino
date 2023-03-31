#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

#define vAxisPin A0
#define hAxisPin A1

RF24 radio(9, 10);

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"};

byte counter;

struct ControlPackage {
  byte leftMotor = 0;
  byte rightMotor = 0;
};

ControlPackage package;

void setup() {
  Serial.begin(9600);

  pinMode(vAxisPin, INPUT);
  pinMode(hAxisPin, INPUT);

  radio.begin();
  radio.setAutoAck(1);
  radio.setRetries(0, 15);
  radio.enableAckPayload();
  radio.setPayloadSize(2);

  radio.openWritingPipe(address[0]);
  radio.setChannel(0x60);
 
  radio.setPALevel (RF24_PA_MAX);
  radio.setDataRate (RF24_250KBPS);

  radio.powerUp();
  radio.stopListening();
}
void loop() {
  //               v axis
  //      (v, h)     |
  //      (1, 1)     |     (1, -1)
  //                 |
  //                 |
  // h axis -------------------------
  //                 |
  //                 |
  //      (-1, 1)    |     (-1, -1)
  //                 |
  int vAxisValue = normalizeJoystickValue(analogRead(vAxisPin));
  int hAxisValue = normalizeJoystickValue(analogRead(hAxisPin));
  byte leftMotorControl = 0;
  byte rightMotorControl = 0;
  if(vAxisValue == 1) {
    leftMotorControl = 1;
    rightMotorControl = 1;
  } else if (vAxisValue == -1) {
    leftMotorControl = 2;
    rightMotorControl = 2;
  } else if (hAxisValue == 1) {
    leftMotorControl = 2;
    rightMotorControl = 1;
  } else if (hAxisValue == -1) {
    leftMotorControl = 1;
    rightMotorControl = 2;
  }
  if (package.leftMotor != leftMotorControl || package.rightMotor != rightMotorControl) {
    Serial.println("Sending new control package:");
    Serial.print("left motor -> ");
    Serial.println(leftMotorControl);
    Serial.print("right motor -> ");
    Serial.println(rightMotorControl);
    package.leftMotor = leftMotorControl;
    package.rightMotor = rightMotorControl;
    bool sentResult = radio.write(&package, sizeof(ControlPackage));
    if (sentResult) {
        Serial.println("  Acknowledge received");
    }
    else {
        Serial.println("  Tx failed");
    }
  }
}

int normalizeJoystickValue(int rawValue) {
  if(rawValue >= 450 && rawValue <= 550) {
    return 0;
  }
  if(rawValue < 450) {
    return 1;
  }
  if(rawValue > 550) {
    return -1;
  }
}
