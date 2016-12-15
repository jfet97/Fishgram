/*
   Fishgram.h - Library for using Telegram with Fishino.
   Created by Andrea S. Costa, December 9, 2016.
   In this sketch we receive every message sent to our bot
   and we'll reply if the sender is authorized
*/



//-------------------------------------------------------------------------------------------------------------------------
//Libraries inclusion

#include <Fishino.h>
#include <SPI.h>
#include <String.h>
#include <Fishgram.h>
#include <Flash.h>
//-------------------------------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------------------------------
// WIFI Configuration

#define MY_SSID  "****"

#define MY_PASS  "****"

#define IPADDR  *, *, *, *

#define GATEWAY *, *, *, *


#ifdef IPADDR
IPAddress ip(IPADDR);
#endif
#ifdef GATEWAY
IPAddress gw(GATEWAY);
#endif
//-------------------------------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------------------------------
//Create a secure client for a https connection

FishinoSecureClient client;
//-------------------------------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------------------------------
//Create an instance of the class Fishgram ONLY AFTER have created a secure client and ONLY AFTER have entered bot token

const char accessToken[] = "*********:***********************************";
Fishgram bot(accessToken, client);
//-------------------------------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------------------------------
//gGlobal variables

//Authorized id value
long id_john = ********;
//-------------------------------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------------------------------
//external void for ESP restart

void resetSetESP()
{
  Serial << F("Reset Wi-Fi\r\n");

  while (!Fishino.reset())
    ;

  Fishino.setMode(STATION_MODE);

  while (!Fishino.begin(MY_SSID, MY_PASS))
  {
    delay(2000);
  }

  Fishino.config(ip, gw);

  while (Fishino.status() != STATION_GOT_IP)
  {
    delay(500);
  }
}
//-------------------------------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------------------------------
//Every 10 minutes I reset the ESP module (This ensures more stability)

unsigned long currentTime;
unsigned long previousTime = 0;
unsigned long interval = 600000;  //Change this value to change the time interval between two reset (ms)

void timeEsp ()
{
  currentTime = millis();

  //Following if construct is useful only after millis overflow
  if ((previousTime - currentTime) < 0)
  {
    previousTime = 0;
  }

  if (currentTime - previousTime >= interval)
  {
    previousTime = currentTime;
    bot.closeConnection();
    resetSetESP();
  }
}
//-------------------------------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------------------------------
//Void setup

void setup()
{
  Serial.begin(115200);
  delay(10);
  Serial << F("Start\r\n");
  // inizializza il modulo SPI
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  //start wifi connection
  resetSetESP();
}
//-------------------------------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------------------------------
//Void loop

void loop()
{
  //Following delay is used to avoid frequent requests to Telegram
  delay(2000);

  //If it's been ten minutes after the last reset, reset again ESP module .
  timeEsp();

  // Create a oldestMessage structure to save data returned by getOldestMessage method:
  // isEmpty is a boolean variable that is true if the json message was an empty or wrong string, false otherwise
  // sender_id is a string with sender's id
  // chat_id is a string with chat's id
  // text is a string containing the received text. WARNING! In the text there are "". (To consider for a strings comparison)
  oldestMessage receivedMessage  = bot.getOldestMessage();

  //store sender's id in a unsigned long variable
  unsigned long senderID = (messaggioRicevuto.sender_id).toInt();

  //If the message is not empty or wrong and if the sender's id is equal to the authorized id...
  if ((!receivedMessage.isEmpty) && (senderID == id_john))
  {
    //print message's text
    Serial.println(receivedMessage.text);

    //Reply to the sender with a "random" message...
    const char reply[] = "Hi, how are you?";
    bot.sendMessage(reply, receivedMessage.sender_id);
    
    //...or with the same text received
    bot.sendMessage(receivedMessage.text, receivedMessage.sender_id);
  }
}
//-------------------------------------------------------------------------------------------------------------------------
