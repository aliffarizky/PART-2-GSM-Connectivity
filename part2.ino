#include <SoftwareSerial.h>
SoftwareSerial myGsm(3,4);
String data_csq[5];
#define DEBUG true
String csq,real_csq;

void setup()
{
 myGsm.begin(9600);  
 Serial.begin(9600);  
 delay(500);

 Serial.println("======================INIT=============================");
 sendData("AT+CGATT=1",200,DEBUG); //enable GPRS Connection
 sendData("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"",1000,DEBUG); //Declare Connection Type (GPRS)
 sendData("AT+SAPBR=3,1,\"APN\",\"\"",5000,DEBUG); //Delare APN Name
 sendData("AT+SAPBR=1,1",10000,DEBUG); //Enable GPRS & APN Setting
 sendData("AT+CSQ",200,DEBUG); //GSM Signal Quality
 //Value from AT+CSQ 1 = -111 dBm with +2 dBm each incerement on value

 sendData("AT+HTTPINIT",2000,DEBUG); //Initializing HTTP
 sendData("AT+HTTPPARA=\"URL\",\"http://ipv4bot.whatismyipaddress.com\"",1000,DEBUG); //Setting HTTP Parameter
 sendData("AT+HTTPACTION=0",3000,DEBUG); //Enabling GET Action
 sendData("AT+HTTPREAD",3000,DEBUG); //Read Data
 sendData("AT+HTTPTERM",1000,DEBUG); //Terminate HTTP Service

 sendCSQData("AT+CSQ",200,DEBUG); //Parsing GSM signal response
 Serial.println("==================DONE, PRINT GSM STRENGTH===========================");
 real_csq = (((csq.toInt() - 1) * 2) - 111);
 Serial.println("Gsm:"+real_csq+" dBm");
 
}

void loop()
{

}

//============================AT COMMAND=================================
String sendData (String command , const int timeout ,boolean debug)
{
  String response = "";
  myGsm.println(command);
  long int time = millis();
  int i = 0;

  while ( (time+timeout ) > millis())
  {
    while (myGsm.available())
    {
      char c = myGsm.read();
      response +=c;
    }
  }
  if (debug) 
    {
     Serial.print(response);
    }
  return response;
}

//============================PARSE CSQ (Signal Quality)=============================
void sendCSQData(String command , const int timeout , boolean debug)
{
  myGsm.println(command);
  long int time = millis();
  int i = 0;

  while((time+timeout) > millis())
  {
    while(myGsm.available())
    {
      char c = myGsm.read();
      if (c != '\n') 
      {
         data_csq[i] +=c;
         delay(100);
      } 
      else 
      {
        i++;  
      }
      if (i == 6) 
      {
        delay(100);
        goto exitL;
      }
    }
  }
  exitL:
  if (debug) 
  {
    data_csq[1].remove(0,6);
    csq = data_csq[1];
  }
}
