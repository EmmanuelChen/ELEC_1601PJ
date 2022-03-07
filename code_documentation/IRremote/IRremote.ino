/*
 *Group 6 Monday 12 
 *
 * IR remote control
 * Use remote to control the robot with specific 
 * signal
 *
 * The robot will receives signal when remote button
 * pressed, it will then execute the function accroding
 * to the signal that user sent.
 *
 * The circuit:
 *  Inpute: IR receiver
 *  Outpute: N/A
 * Functions: 
 *    void forward():    
 *        move the robot forward 
 *    void backward():    
 *        move the robot backward 
 *     void left():    
 *        turn the robot left 
 *     void right():    
 *        turn the robot right
 *     void pause():    
 *        pause the robot 
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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  irrecv.enableIRIn();
  servoLeft.attach(13);  // Attach left signal to pin 13
  servoRight.attach(12); // Attach right signal to pin 12
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

//move forward;
void forward(){
   servoLeft.writeMicroseconds(1700);  
   servoRight.writeMicroseconds(1300); 
}

//move backward;
void backward(){
   servoLeft.writeMicroseconds(1300);  
   servoRight.writeMicroseconds(1700);   
}

//turn left
void left(){
   servoLeft.writeMicroseconds(1300);  
   servoRight.writeMicroseconds(1300);   
}

//turn right
void right(){
    servoLeft.writeMicroseconds(1700); 
   servoRight.writeMicroseconds(1700); 
}

//pause robot 
void pause(){
    servoLeft.writeMicroseconds(1485); 
   servoRight.writeMicroseconds(1505);   
}
