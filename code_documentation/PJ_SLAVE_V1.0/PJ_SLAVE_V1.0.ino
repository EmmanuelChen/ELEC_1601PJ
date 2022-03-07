/*
 *Group 6 Monday 12 
 *
 * BT_SLAVE
 * Use Bluetooth unit to communicates between arduino
 * to robot car with IR receiver. 
 * Use remote to control the robot with specific 
 * signal, and turn on the different colour for
 * different signal
 *
 * The robot will receives signal when remote button
 * pressed, it will then execute the function accroding
 * to the signal that user sent.
 *
 * The circuit:
 *  Inpute:  Bluetooth
 *  Outpute: RGB LED, Bluetooth
 *  
 * Functions: 
 *    void setupBlueToothConnection():
 *        setup Bluetooth connection between maset
 *        and slave with communication rate of 
 *        38400
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
 * Date 8 OCT 2018
 * Author Group 6 and Template from Canvas UNiversity of Sydney
 */


#include <SoftwareSerial.h>   //Software Serial Port
#include<Servo.h>

Servo servoLeft;
Servo servoRight;

#define RxD 7

#define TxD 6

#define DEBUG_ENABLED  1



SoftwareSerial blueToothSerial(RxD,TxD);

void setup()

{

    Serial.begin(9600);

    servoLeft.attach(13);
    servoRight.attach(12);
   
    pinMode(RxD, INPUT);

    pinMode(TxD, OUTPUT);

    setupBlueToothConnection();



}



void loop()

{

    char recvChar;

    while(1)

    {

        if(blueToothSerial.available())

        {//check if there's any data sent from the remote bluetooth shield

            recvChar = blueToothSerial.read();

            Serial.print(recvChar);
            //check recvChar to execute command
             switch(recvChar){
                case 0xFD807F: backward(); break;
                case 0xFD906F: forward() ;  break;
                case 0xFD609F: right() ; break;
                case 0xFD20DF: left() ; break;
                case 0xFDA05F: pause(); break;
             } 
            
  
        }

        if(Serial.available())

        {//check if there's any data sent from the local serial terminal, you can add the other applications here

            recvChar  = Serial.read();

            blueToothSerial.print(recvChar); 

        }

    }

}
  


void setupBlueToothConnection()
{
    blueToothSerial.begin(38400);                           // Set BluetoothBee BaudRate to default baud rate 38400
    blueToothSerial.print("\r\n+STWMOD=0\r\n");             // set the bluetooth work in slave mode
    blueToothSerial.print("\r\n+STNA=Slave8+\r\n");    // set the bluetooth name as "SeeedBTSlave"
    blueToothSerial.print("\r\n+STOAUT=1\r\n");             // Permit Paired device to connect me
    blueToothSerial.print("\r\n+STAUTO=0\r\n");             // Auto-connection should be forbidden here
    delay(2000);                                            // This delay is required.
    blueToothSerial.print("\r\n+INQ=1\r\n");                // make the slave bluetooth inquirable
    Serial.println("The slave bluetooth is inquirable!");
    delay(2000);                                            // This delay is required.

    blueToothSerial.flush();

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
