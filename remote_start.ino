
/*

  This project will use a gsm module to recieve incoming text messages.
  Based on what the text reads, the arduino can run different sub-routines.
  In this sketch, the arduino is connected to a 8 relay board that you can
  interface with the real world such a truck to remote start.

  Circuit:
  GSM module attached to and Arduino pins 3 and 4 with tx and rx
  SIM card that can receive SMS messages
  8 relays to control various things

  created 1/21/2016
  by ZooRatedProductions

  based on code by Javier Zorzano / TD & Tom Igoe

  Edited by EzCrash

*/

// include the GSM library
#include <GSM.h>

// PIN Number for the SIM (set if sim pin code protected)
#define PINNUMBER ""

// initialize the library instances
GSM gsmAccess;
GSM_SMS sms;

// Array to hold the number a SMS is retreived from
char senderNumber[20];
char outGoing[16];

// flag to make sure you can not start vehicle while it's already running
int runningflag = 0;

// variables to define pins to be set for each relay
int on = 12;
int start = 11;
int parking_lights = 10;
int acc = 9;
int on2 = 8;
int relay_6 = 6;
int relay_7 = 5;
int relay_8 = 4;

// define String variable to hold incoming message
String message;

void setup()
{
  // configure pins to connect to relays
  pinMode(on, OUTPUT);
  pinMode(start, OUTPUT);
  pinMode(parking_lights, OUTPUT);
  pinMode(acc, OUTPUT);
  pinMode(on2, OUTPUT);
  pinMode(relay_6, OUTPUT);
  pinMode(relay_7, OUTPUT);
  pinMode(relay_8, OUTPUT);

  digitalWrite(on, 1);
  digitalWrite(start, 1);
  digitalWrite(parking_lights, 1);
  digitalWrite(acc, 1);
  digitalWrite(on2, 1);
  digitalWrite(relay_6, 1);
  digitalWrite(relay_7, 1);
  digitalWrite(relay_8, 1);

//initialize serial communications and wait for port to open:
    Serial.begin(9600);
    while (!Serial) {
     ; // wait for serial port to connect. Needed for Leonardo only
    }
    
  // configure and initialise GSM connection
  Serial.println("SMS Messages Receiver");

  // connection state
  boolean notConnected = true;

  // Start GSM connection
  while (notConnected)
  {
    if (gsmAccess.begin(PINNUMBER) == GSM_READY)
      notConnected = false;
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("GSM initialized");
  Serial.println("Waiting for messages");
}

void loop()
{
  char c;

  // If there are any SMSs available()
  if (sms.available())
  {
    Serial.println("Received SMS from:");

    // Get remote number
    sms.remoteNumber(senderNumber, 20);
    Serial.println(senderNumber);

    // Read message bytes and print them
    while (c = sms.read())
      message += c; // build the message from the sms.read() function

    //Serial.print(c);
    Serial.println(message); // display message on serial for testing
    message.toLowerCase(); // convert to lower case to make sure the comparision works later

    Serial.println("\nEND OF MESSAGE");

  
    if (message == "start" && runningflag == 0)  //if message contains certain text, do task
    {
      Serial.println("Received message to start car");

      sms.flush();  //delete message
      Serial.println("MESSAGE DELETED");

      Startup();

      for (int del = 0; del <= 300; del++) { //timer loop to run for 10 minutes, but able to check for messages every 2 seconds


        if (sms.available())
        {
          Serial.println("Message received from:");
          message = "";
          // Get remote number
          sms.remoteNumber(senderNumber, 20);
          Serial.println(senderNumber);

          // Read message bytes and print them
          while (c = sms.read())
            message += c; // build the message from the sms.read() function
          message.toLowerCase();

          if (message == "turnoff")  //if engine is running, stop the engine
          {
            Serial.println("Received message to stop engine");

            Stopengine();

            Serial.println("\nCOMPLETE");

            message = ""; //resets the String variable message to blank to be able to accept the next SMS into the String
            break;
          }


        }

        delay(2000);

        if (del == 300) {
          Sleepmode(); //when the timer runs out, turn off engine
        }
      }

      Serial.println("\nCOMPLETE");

      message = "";
    }

    if (message == "turnoff")
    {
      Serial.println("received message to stop engine");

      Stopengine();

      Serial.println("\nCOMPLETE");

      message = "";

    }
    else {
      message = "";
    }

    // Delete message from modem memory
    sms.flush();
    Serial.println("MESSAGE DELETED");
  }

  delay(1000);

}

/* function that will start the vehicle and set flag to 1. Flash parking lights,
  turn ignition on, and start engine.
*/
void Startup()
{
  runningflag = 1;

  for (int x = 0; x <= 7; x++) {

    digitalWrite(parking_lights, 1);
    delay(200);
    digitalWrite(parking_lights, 0);
    delay(200);
    digitalWrite(parking_lights, 1);

  }

  delay(1000);
  digitalWrite(on, 0);
  delay(3000);
  digitalWrite(start, 0);
  delay(1200);
  digitalWrite(start, 1);
  delay(3000);
  digitalWrite(on2, 0);
  delay(3000);
  digitalWrite(acc, 0);

}

/* function to shut off the vehicle. Flash parking lights, turn ignition off, and set the flag to 0.
*/

void Stopengine()
{

  for (int x = 0; x <= 5; x++) {

    digitalWrite(parking_lights, 0);
    delay(200);
    digitalWrite(parking_lights, 1);
    delay(200);
  }

  delay(200);
  digitalWrite(on, 1);
  delay(200);
  digitalWrite(on2, 1);
  delay(200);
  digitalWrite(acc, 1);

  runningflag = 0;
}

/*function to not cause confusion while on the road. After the 9 minute timer has run out, it will
  shut off the ignition, and shut off the parking lights. If you have inserted the key and turned it to run,
  the vehicle will continue to run.
*/
void Sleepmode() {
  digitalWrite(parking_lights, 1);
  digitalWrite(on, 1);
  digitalWrite(on2, 1);
  digitalWrite(acc, 1);

  
}
