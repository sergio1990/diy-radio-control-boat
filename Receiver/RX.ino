#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

#define leftMotorPinA 7
#define leftMotorPinB 6
#define rightMotorPinA 5
#define rightMotorPinB 4

RF24 radio(9, 8);

byte address[][6] = {"1Node","2Node","3Node","4Node","5Node","6Node"};

const int leftMotorPins[2] = {
  leftMotorPinA,
  leftMotorPinB
};
const int rightMotorPins[2] = {
  rightMotorPinA,
  rightMotorPinB
};

// 0 - stop
// 1 - forward
// 2 - backward
struct ControlPackage {
  byte leftMotor = 0;
  byte rightMotor = 0;
};

const byte controlToPinValues[3][2] = {
  {0, 0},
  {0, 1},
  {1, 0}
};

ControlPackage package;

void setup(){
  Serial.begin(9600);

  pinMode(leftMotorPinA, OUTPUT);
  pinMode(leftMotorPinB, OUTPUT);
  pinMode(rightMotorPinA, OUTPUT);
  pinMode(rightMotorPinB, OUTPUT);

  radio.begin();
  radio.setAutoAck(1);
  radio.setRetries(0, 15);
  radio.enableAckPayload();
  radio.setPayloadSize(2);

  radio.openReadingPipe(1, address[0]);
  radio.setChannel(0x60);

  radio.setPALevel (RF24_PA_MAX);
  radio.setDataRate (RF24_250KBPS);
  
  radio.powerUp();
  radio.startListening();
}

void loop() {
  byte pipeNumber;
  while (radio.available(&pipeNumber)) {
    if(pipeNumber != 1) {
      return;
    }
    Serial.print("Received a package on pipe: ");
    Serial.println(pipeNumber);
    Serial.println("Reading new package...");
    radio.read(&package, sizeof(ControlPackage));
    Serial.println("Received control package:");
    Serial.print("leftMotor: ");
    Serial.println(package.leftMotor);
    Serial.print("rightMotor: ");
    Serial.println(package.rightMotor);
    Serial.println("=============================");
    
    set_motor_control(leftMotorPins, package.leftMotor);
    set_motor_control(rightMotorPins, package.rightMotor);
  }
}


void set_motor_control(int pins[], byte control) {
  byte pinValues[2] = {0, 0};
  memcpy(pinValues, controlToPinValues[control], sizeof(controlToPinValues[control]));
  for(int i = 0; i <= 1; i++) {
    byte value = pinValues[i];
    digitalWrite(pins[i], value);
  }
}



