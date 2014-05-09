 /*
  Remote temperatur monitor
  Arduino/ Ethernet based

  Written by: Eden Candelas 

*/

#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008



#define SENS 0
#define READ_TIME 30
#define SEND_TIME 3600
#define MULTIPLIER 1000
#define SERVER_TIME 60

/*---------------------------- Ethernet parameters and configurations----------------------------*/
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192,168,100,55);
IPAddress dnServer(8,8,4,4);
IPAddress gw(192,168,100,1);

IPAddress server (50,116,27,102);
unsigned int rPort = 8081;


EthernetClient client;
unsigned int localPort = 8888;

char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,

//Identificador equipo
String ID = "1";
int lstTemp[] = {0,0,0,0,0,0,0,0,0,0,0,0};
unsigned long lstTime[] = {0,0,0,0,0,0,0,0,0,0,0,0};
unsigned long initMillisRead = 0;
int lstTempSize = sizeof(lstTemp)/sizeof(int);
int lstTimeSize = sizeof(lstTime)/sizeof(unsigned long);
int numReads = 0;


/*-------------------------------------------MAIN ----------------------------------------------------*/

void setup() {
  Serial.begin(9600);
  delay(200);
  ethernetInit();
  Serial.println("Iniciando");
  initMillisRead = millis();
  Serial.print("initMillisRead: ");
  Serial.println(initMillisRead);
}

void loop() {

  if (timer(READ_TIME, initMillisRead) == 1){     //each time the timer expires take a new read and increments the reads counter
    takeRead(SENS);
    numReads += 1;
    initMillisRead = millis();
  }
  if (numReads == lstTempSize){                            // when reads counter reach the specified value generates the code
      // printValues();
      
      char jsonChar[300];jsonChar[0] = '\0';
      char jsonEncoded[600];
          
      Serial.println("-----------------------");
      makeJson(jsonChar);
      Serial.println("-----------------------");
      encodeJson(jsonChar, jsonEncoded);
      Serial.println("-----------------------");
      // udpSendme(jsonEncoded);
      tcpSendme(jsonEncoded);
      Serial.println("-----------------------");
      numReads = 0;
  }
}

/*----------------------------------------FUNCTIONS---------------------------------------------------*/



int timer (unsigned long retraso, unsigned long initTime){
  unsigned long milliSec = millis();
  if ((milliSec - initTime) >= (retraso * MULTIPLIER)){
      Serial.println("timer expired");
    return 1;
  } 
  return 0;
}

void takeRead(int sensor){
  static int i = 0;
  static int x = 0;
  int lectura = analogRead(sensor);
  unsigned long millisSec = millis();
  lstTemp[i] = lectura;
  lstTime[x] = millisSec;
  Serial.print("Lectura-");
  Serial.print(i);
  Serial.print(": ");
  Serial.print(lstTemp[i]);
  Serial.print("-");
  Serial.println(lstTime[x]); 
  if(i < lstTempSize- 1){
    i += 1;
  }else{
    i = 0;  
  }
  if( x < lstTimeSize -1){
    x += 1;
  }else{
    x = 0;
  }
}

void printValues(){
  Serial.println("---Reads info---");
  for ( int i = 0; i < lstTempSize ; i++ ){
    Serial.print("Read ");
    Serial.print(i);
    Serial.print(":");
    Serial.println(lstTemp[i]);
  }
}

int makeJson(char *jsonCharArray){
  char jsonTempsArray[40];jsonTempsArray[0] = '\0';

  for(int iterTime = 0, iterTemp = 0; (iterTime < lstTimeSize) && (iterTemp < lstTempSize); iterTime++, iterTemp++){
    snprintf(jsonTempsArray,40,"{'t%i':%lu,'T%i':%i}",iterTime,lstTime[iterTime],iterTemp,lstTemp[iterTemp]);
    strncat(jsonCharArray,jsonTempsArray,400);
  }

  Serial.print("jsonCharArray: ");
  Serial.println(jsonCharArray);

  return 0;       // needed to send it to URLEncode (compiler yells if not)
}

int encodeJson(char *charToEncode, char *encodedMsg){
  
  const char *hex = "0123456789abcdef";
  char *msg = charToEncode;
  char *originMsg;
  originMsg = encodedMsg;


  while (*msg!='\0'){
    if( ('a' <= *msg && *msg <= 'z')
        || ('A' <= *msg && *msg <= 'Z')
        || ('0' <= *msg && *msg <= '9') ) {
          *encodedMsg = *msg;
          // Serial.print(*encodedMsg);
          encodedMsg++;
    } else {
      *encodedMsg = '%';
      // Serial.print(*encodedMsg);
      encodedMsg++;
      *encodedMsg = hex[*msg >> 4];
      // Serial.print(*encodedMsg);
      encodedMsg++;
      *encodedMsg = hex[*msg & 15];
      // Serial.print(*encodedMsg);
      encodedMsg++;
      }
    msg++;
  }
  *encodedMsg = '\0';
  encodedMsg = originMsg;

  Serial.print("\n");
  Serial.print("jsonEncoded: ");
  Serial.println(encodedMsg);
// 
  return 0;
}

/*------------------- Inits Ethernet and UDP objects, returns the info serialized----------------*/

void ethernetInit(){
  Ethernet.begin(mac,ip,dnServer,gw);
  //Udp.begin(localPort);
  Serial.println("---------------");
  Serial.println("Configuracion de Red: ");
  Serial.print("mac: ");
  for(int x=0; x<6;x++){
    Serial.print("0x");
    Serial.print(mac[x], HEX);
    Serial.print(" , ");
  }
  Serial.println();
  // Serial.println(mac);
  Serial.print("ip: ");
  Serial.println(ip);
  Serial.print("gateway: ");
  Serial.println(gw);
  Serial.print("server: ");
  Serial.println(server);
  Serial.print("puerto server: ");
  Serial.println(rPort);
  Serial.println("---------------");
}

int tcpSendme(char *datosTx){
  char beginURL[100] = "GET /Tlogger/Principal?action=test&json=";

  Serial.print("Request: ");
  Serial.println(beginURL);
  Serial.println(datosTx);

  if (client.connect(server, rPort)) {
    Serial.println("connected");
    client.print(beginURL);
    client.print(datosTx);
    client.println();
  } else {
    Serial.println("connection failed");
    return 1;
  }

  unsigned long initTimerReply = millis();

  Serial.println("------------Sent--------------");

  while(timer(20, initTimerReply) == 0 ) {
  if(client.available() != 0){
      char c = client.read();
      Serial.print(c);
    }
  }

    // if the server's disconnected, stop the client:

  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }
  return 0;
}