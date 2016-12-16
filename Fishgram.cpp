/*
  Fishgram.h - Library for using Telegram with Fishino.
  Created by Andrea S. Costa, December 9, 2016.
*/
#include "Fishgram.h"


//Create a structure that will be modified by getOldestMessage and then returned into the void loop
oldestMessage olMe;
//Store the value of the offset
long offset = 1;
//The following variable is used to distinguish the id of the sender from the chat one into the json parser.
bool checkID;
//The following variable is used to distinguish a right JSON (not empty, not coming from groups/bot)
bool checkJson;

//the constructor needs a const char * containing the bot token and the client object of FishinoSecureClient class
Fishgram::Fishgram(const char* accessToken, FishinoSecureClient &client)
{
  this->client = &client;
  this->accessToken = accessToken;
}

//private void called internally by getOldestUpdates method to connect to Telegram
void Fishgram::start()
{
  if (!client->connected())
  {
    while (client->available())
    {
      char kk = client->read();
    }
    (client->connect(HOST, SSL_PORT));
  }
}

//this public void allows to log off from Telegram
void Fishgram::closeConnection()
{
  client->stop();
}

//private void for parsing a JSON Message
void parserCallback(uint8_t filter, uint8_t level, const char *name, const char *value, void *obj)
{
  if ((strcmp(name, "update_id") == 0))
  {
    String offsetString = value;
    offset = offsetString.toInt();
    offset = offset + 1;
  }
  if ((strcmp(name, "from") == 0))
  {
    checkID = true;
  }
  if ((strcmp(name, "chat") == 0))
  {
    checkID = false;
  }
  if ((strcmp(name, "id") == 0))
  {
    if (checkID)
    {
      olMe.sender_id = value;
    }
    if (!(checkID))
    {
      olMe.chat_id = value;
    }
  }
  if ((strcmp(name, "text") == 0))
  {
    olMe.text = value;
    //If I find the text parameter i know that the JSON is not empty.
    //I just have to check that comes from the sender chat
    if (olMe.chat_id == olMe.sender_id)
    {
      checkJson = true;
    }
  }
}

//this public void is used to get a message from Telegram and returns a oldestMessage structure
oldestMessage  Fishgram::getOldestMessage()
{
  //If something for some reasons remained into the incoming buffer I will clean it
  while (client->available())
  {
    char so = client->read();
  }

  //Prepare the parser
  JSONStreamingParser parser;
  parser.setCallback(parserCallback, 0);
  parser.reset();
  
  //Start with a wrong JSON message. 
  //If it will be right, parserCallback will change it in true.
  checkJson = false;
   
  start();

  //Request to Telegram
  String getRequest = "GET /bot" + String(accessToken) + "/getUpdates?offset=" + String(offset) + "&timeout=8&limit=1&allowed_updates=messages HTTP/1.1";
  client->println(getRequest);
  client->println("User-Agent: curl/7.37.1");
  client->println("Host: api.telegram.org");
  client->println();

  //Waiting answer (long polling)
  unsigned long tim = millis() + 12000;
  while ((!client->available()) && (millis() < tim))
  {
    ;
  }
 
  while (client->available())
  {
    char s = client->read();
    //JSON message starts with {
    if (s == '{')
    {
      while (client->available())
      {
        //Send characters to parserCallback();
        parser.feed(s);
        s = client->read();
      }
    }
  }

  //If JSON is wrong a empty oldestMessage structure will be returned.
  if (!checkJson)
  {
    olMe.text = "\0";
    olMe.sender_id = "\0";
    olMe.chat_id = "\0";
    olMe.isEmpty = true;
    parser.reset();
    return olMe;
  } else {
    olMe.isEmpty = false;
    parser.reset();
    return olMe;
  }
}


void Fishgram::sendMessage(const char* answerMessage, String receiver_id)
{
  start();
  
  this->answerMessage  = answerMessage;
  this->receiver_id = receiver_id;

  String  jsonMess = "{\"chat_id\":\"" + receiver_id + "\",\"text\":\"" + answerMessage + "\"}";
  client->print("POST /bot");
  client->print(String(accessToken));
  client->println("/sendMessage HTTP/1.1");
  client->println("Host: api.telegram.org");
  client->println("Content-Type: application/json");
  client->print("Content-Length: ");
  client->println(jsonMess.length());
  client->println();
  client->println(jsonMess);

  while (client->available())
  {
    char kkk = client->read();
  }
  
  client->stop();
}


void Fishgram::sendMessage(String &answerMessage, String receiver_id)
{
  start();

  this->answerMessage  = answerMessage;
  this->receiver_id = receiver_id;

  String  jsonMess = "{\"chat_id\":\"" + receiver_id + "\",\"text\":\"" + answerMessage + "\"}";
  client->print("POST /bot");
  client->print(String(accessToken));
  client->println("/sendMessage HTTP/1.1");
  client->println("Host: api.telegram.org");
  client->println("Content-Type: application/json");
  client->print("Content-Length: ");
  client->println(jsonMess.length());
  client->println();
  client->println(jsonMess);
  this->answerMessage = "\0";

  while (client->available())
  {
    char kkk = client->read();
  }
  client->stop();
}
