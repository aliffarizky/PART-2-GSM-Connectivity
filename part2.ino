#include <SoftwareSerial.h>
#define DEBUG true
SoftwareSerial myGsm(3,4);
String data_csq[5];
String data_ip[3];
String csq,real_csq,ip;

void setup()
{
 myGsm.begin(9600);  
 Serial.begin(9600);  
 delay(500);

 Serial.println("======================INIT=============================");
 sendData("AT+CGATT=1",200,DEBUG); //enable GPRS Connection
 delay(1000);
 sendData("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"",1000,DEBUG); //Declare Connection Type (GPRS)
 delay(1000);
 sendData("AT+SAPBR=3,1,\"APN\",\"\"",5000,DEBUG); //Delare APN Name
 delay(1000);
 sendData("AT+SAPBR=1,1",10000,DEBUG); //Enable GPRS & APN Setting
 delay(1000);
 sendData("AT+CSQ",200,DEBUG); //GSM Signal Quality
 //Value from AT+CSQ 1 = -111 dBm with +2 dBm each incerement on value
 delay(1000);

 sendData("AT+HTTPINIT",2000,DEBUG); //Initializing HTTP
 delay(1000);
 sendData("AT+HTTPPARA=\"URL\",\"http://ipv4bot.whatismyipaddress.com\"",1000,DEBUG); //Setting HTTP Parameter
 delay(1000);
 sendData("AT+HTTPACTION=0",3000,DEBUG); //Enabling GET Action
 delay(1000);
 sendipData("AT+HTTPREAD=0,20",3000,DEBUG); //Read HTTP Data and parsing it
 delay(1000);
 sendData("AT+HTTPTERM",1000,DEBUG); //Terminate HTTP Service
 delay(1000);
 sendCSQData("AT+CSQ",200,DEBUG); //Parsing GSM signal response
 delay(1000);
 Serial.println("================DONE, PRINT IP ADRESS & GSM STRENGTH======================");
 Serial.println("Ip:"+ip);
 real_csq = (((csq.toInt() - 1) * 2) - 111);  //Value from AT+CSQ 1 = -111 dBm with +2 dBm each incerement on value
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
      if (i == 3) 
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

//===============================PARSING IP================================================

void sendipData(String command , const int timeout , boolean debug){

  myGsm.println(command);
  long int time = millis();
  int i = 0;

  while((time+timeout) > millis()){
    while(myGsm.available()){
      char c = myGsm.read();
      if (c != '\n') {
         data_ip[i] +=c;
         delay(100);
      } else {
        i++;  
      }
      if (i == 3) {
        delay(100);
        goto exitL;
      }
    }
  }exitL:
  if (debug) {
    ip = data_ip[2];
    delay(1000);
    data_ip[2].remove(0);
  }
}
