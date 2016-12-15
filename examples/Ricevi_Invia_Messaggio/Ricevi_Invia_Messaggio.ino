/*
   Fishgram.h - Library for using Telegram with Fishino.
   Created by Andrea S. Costa, December 9, 2016.
   In questo sketch riceviamo da Telegram ogni messaggio
   e rispondiamo se il mittente era autorizzato.
*/



//-------------------------------------------------------------------------------------------------------------------------
//Inclusione librerie

#include <Fishino.h>
#include <SPI.h>
#include <String.h>
#include <Fishgram.h>
#include <Flash.h>
//-------------------------------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------------------------------
// Configurazione WIFI

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
//Avvio il client x connessioni https

FishinoSecureClient client;
//-------------------------------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------------------------------
//Creo un'istanza della classe Fishgram SOLO DOPO aver creato un client sicuro e SOLO DOPO aver inserito l'access token

const char accessToken[] = "*********:***********************************";
Fishgram bot(accessToken, client);
//-------------------------------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------------------------------
//Variabili globali

//Valore dell'id autorizzato
long id_giovanni = ********;
//-------------------------------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------------------------------
//Void esterna per riavvio ESP

void resetSetESP()
{
  Serial << F("Reset Wi-Fi\r\n");
  // resetta e testa il modulo WiFi
  while (!Fishino.reset())
    ;

  // imposta la modalità stazione: è quindi necessario un access point wifi al quale collegarsi
  Fishino.setMode(STATION_MODE);

  // tenta la connessione finchè non riesce
  while (!Fishino.begin(MY_SSID, MY_PASS))
  {
    delay(2000);
  }

  Fishino.config(ip, gw);  //Imposto l'IP e il Gateway

  // In attesa fino a che la connessione non viene stabilita
  while (Fishino.status() != STATION_GOT_IP)
  {
    delay(500);
  }
}
//-------------------------------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------------------------------
//Void esterna (ogni dieci mins reset ESP)

unsigned long tempoCorrente;
unsigned long tempoPrecedente = 0;
unsigned long intervallo = 600000;  //Modificare questo valore per modificare l'intervallo di tempo tra due reset

void timeEsp ()
{
  tempoCorrente = millis();

  //Se sono passati 50 gg e millis va in overflow, millis riparte da 0.
  //Perciò tempoCorrente diventerebbe < di tempoPrecedente
  //Lo resetto a 0
  if ((tempoPrecedente - tempoCorrente) < 0)
  {
    tempoPrecedente = 0;
  }

  //Controllo se sono passati dieci minuti dall'ultimo riavvio
  if (tempoCorrente - tempoPrecedente >= intervallo)
  {
    Serial << F("10 minuti trascorsi\r\n");
    tempoPrecedente = tempoCorrente;
    //prima di resettarmi chiudo la connessione con Telegram per sicurezza
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
  //inizializzo l'ESP
  resetSetESP();
}
//-------------------------------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------------------------------
//Void loop

void loop()
{
  //Evitare richieste https troppo frequenti
  delay(2000);

  //controllo se sono passati due minuti dall'ultimo riavvio dell'ESP
  timeEsp();

  //Creo una struttura oldestMessage per salvarci i dati che ricevo tramite il metodo getOldestMessage:
  //isEmpty è una variabile bool che è true se il messaggio json era una stringa vuota o non valida, altrimenti è false
  //sender_id è una stringa con l'id del mittente del messaggio
  //chat_id è una stringa con l'id della chat dalla quale è partito il messaggio verso il bot.
  //text è una stringa contenente il testo ricevuto. ATTENZIONE! Nel testo sono presenti le " iniziali e le " finali. (da tenere presente in un eventuale comparazione di stringhe)
  oldestMessage messaggioRicevuto = bot.getOldestMessage();

  //Converto la stringa con il valore dell'id del mittente in un valore numerico
  unsigned long senderID = (messaggioRicevuto.sender_id).toInt();

  //Se il messaggio non è vuoto E se l'id della chat corrisponde a uno degli id autorizzati
  if ((!messaggioRicevuto.isEmpty) && (senderID == id_giovanni))
  {
    //stampo il testo del messaggio
    Serial.println(messaggioRicevuto.text);

    //Rispondo al mittente usando il metodo sendMessage utilizzando una frase di mia scelta
    const char rispostaBot[] = "Ciao, come stai?";
    bot.sendMessage(rispostaBot, messaggioRicevuto.sender_id);
    //Posso anche rispondere con ciò che il mittente mi ha scritto
    bot.sendMessage(messaggioRicevuto.text, messaggioRicevuto.sender_id);
  }
}
//-------------------------------------------------------------------------------------------------------------------------
