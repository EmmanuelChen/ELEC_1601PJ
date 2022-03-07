/*
 * Group 6 Monday 12
 * 
 * BT_MASTER
 * Use Bluetooth unit to communicates between arduino
 * to robot car with IR receiver. 
 * 
 * The master Bluetooth will send corresponding signal 
 * to slave Bluetooth when receiving the IR signal from 
 * the remote control.
 * 
 * Master Bluetooth also receives signal from the slave 
 * when type in from the serial monitor
 * 
 * The circuit:
 *   Input: IR receiver, Bluetooth 
 *   Output : Bluetooth
 * 
 * Function:
 *   void setupBlueToothConnection():
 *      connection between master and slave with
 *      communication rate of 38400
 *   
 * Version V1.1
 * Date Oct 15 2018
 * Author Group 6 and Template from canvas University of Sydney
 */

#include <SoftwareSerial.h>                     // Software Serial Port
#define RxD 7
#define TxD 6
#define DEBUG_ENABLED  1

#include<IRremote.h>

const int RECV_PIN = 3;
IRrecv irrecv(RECV_PIN);
decode_results results;

String retSymb = "+RTINQ=";                     // start symble when there's any return
String slaveName = ";Slave12";             // caution that ';'must be included, and make sure the slave name is right.
int nameIndex = 0;
int addrIndex = 0;

String recvBuf;                           
String slaveAddr;
String connectCmd = "\r\n+CONN=";
SoftwareSerial blueToothSerial(RxD,TxD);



const long W = 0xFD807F;
const long S = 0xFD906F;
const long D = 0xFD609F;
const long A = 0xFD20DF;
const long P = 0xFDA05F;
const long button1 = 0xFD08F7;
const long button0 = 0xFD30CF;

char sentTo = 'N';

void setup()

{

    Serial.begin(9600);
               
    pinMode(RxD, INPUT);

    pinMode(TxD, OUTPUT);

    setupBlueToothConnection();

     irrecv.enableIRIn();

    //wait 1s and flush the serial buffer

    delay(1000);

    Serial.flush();

    blueToothSerial.flush();

}



void loop()

{
    
    char recvChar;

    while(1)

    {
            if(irrecv.decode(&results)){
               Serial.println(results.value, HEX);
               irrecv.resume();
            }
            //make sentTo to values 0 to 6
            //make signal to signal character so
            //it's easy to decode by slave
            switch(results.value){
              case W : sentTo = '2';  break;
              case S : sentTo = '3';  break;
              case A : sentTo = '4';  break;
              case D : sentTo = '5';  break;     
              case P : sentTo = '6';  break;
              case button0 : sentTo = '0'; break;
              case button1 : sentTo = '1'; break;
            } 
        //send signal to slave once sentTo is not
        //default value
        if(sentTo != 'N'){
            Serial.println(sentTo);
            Serial.write(sentTo);
            delay(600);  //need a delay otherwise it will sent char constantly and mess
                         //up the received value by slave; 
         }
  
    
        if(blueToothSerial.available())         //check if there's any data sent from the remote bluetooth shield

        {

            recvChar = blueToothSerial.read();
            Serial.print(recvChar);

        }

        if(Serial.available())                  //check if there's any data sent from the local serial terminal, you can add the other applications here

        {

            recvChar  = Serial.read();
            blueToothSerial.print(results.value, HEX);
            blueToothSerial.print(recvChar);
          
        }

    }

}



void setupBlueToothConnection()
{
    blueToothSerial.begin(38400);                               // Set BluetoothBee BaudRate to default baud rate 38400
    blueToothSerial.print("\r\n+STWMOD=1\r\n");                 // set the bluetooth work in master mode
    
    blueToothSerial.print("\r\n+STNA=Master12\r\n");       // set the bluetooth name as
        
    blueToothSerial.print("\r\n+STAUTO=0\r\n");                 // Auto-connection is forbidden here
    delay(2000);                                                // This delay is required.
    blueToothSerial.flush();
    blueToothSerial.print("\r\n+INQ=1\r\n");                    //make the master inquire
    Serial.println("Master is inquiring!");
    delay(2000); // This delay is required.



    //find the target slave

    char recvChar;

    while(1)

    {

        if(blueToothSerial.available())

        {

            recvChar = blueToothSerial.read();

            recvBuf += recvChar;

            nameIndex = recvBuf.indexOf(slaveName);             //get the position of slave name

            

                                                                //nameIndex -= 1;

                                                                //decrease the ';' in front of the slave name, to get the position of the end of the slave address

            if ( nameIndex != -1 )

            {

                //Serial.print(recvBuf);

                addrIndex = (recvBuf.indexOf(retSymb,(nameIndex - retSymb.length()- 18) ) + retSymb.length());//get the start position of slave address

                slaveAddr = recvBuf.substring(addrIndex, nameIndex);//get the string of slave address

                break;

            }

        }

    }

    

    //form the full connection command

    connectCmd += slaveAddr;

    connectCmd += "\r\n";

    int connectOK = 0;

    Serial.print("Connecting to slave:");

    Serial.print(slaveAddr);

    Serial.println(slaveName);

    //connecting the slave till they are connected

    do

    {

        blueToothSerial.print(connectCmd);//send connection command

        recvBuf = "";

        while(1)

        {

            if(blueToothSerial.available()){

                recvChar = blueToothSerial.read();

                recvBuf += recvChar;

                if(recvBuf.indexOf("CONNECT:OK") != -1)

                {

                    connectOK = 1;

                    Serial.println("Connected!");

                    blueToothSerial.print("Connected!");

                    break;

                }

                else if(recvBuf.indexOf("CONNECT:FAIL") != -1)

                {

                    Serial.println("Connect again!");

                    break;

                }

            }

        }

    }while(0 == connectOK);

}
