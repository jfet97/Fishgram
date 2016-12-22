/*
  Fishgram.h - Library for using Telegram with Fishino.
  Created by Andrea S. Costa, December 9, 2016.
*/
#ifndef Fishgram_h
#define Fishgram_h


#include "Arduino.h"
#include "Fishino.h"                                          
#include "JSONStreamingParser.h"
#include "String.h"
#include "Flash.h"
#define HOST "api.telegram.org"
#define SSL_PORT 443

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
   void reset();
   void jsonAnswer();
   FishinoSecureClient *client;
   String accessToken;
   String receiver_id;
   String answerMessage;
};

#endif
