#include "MotorController.h"
#include <STM32Encoder.h>
#include "PIDController.h"
#include "Rate.h"
//#include "CytronQuadController.h"
#include "EncoderHandler.h"

// Motor 1
const int motorPinDirection = PA10;
const int motorPinSpeed = PB6;
const int encoderPinA_Motor = PA4;
const int encoderPinB_Motor = PA1;

// Motor 2
const int motorPinDirection2 = PB5;
const int motorPinSpeed2 = PB4;
const int encoderPinA_Motor2 = PC1;
const int encoderPinB_Motor2 = PB0;

// Motor 3
const int motorPinDirection3 = PA9;
const int motorPinSpeed3 = PC7;
const int encoderPinA_Motor3 = PB10;
const int encoderPinB_Motor3 = PA8;

// Motor 4
const int motorPinDirection4 = PA6;
const int motorPinSpeed4 = PA7;
const int encoderPinA_Motor4 = PA0; //A0
const int encoderPinB_Motor4 = PC0; //A5

CytronQuadController controller(13, 12); // Pin 13 denotes the signal pin of driver 1 and Pin 12 denotes the signal pin of driver 2
EncoderHandler e(encoderPinA_Motor, encoderPinB_Motor, encoderPinA_Motor2, encoderPinB_Motor2, encoderPinA_Motor3, encoderPinB_Motor3, encoderPinA_Motor4, encoderPinB_Motor4);

PIDController p[4];
Rate rate(10);
int baud_rate = 9600;
const int BUFFER_SIZE = 16;// setting the buffer size for receiving the values from Jetson
byte buf[BUFFER_SIZE];

float target_velocity[4] = {0, 0, 0, 0};
bool print_vel = false;

union {
  byte b[4];
  float fval;// Conversion of incoming bytes to velocity
} u;  

union {
  byte b[32];
  int64_t val[4];
} enc_val;// Conversion of wheel positions to bytes

void setup() {
  Serial.begin(baud_rate);
  controller.begin(baud_rate);

  pinMode(15, OUTPUT);  // indicator light
  digitalWrite(15, HIGH);
  
  p[0].setPID(0.001, 0.15, 0.0);
  p[1].setPID(0.001, 0.15, 0.0);
  p[2].setPID(0.001, 0.15, 0.0);
  p[3].setPID(0.001, 0.15, 0.0);
  
  Serial.setTimeout(0);
  delay(2000);  // delay for indicator light
  digitalWrite(15, LOW);
}

void loop() {
  // reading serial input
  int inp_len = Serial.readBytes(buf, BUFFER_SIZE);

  if (inp_len == BUFFER_SIZE) {
    for(int i=0; i<4; i++) {
      for(int j=0; j<4; j++) {
        u.b[j] = buf[i*4+j];
      }
      target_velocity[i] = u.fval;
    }
  }

  for(int i=0; i<4; i++) {
    // get current velocity from encoders
    float curr_vel = e.getVelocity(i+1);

    // get the PID controlled speed        
    float speed = p[i].compute(curr_vel, target_velocity[i]);
    controller.setSpeed(i+1, speed);   
  }

  if(print_vel) {
    /*
      Serial Plotter format:
        "velocity_1:0,velocity_2:0,velocity_3:0,velocity_4:0"    
    */    
    Serial.print("velocity_1:");
    Serial.print(e.getVelocity(1));
    Serial.print(",");
    Serial.print("velocity_2:");
    Serial.print(e.getVelocity(2));
    Serial.print(",");
    Serial.print("velocity_3:");
    Serial.print(e.getVelocity(3));
    Serial.print(",");
    Serial.print("velocity_4:");
    Serial.println(e.getVelocity(4));
  }
  
  // control the loop execution time
  rate.sleep();

  // obtain and send current encoder values
  for(int i=0; i<4; i++) {
    enc_val.val[i] = e.getCount(i+1);
  }
  Serial.write(enc_val.b, 32);
  Serial.println("");
  
  // update the velocities
  e.updateVelocities();
}