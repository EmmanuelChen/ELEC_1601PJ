/*
 * Group 6 Monday 12
 * 
 * BT_MASTER
 * Use Bluetooth unit to communicates between arduino
 * to robot car with IR receiver. 
 * 
 * We decide to change to a different Bluetooth as the
 * previous one has connection problems, and has a unstable
 * connection. 
 * 
 * The master Bluetooth will send corresponding signal 
 * to slave Bluetooth when receiving the IR signal from 
 * the remote control.
 * 
 * Master Bluetooth also receives signal from the slave 
 * to turn on or off the LED light accordingly
 * 
 * The circuit:
 *   Input: IR receiver, HC05-Bluetooth 
 *   Output :  LED light, HC05-Bluetooth
 * 
 * Version V2.0
 * Date Oct 22 2018
 * Author Group 6
 */

#include<IRremote.h>

const long W = 0xFD807F;
const long S = 0xFD906F;
const long D = 0xFD609F;
const long A = 0xFD20DF;
const long P = 0xFDA05F;
const long button1 = 0xFD08F7;
const long button0 = 0xFD30CF;

//set state to any letter as long as it 
//is not equal to 'Y', we will receive 'Y'
//from slave at certain temperature change
//and 'N' if temperature has dropped after 
//rising
char state = 'S';

//define pins for led and IR receiver
const int led_pin = 10;
const int RECV_PIN = 3;
IRrecv irrecv(RECV_PIN);
decode_results results;

//set sentTo to any letter as long as
//it is not equal to number 0 to 6
char sentTo = 'N';

void setup() {
  irrecv.enableIRIn();
  pinMode(led_pin, OUTPUT);
  Serial.begin(9600); //Default communication rate of the Bluetooth module
                      //rate may vary depend on model and manufacture
}

void loop() {
  //sent signal to the slave only when 
  //we have received signal from remote
  if(irrecv.decode(&results)){
            //Serial.println(results.value, HEX);
            irrecv.resume();
            
            //make signal to signal character so
            //it's easy to decode by slave
            switch(results.value){
              case W : sentTo = '2';        break;
              case S : sentTo = '3';        break;
              case A : sentTo = '4';        break;
              case D : sentTo = '5';        break;
              case P : sentTo = '6';        break;
              case button0 : sentTo = '0';  break;
              case button1 : sentTo = '1';  break;
            }
            //send signal to slave
            Serial.write(sentTo);         
            Serial.flush();
             
     //need a delay otherwise it will sent char constantly and mess
     //up the received value by slave;
            delay(50);          
      
  }  
  
 // if received signal from slave turn on or off 
 //the led according to the char we received
 if(Serial.available() > 0){ // Checks whether data is comming from the serial port
   state = Serial.read(); // Reads the data from the serial port
   if(state == 'Y'){
      digitalWrite(led_pin, HIGH);
      delay(500);
    }else{
      digitalWrite(led_pin, LOW);
    }
    
 } 
}
