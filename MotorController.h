#include <HardwareSerial.h>

class CytronQuadController {

  //VARIABLES
  HardwareSerial *driver1;
  HardwareSerial *driver2;
  byte m1_byte=0, m2_byte=128, m3_byte=0, m4_byte=128;

public:
  CytronQuadController(HardwareSerial& serial1, HardwareSerial& serial2)
    : driver1(&serial1), driver2(&serial2) {}

  void begin(uint baud_rate){
    driver1->begin(baud_rate);
    driver2->begin(baud_rate);
  }  
  
  void setSpeed(int motor, int speed){
    /*
      - The speed will range from -100 to 100.
      - The motor options are 1, 2, 3, 4
      Input example: setSpeed(2, -20).
      Explanation: Set motor 2 to speed 20 anticlockwise
    */

    // convert speed range from [-100, 100] to [-63 to +63]
    
    if(speed > 100) {
      speed = 100;
    }
    else if(speed < -100) {
      speed = -100;
    }

    byte vel=0;
    // set the direction bit (6th bit)
    if(speed<0){
      vel = vel | (1<<6); // making direction counter-clockwise
      speed *= -1;
    }
    
    vel = vel | (byte)(speed*63/100); // Converting the value 
    
    // select motor
    switch(motor){
      case 1:
        //vel = vel ^ (1<<6);  // uncomment to reverse the direction
        driver1->write(vel);
        break;
      case 2:
        //vel = vel ^ (1<<6);  // uncomment to reverse the direction
        vel = vel | (1<<7);  // selecting right motor
        driver1->write(vel);
        break;
      case 3:
        vel = vel ^ (1<<6);  // uncomment to reverse the direction
        vel = vel | (1<<7);
        driver2->write(vel);
        Serial.println(vel,BIN); 
        break;
      case 4:
        //vel = vel ^ (1<<6);  // uncomment to reverse the direction
        //vel = vel | (1<<7);  // selecting right motor
        driver2->write(vel);
        break;
      default:
        return;
    }

  }
  void stopAll(){
    for(int i=0; i<4; i++){
      setSpeed(i+1, 0);
    }
  }
};
