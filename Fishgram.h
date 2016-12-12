/*
  Fishgram.h - This library allows you to manage your Fishino card using Telegram.
  Created by Andrea S. Costa, December 9, 2016.
*/
#ifndef Fishgram_h
#define Fishgram_h


#include "Arduino.h"
#include "Fishino.h"                                          
#include "JSONStreamingParser.h"
#include "String.h"

#define HOST "api.telegram.org"
#define PORT 443

struct oldestMessage{
  String text;
  String sender_id;
  String chat_id;
  bool isEmpty;
};


class Fishgram
{
  public:
   Fishgram(const char* accessToken, FishinoSecureClient &client);
   oldestMessage getOldestMessage();
   void sendMessage(const char* answerMessage, String receiver_id);
   void sendMessage(String &answerMessage, String receiver_id);
   void closeConnection();
  private:
   void start();
   FishinoSecureClient *client;
   String accessToken;
   String receiver_id;
   String answerMessage;
};

#endif
