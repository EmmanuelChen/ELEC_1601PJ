#include<Servo.h>
#include<IRremote.h>

Servo servoLeft;
Servo servoRight;
Servo myServo;

//front angle for myServo
#define MIDDLE 105 

char recvChar;

//define pins for temp sensor, ultrasonic
//sensor and RGB LED

const int temp_pin  = A0;
const int trig_pin  = 8;
const int echo_pin  = 9;
const int red_pin   = 3;
const int green_pin = 4;
const int blue_pin  = 5;

int distance  = 0;

//set isAuto to 2 initially not 0, or 1
//user have to press button in order to 
//entre auto or manual mode 
int isAuto    = 2; 

float init_temp;
float current_temp;
float temp_temp; 

//signals that can be sent back 
//to master to open the LED, or 
//close the LED
char send_back = 'N';

void setup() {
  Serial.begin(9600);
  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
  pinMode(temp_pin, INPUT);
  
  init_temp = sense_temp();
  
  servoLeft.attach(13);
  servoRight.attach(12);
  myServo.attach(11);
  
}

void loop() {
  
  //wait 100ms for master signal to arrive
  delay(100);
  //if no signal arrive and we are in manual mode
  //stop the car, and set recvChar to 'F' or any
  //other letters as long as it does not match number
  //0 to 6;
  
  if(Serial.available() < 1 && isAuto == 0){
      pause();
      recvChar = 'F';
    
  }
  else if(Serial.available())
        {//check if there's any data sent from the local serial terminal, you can add the other applications here

            recvChar  = Serial.read();
            Serial.println(recvChar);

           
          switch(recvChar){
                case '0' : isAuto = 0; break;
                case '1' : isAuto = 1; break;
          }
          
        }
        
        distance = sense_obj();
        temp_temp = sense_temp();

        //send 'Y' signal to master when
        //temp has rise over 5 degress
        //send 'N' signal to master when
        //temp drops after rising
        if (temp_temp - init_temp > 5){
            current_temp = temp_temp; 
            send_back = 'Y';
            Serial.write(send_back);
        }

        if(current_temp - temp_temp > 0){
            send_back = 'N';  
            Serial.write(send_back);
        }

        //check isAuto in order to entre
        //then auto, manual mode     
        if (isAuto == 1){
           auto_mode();
        }else if(isAuto == 0){
           manual_mode();
        }else{
           pause();  
        }
}


//manual mode is fully controlled by user,
//to entre manual mode first press button 0;
//user needs to press and hold the command 
//button to make the robot continously moving, 
//once release it stops.
//to switch to auto mode, press button 1

void manual_mode(){
      int L;
      int R;
      //when distance is below 25, robot will turn
      //to the farest distance availiable and stop,
      //unlike auto mode, user have to decide to go
      //forward or backward or turning
         if(distance < 25){
              backward();
              delay(450);
              pause();
              delay(1000);
              
              R = look_right();
              delay(1500);
              myServo.write(MIDDLE);
              
              delay(1000);
              L = look_left();
              delay(1500);
              myServo.write(MIDDLE);
              
                 if(L > R){
                      left();
                      delay(450);
                      pause(); 
                  }else if (R > L){
                      right();
                      delay(450);
                      pause();
                  }else{
                       backward();
                       delay(500);
                       pause();
                  }
          }
   //check received signal to execute proper
   //command if button1 pressed, entre auto mode
   switch(recvChar){
      case '1' : isAuto=1;           
                 pause();  delay(1000);
                 
      case '2' :          forward();             break;
      case '3' :         backward();             break;
      case '4' :             left();             break;
      case '5' :            right();             break;
      case '6' :            pause();             break;  
    }
}

//auto mode is only controllable with button 0
//which is manual mode, and pause button.Once
//pause button is pressed the robot will goes 
//into manual mode.
//in auto mode if robot detects object, it will
//decide which way to go by it self
//to switch to manual mode press button 0 or 
//pause button;

void auto_mode(){
      
         switch(recvChar){
            case '0' : isAuto = 0; pause(); delay(800); break;
            case '6' : isAuto = 0; break;
         }

      int L;
      int R;   
      //when distance of an object is too close
      //stop and check left right, then decide
      //which way to go
          if(distance < 25){
              backward();  
              delay(450);
              pause();
              delay(1000);
              R = look_right();
              delay(1500);
              myServo.write(MIDDLE);
              
              delay(1000);
              L = look_left();
              delay(1500);
              myServo.write(MIDDLE);
              
                 if(L > R){
                      left();
                      delay(450); 
                  }else if (R > L){
                      right();
                      delay(450);
                  }else{
                       backward();  
                       delay(1000);
                       pause();
                       delay(1000);
                       left();
                       delay(450);
                  }
            }
          forward(); 
}

//checking left side object dist
int look_left(){
      int dis = 0;
      long dur = 0;
      myServo.write(160);
      delay(500);
      digitalWrite(trig_pin,LOW); //make sure trig is clean 
      delayMicroseconds(2);
    
      digitalWrite(trig_pin,HIGH);  
      delayMicroseconds(10);
      digitalWrite(trig_pin,LOW);
    
      dur = pulseIn(echo_pin, HIGH);
      dis = dur*0.034/2;
      return dis;
}

//checking right side object dist
int look_right(){
      int dis;
      long dur; 
      myServo.write(45);
      delay(500);
      digitalWrite(trig_pin,LOW); //make sure trig is clean 
      delayMicroseconds(2);
  
      digitalWrite(trig_pin,HIGH);  
      delayMicroseconds(10);
      digitalWrite(trig_pin,LOW);
    
      dur = pulseIn(echo_pin, HIGH);
      dis = dur*0.034/2;
      return dis; 
}

//checking front object dist
int sense_obj(){
   int dis;
   long dur; 
   digitalWrite(trig_pin,LOW); //make sure trig is clean 
   delayMicroseconds(2);

   digitalWrite(trig_pin,HIGH);  
   delayMicroseconds(10);
   digitalWrite(trig_pin,LOW);

   dur = pulseIn(echo_pin, HIGH);
   dis = dur*0.034/2;
   return dis; 
}

//checking temperature
float sense_temp(){
  int recv = analogRead(temp_pin);
  float V = (double)recv/1024*5000;
  float celsius = V/10;
  return celsius;
}

//moving forward with green light;
void forward(){
   servoLeft.writeMicroseconds(1700);
   servoRight.writeMicroseconds(1300); 
   digitalWrite(green_pin,HIGH);
   digitalWrite(red_pin,LOW);
   digitalWrite(blue_pin,LOW);
}

//moving backward with white light;
void backward(){
   servoLeft.writeMicroseconds(1300);  
   servoRight.writeMicroseconds(1700);   
   digitalWrite(green_pin,HIGH);
   digitalWrite(red_pin,HIGH);
   digitalWrite(blue_pin,HIGH);
}

//moving left with purple light;
void left(){
   servoLeft.writeMicroseconds(1300);  
   servoRight.writeMicroseconds(1300); 
   digitalWrite(green_pin,LOW);
   digitalWrite(red_pin,HIGH);
   digitalWrite(blue_pin,HIGH); 
}

//moving right with blue light
void right(){
   servoLeft.writeMicroseconds(1700);  
   servoRight.writeMicroseconds(1700); 
   digitalWrite(green_pin,LOW);
   digitalWrite(red_pin,LOW);
   digitalWrite(blue_pin,HIGH); 
}

//pause with red light
void pause(){
   servoLeft.writeMicroseconds(1500);
   servoRight.writeMicroseconds(1500);
   digitalWrite(green_pin,LOW);
   digitalWrite(red_pin,HIGH);
   digitalWrite(blue_pin,LOW);  
}
