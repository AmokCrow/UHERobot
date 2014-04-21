
#include "MotorControlLib.h"
#include "CommandReceiver.h"
#include <Wire.h>
#include <Servo.h>



char textBuffer[40];
unsigned long int lastCommandTime;

SeeedMC motors;
CommandReceiver receiver;
Servo panServo;
Servo tiltServo;


void setup()  {
  // Init communication peripherals
  Serial.begin(9600);
  Wire.begin();
  
  // Init the motor driver board
  delayMicroseconds(10000); // Wait a moment to be sure that the motor controller board got itself up
  motors.init(0x0f); // Init the local driver
  delay(100); // Wait a moment - this is from the example code, so no idea why we need to wait
  motors.MotorDirectionSet(0b0000); // This shorts both motor outputs, which roughly equals a handbrake. An old, rusty handbrake.
  
  // Init servos
  panServo.attach(9); // Pins 8 and 9 are the servos, but
  tiltServo.attach(8); // which one is pan and which tilt might change later.
  // Center the servos to begin with. It a small detail, but a big convenience when installing
  // the camera (or other type of sensor) if all the servos point straight ahead already.
  panServo.write(90);
  tiltServo.write(90);
  
  // The misnamed BASE16 decoder should have been initialized by the constructor, but I want to keep things uniform
  receiver.init();
  
  Serial.println("Started");
}

void loop()  {
  
  // If the last message was too long ago, stop moving.
  // It's not much of a safety, but it's a start.
  if((millis() - lastCommandTime) > 5000)
  {
    motors.MotorDirectionSet(0b0000);
    lastCommandTime = millis();
  }
  
  
}

void serialEvent() 
{
  char tmp;
  uint8_t const* data;
  
  while (Serial.available()) 
  {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    // add it to the inputString:
    tmp = receiver.addReceivedChar(inChar);
    
    if(tmp == CommandReceiver::MESSAGE_ERROR)
    {
      motors.MotorDirectionSet(0b0000);
    }
    else if(tmp == CommandReceiver::MESSAGE_READY)
    {
      lastCommandTime = millis();
      data = receiver.getBuffer();
      
      if((data[0] == SET_SERVOS) && (receiver.buffLength() == 3))
      {
        panServo.write(map(data[1], 0, 255, 0, 180));
        tiltServo.write(map(data[2], 0, 255, 0, 180));
      }
      else if((data[0] == SET_MOTORS) && (receiver.buffLength() == 3))
      {
        motors.MotorControlBySignedBytes((char)data[1], (char)data[2]);
      }
    }
    
 }
}

