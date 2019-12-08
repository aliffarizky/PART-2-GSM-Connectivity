#include <SoftwareSerial.h>  //include software serial library
SoftwareSerial client(3,4); //Assigning TX(3) & RX(4) pin to the module
#define DEBUG true //defining debug variable

//Define Variables
String raw_gsm,gsm,ip; 
String data_ip[3];
String data_gsm[3];
void setup()
{
 client.begin(9600);  
 Serial.begin(9600);  
 delay(500);

 connectGPRS(); //Setup GPRS Connection
 
}

void loop()
{
  readIP(); //Read the Ip from htttp://ipv4bot.whatismyipaddress.com 
  if (ip == "") //check if GET HTTP fail to read the IP
  {
    readIP(); //Read the IP
  }
  readGSM(); //Read GSM Signal Strength
  Serial.println("Ip address :"+ip);
  Serial.println("GSM Strength:"+gsm+"dBm");
  delay(1000);
  clear_buf(); //clear
}

//=========================================================READ GSM====================================================================
void readGSM()
{
  sendGsmData("AT+CSQ",1000,DEBUG); //AT-Command to display GSM Signal Strength Info and parsing it. Ranged and conversed to 113 - 53 dBM, https://m2msupport.net/m2msupport/atcsq-signal-quality/
  delay(1000);
}

//=========================================================PARSE GSM====================================================================
void sendGsmData(String command , const int timeout , boolean debug)
{
  client.println(command); //print the command to module
  long int time = millis(); //Set timer for timeout
  int k = 0;

  while((time+timeout) > millis()){ //while timeout is not exceeded
    while(client.available()){
      char e = client.read(); //read modules respond
      delay(100);
      if (e != '\n') { //parse modules respond with (\n) as delimiter
         data_gsm[k] +=e; //add modules respond to string array
         delay(100);
      } else {
        k++;  
      }
      if (k == 3) {
        delay(100);
        goto exitc;
      }
    }
  }exitc:
  if (debug) {
    data_gsm[1].remove(0,6);
    raw_gsm = data_gsm[1]; //get parsed gps strength from modules respond
    raw_gsm.trim();
    gsm = (((raw_gsm.toInt()-1)*2) - 111); //conversing 0-99 value to (-113)-(-53) dBm, https://m2msupport.net/m2msupport/atcsq-signal-quality/
    gsm.trim();
  }
}

//=========================================================INIT GPRS====================================================================
void connectGPRS()
{ 
  sendData("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"",1000,DEBUG); //AT-Command to declare Connection Type
  delay(1000);

  sendData("AT+SAPBR=3,1,\"APN\",\"\"",1000,DEBUG); //AT-Command to Enable APN Setting
  delay(1000);

  sendData("AT+SAPBR=1,1",1000,DEBUG); //AT-Command to enable GPRS and APN Setting
  delay(1000);
}

//=========================================================READ IP====================================================================
void readIP()
{
  sendData("AT+HTTPINIT",1000,DEBUG); //AT-Command to initialize HTTP
  delay(1000);
  sendData("AT+HTTPPARA=\"URL\",\"http://ipv4bot.whatismyipaddress.com\"",1000,DEBUG); //AR-Command Setting HTTP Parameter
  delay(1000);
  sendData("AT+HTTPACTION=0",1000,DEBUG); //AT-Command to Enable GET Action
  delay(1000); 
  sendIpData("AT+HTTPREAD=0,20",3000,DEBUG); //AT-Command to read HTTP GET respond and parsing it
  delay(3000);
  sendData("AT+HTTPTERM",1000,DEBUG); //AT-Command to Terimnate HTTP Service
  delay(1000);
}


//=========================================================PARSE IP====================================================================
void sendIpData(String command , const int timeout , boolean debug)
{
  client.println(command); //print the command to module
  long int time = millis(); //Set timer for timeout
  int m = 0;

  while((time+timeout) > millis()){ //while timeout is not exceeded
    while(client.available() > 0){ 
      char g = client.read(); //read modules respond
      delay(100);
      if (g != '\n')  //parse modules respond with (\n) as delimiter
      {
         data_ip[m] +=g; //add modules respond to string array
         delay(100);
      } else 
      {
        m++;  
      }
      if (m == 3) 
      {
        delay(100);
        goto exitd;
      }
    }
  }
  exitd:
  if (debug) 
  {
    ip = data_ip[2]; //get parsed gps strength from modules respond
    ip.trim();
  }
}


//=========================================================WRITE FUNCTION====================================================================
String sendData (String command , const int timeout ,boolean debug)
{
  String response = ""; //Clear modules respond
  delay(1000);
  client.println(command); //Print/Send Command to modules
  long int time = millis();
  int h = 0;

  while ( (time+timeout ) > millis()) //While timeout not exceeded 
  {
    while (client.available())
    {
      char b = client.read(); //Read the module's respond
      delay(100);
      response +=b; //store the respond to variable
    }
  }
  if (debug) 
    {
     Serial.print(response); //Serial print the respond
     delay(100);
    }
     return response;
}

//==================================================Clear Serial=================================
void clear_buf() //Clear Payload
{
  raw_gsm = "";
  gsm = "";
  data_gsm[0]="";
  data_gsm[1]="";
  data_gsm[2]="";
  data_gsm[3]="";
  data_ip[0]="";
  data_ip[1]="";
  data_ip[2]="";
  data_ip[3]="";
}
