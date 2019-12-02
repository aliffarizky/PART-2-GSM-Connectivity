#include <SoftwareSerial.h>
SoftwareSerial myGsm(3,4);
#define DEBUG true
String data_ip[8];
String ip;

void setup()
{
 myGsm.begin(9600);  
 Serial.begin(9600);  
 delay(500);

 Serial.println("========================INIT===============================");
 sendData("AT+CGATT=1",200,DEBUG);
 
 sendData("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"",1000,DEBUG);
 sendData("AT+SAPBR=3,1,\"APN\",\"\"",5000,DEBUG);
 sendData("AT+SAPBR=1,1",10000,DEBUG);

 sendData("AT+HTTPINIT",2000,DEBUG);
 sendData("AT+HTTPPARA=\"URL\",\"http://ipv4bot.whatismyipaddress.com\"",1000,DEBUG);
 sendData("AT+HTTPACTION=0",3000,DEBUG);
 sendipData("AT+HTTPREAD=0,20",3000,DEBUG);
 sendData("AT+HTTPTERM",1000,DEBUG);
 Serial.println("====================DONE, PRINT THE IP===========================");
 Serial.println("Ip:"+ip);
}

void loop()
{

}

//===========================AT COMMAND=================================
String sendData (String command , const int timeout ,boolean debug){
  String response = "";
  myGsm.println(command);
  long int time = millis();
  int i = 0;

  while ( (time+timeout ) > millis()){
    while (myGsm.available()){
      char c = myGsm.read();
      response +=c;
    }
  }
  if (debug) {
     Serial.print(response);
     }
     return response;
}

//===========================PARSE IP=================================
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
    delay(1500); //delay to reset string
    data_ip[2].remove(0); //reset ip address for next task
  }
}
