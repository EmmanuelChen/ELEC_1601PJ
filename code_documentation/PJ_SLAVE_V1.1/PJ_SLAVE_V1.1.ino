/*
 *Group 6 Monday 12 
 *
 * BT_SLAVE
 * Use Bluetooth unit to communicates between arduino
 * to robot car with IR receiver. 
 * Use remote to control the robot with specific 
 * signal, and turn on the different colour for
 * different signal,with the help of ultrasonic sensor
 * it can now detects object and avoid them when too close
 * we also added IR sensor for detecting edge.
 * Robot can now switch between auto mode and manual mode
 *
 * The robot will receives signal when remote button
 * pressed, it will then execute the function accroding
 * to the signal that user sent.
 *
 * The circuit:
 *  Input: IR receiver, Bluetooth, ultrasonic sensor, 
 *  Output: RGB LED, Bluetooth, ultrasonic sensor, 
 *  
 * Functions:   
 *    void setupBlueToothConnection():
 *        setup Bluetooth connection between maset
 *        and slave with communication rate of 
 *        38400
 *    void manual_mode():
 *        entre manual mode so that user can can fully control
 *        the robot with remote control 
 *    void auto_mode():    
 *        entre auto mode so that robot does everthing automatically
 *        user can not control the robot, excpet when the stop
 *        or manual mode button is press, both button will make
 *        robot entres manual mode
 *    int look_left():    
 *        robot will check the object distance on its left hand side
 *        and return the distance it found
 *    int look_right();    
 *        robot will check the object distance on its right hand side
 *        return the distance it found
 *    void sense_obj():    
 *        robot will check the distance of the object in front of it
 *        one the left and one the right and decide which way to go        
 *    void detect_edge():    
 *        robot will detect edge with IR and prevent it from falling
 *    void forward():    
 *        move the robot forward with green light
 *    void backward():    
 *        move the robot backward with white light
 *    void left():    
 *        turn the robot left with purple light
 *    void right():    
 *        turn the robot right with blue light
 *    void pause():    
 *        pause the robot with red light
 *        
 * Version V1.1;       
 * Date 15 OCT 2018
 * Author Group 6 and Template from Canvas UNiversity of Sydney
 */


#include <SoftwareSerial.h>   //Software Serial Port
#include<Servo.h>

Servo servoLeft;
Servo servoRight;
Servo myServo;

const int trig_pin = 9;
const int echo_pin = 10;
const int IR_R_IN = 8;
const int IR_R_OUT = 2;
const int IR_L_IN = 1;
const int IR_L_OUT = 0;
const int frequency = 38000;

long duration = 0;
int distance = 0;

int redPin = 3;
int greenPin = 4;
int bluePin = 5;

int isAuto = 0;
char recvChar;

#define RxD 7

#define TxD 6



#define DEBUG_ENABLED  1

SoftwareSerial blueToothSerial(RxD,TxD);



void setup()

{

    Serial.begin(9600);

    pinMode(RxD, INPUT);

    pinMode(TxD, OUTPUT);
    
    servoLeft.attach(13);
    servoRight.attach(12);
    myServo.attach(11);

    servoLeft.writeMicroseconds(1485);
    servoRight.writeMicroseconds(1505);
    myServo.write(90);
    delay(1000);                       //all servo stay still for 1 sec

    setupBlueToothConnection();



}


void loop()

{

    while(1)

    {
      
        if(blueToothSerial.available())

        {//check if there's any data sent from the remote bluetooth shield

            recvChar = blueToothSerial.read();
            
            Serial.print(recvChar);

        }

        if(Serial.available())

        {//check if there's any data sent from the local serial terminal, you can add the other applications here

            recvChar  = Serial.read();
            
            blueToothSerial.print(recvChar);

//            switch(recvChar){
//                case '0' : isAuto = 0; break;
//                case '1' : isAuto = 1; break;
//                case '2' : forward();  break;
//                case '3' : backward(); break;
//                case '4' : left();     break;
//                case '5' : right();    break;
//                case '6' : pause();    break;  
//            }
        }
        //check if we are going to
        //entre auto or manual mode
        if (isAuto){
           auto_mode();
        }else{
           manual_mode();
        }
    }

}
  
void setupBlueToothConnection()
{
    blueToothSerial.begin(38400);                           // Set BluetoothBee BaudRate to default baud rate 38400
    blueToothSerial.print("\r\n+STWMOD=0\r\n");             // set the bluetooth work in slave mode
    blueToothSerial.print("\r\n+STNA=Slave12+\r\n");    // set the bluetooth name as "SeeedBTSlave"
    blueToothSerial.print("\r\n+STOAUT=1\r\n");             // Permit Paired device to connect me
    blueToothSerial.print("\r\n+STAUTO=0\r\n");             // Auto-connection should be forbidden here
    delay(2000);                                            // This delay is required.
    blueToothSerial.print("\r\n+INQ=1\r\n");                // make the slave bluetooth inquirable
    Serial.println("The slave bluetooth is inquirable!");
    delay(2000);                                            // This delay is required.

    blueToothSerial.flush();

}

//manual mode is fully controlled by user,
//to entre manual mode first press button 0;
//to switch to auto mode, press button 1

void manual_mode(){
  
   //check received signal to execute proper
   //command if button1 pressed, entre auto mode
   switch(recvChar){
      case '1' : isAuto=1;   pause(); delay(800); break;
      case '2' :          forward();             break;
      case '3' :         backward();             break;
      case '4' :             left();             break;
      case '5' :            right();             break;
      case '6' :            pause();             break;  
    }
}


//auto mode is only controllable with button 0
//which is manual mode, and pause button.Once
//pause button or button 0 is pressed the robot 
//will goes  into manual mode.
//in auto mode if robot detects object, it will
//decide which way to go by it self
void auto_mode(){
  
   //check received signal to execute proper
   //command if button0 or pause pressed, entre 
   //manual mode
    switch(recvChar){
      case '0' : isAuto = 0; pause(); delay(800); break;
      case '6' :             pause();             break;
    }
    sense_obj();
    foward();
    
}

//    initally both are 0, means we detecting 'road' ahead
//    if both IR receiver goes zero{turn and check or go back and turn}
//    if right side IR receiver goes 1{turn left and check}  
//    if left side IR receiver goes 1{turn right and check}
void detectEdge(){

  tone(IR_L_OUT, frequency, 8);     // IRLED 38 kHz for at least 1 ms
  delay(1);                         // Wait 1 ms
  int Lir = digitalRead(IR_L_IN);   // IR receiver -> ir variable
  delay(1);                         // Down time before recheck
  tone(IR_R_OUT, frequency, 8);     
  delay(1);                         
  int Rir = digitalRead(IR_R_IN);   
  delay(1);

  if(Lir == 1 && Rir == 1){
      backward();
      delay(200);
      pause();
      sense_obj();  
  }else if(Lir == 1){
      right();
      delay(400);
      pause();
      sense_obj(); 
    
  }else if(Rir == 1){
      left();
      delay(400);
      pause();
      sense_obj();     
  }                        
                                                                   
}

//checking left side object dist
//with ultrasonic sensor
int look_left(){
      int dis = 0;
      long dur = 0;
      myServo.write(160);           //turn servo 160 degree to left
      delay(500);
      digitalWrite(trig_pin,LOW);   //make sure trig is clean 
      delayMicroseconds(2);
    
      digitalWrite(trig_pin,HIGH);  //output ultrasonic wave  
      delayMicroseconds(10);
      digitalWrite(trig_pin,LOW);   // close the output
    
      dur = pulseIn(echo_pin, HIGH);// measure the duration of the wave
      dis = dur*0.034/2;            //calculate the distance found
      return dis;
}

//checking right side object distance
//with ultrasonic sensor
int look_right(){
      int dis;
      long dur; 
      myServo.write(45);             //turn servo 45 degress to right
      delay(500);
      digitalWrite(trig_pin,LOW);    //make sure trig is clean 
      delayMicroseconds(2);
  
      digitalWrite(trig_pin,HIGH);   //output ultrasonic wave  
      delayMicroseconds(10);
      digitalWrite(trig_pin,LOW);    //close the output
    
      dur = pulseIn(echo_pin, HIGH); // measure the duration of the wave
      dis = dur*0.034/2;             //calculate the distance found
      return dis; 
}

//make ultrasonic sensor read  
//object distance
//if < 25 stop, look left and right
//then decide witch way to go
void sense_obj(){
    
    int L;
    int R;
    if (distance < 25){
        L = look_left();
        delay(1000);
        R = look_right();
        delay(1000);
        if(R > L){
             right();
             delay(500);
             forward();
        }else{
             left();
             delay(500);
             forward();         
        }
      
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
