/*
 *Group 6 Monday 12 
 *
 * IR remote controlwith RGB LED
 * Use remote to control the robot with specific 
 * signal, and turn on the different colour for
 * different signal
 *
 * The robot will receives signal when remote button
 * pressed, it will then execute the function accroding
 * to the signal that user sent.
 *
 * The circuit:
 *  Inpute: IR receiver
 *  Outpute: RGB LED
 * Functions: 
 *    void forward():    
 *        move the robot forward with green light
 *    void backward():    
 *        move the robot backward with white light
 *     void left():    
 *        turn the robot left with purple light
 *     void right():    
 *        turn the robot right with blue light
 *     void pause():    
 *        pause the robot with red light
 *        
 * Version V1.0;       
 * Date 2 OCT 2018
 * Author Group 6
 */


#include<IRremote.h>
#include<Servo.h>

Servo servoLeft;
Servo servoRight;

const int RECV_PIN = 6  ;
IRrecv irrecv(RECV_PIN);
decode_results results;

int redPin = 3;
int greenPin = 4;
int bluePin = 5;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  irrecv.enableIRIn();
  servoLeft.attach(13);  // Attach left signal to pin 13
  servoRight.attach(12); // Attach right signal to pin 12
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
}

void loop() {
  //test if receive decoding data and save it to variable results
if (irrecv.decode(&results)) {
// print data received in a hexadecimal
Serial.println(results.value, HEX);
irrecv.resume(); //wait for the next signal
}

//check which signal has received, and 
//execute the command accordingly
 switch(results.value){
    case 0xFD807F: backward(); break;
    case 0xFD906F: forward() ;  break;
    case 0xFD609F: right() ; break;
    case 0xFD20DF: left() ; break;
    case 0xFDA05F: pause(); break;
 } 
}

//1700 means counterclockwise
//1300 means clockwise
//1500 means the middle
//but since our servos are not 
//centred properly, we have
//1485 for left, and 1505 for right
//to stop


//moving forward with green light;
void forward(){
   servoLeft.writeMicroseconds(1700);  
   servoRight.writeMicroseconds(1300); 
   digitalWrite(greenPin,HIGH);
   digitalWrite(redPin,LOW);
   digitalWrite(bluePin,LOW);  
}

//moving backward with white light;
void backward(){
   servoLeft.writeMicroseconds(1300);  
   servoRight.writeMicroseconds(1700); 
   digitalWrite(greenPin,HIGH);
   digitalWrite(redPin,HIGH);
   digitalWrite(bluePin,HIGH);   
}

//moving left with purple light;
void left(){
   servoLeft.writeMicroseconds(1300);  
   servoRight.writeMicroseconds(1300); 
   digitalWrite(greenPin,LOW);
   digitalWrite(redPin,HIGH);
   digitalWrite(bluePin,HIGH);   
}

//moving right with blue light
void right(){
    servoLeft.writeMicroseconds(1700);  
   servoRight.writeMicroseconds(1700); 
   digitalWrite(greenPin,LOW);
   digitalWrite(redPin,LOW);
   digitalWrite(bluePin,HIGH); 
}

//pause with red light
void pause(){
    servoLeft.writeMicroseconds(1485);  
   servoRight.writeMicroseconds(1505);  
   digitalWrite(greenPin,LOW);
   digitalWrite(redPin,HIGH);
   digitalWrite(bluePin,LOW);  
}
