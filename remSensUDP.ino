 /*
  Remote temperatur monitor
  Arduino/ Ethernet based

  Written by: Eden Candelas 

*/

#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008



#define SENS 0
#define READ_TIME 5
#define SEND_TIME 60
#define MULTIPLIER 100

/*-------------- Ethernet parameters and configurations------------------*/
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 3);
IPAddress dnServer(8,8,4,4);
IPAddress gw(192,168,1,1);

//IPAddress server(198, 58, 124, 96);
IPAddress server(192, 168, 1, 100);

unsigned int rPort = 7070;

EthernetUDP Udp;
unsigned int localPort = 8888;

char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,

//Identificador equipo
String ID = "1";
int lstTemp[] = {0,0,0,0,0,0};
unsigned long lstTime[] = {0,0,0,0,0,0};
unsigned long initMillisRead = 0;
int lstTempSize = sizeof(lstTemp)/sizeof(int);
int lstTimeSize = sizeof(lstTime)/sizeof(unsigned long);
int numReads = 0;


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
      
      char jsonChar[400];
      //char jsonEncoded[400];
          
      Serial.println("-----------------------");
      makeJson(jsonChar);
      Serial.println("-----------------------");
      // encodeJson(jsonChar, jsonEncoded);
      // Serial.println("-----------------------");
      // udpSendme(jsonEncoded);
      numReads = 0;
  }
}

int makeJson(char *jsonCharArray){
  // String strJsonTemps = "";
  char *ptr = jsonCharArray;
  char jsonTempsArray[40];
  // strcpy(jsonTempsArray,"");
  for(int iterTime = 0, iterTemp = 0; (iterTime < lstTimeSize) && (iterTemp < lstTempSize); iterTime++, iterTemp++){
    snprintf(jsonTempsArray,"{'t%i':%lu,'T%i':%i}",iterTime,lstTime[iterTime],iterTemp,lstTemp[iterTemp]);
    // String tempEntry = "{\'t"+ String(iterTime) + "\':" + String(lstTime[iterTime]) + "," + "\'T"+ String(iterTemp) + "\':" + String(lstTemp[iterTemp]) + "}," ; 
    strncat(jsonCharArray,jsonTempsArray,400);
  }
  jsonCharArray=ptr;
  Serial.print("jsonCharArray: ");
  Serial.println(jsonCharArray);
  // strJsonTemps = strJsonTemps.substring(0, strJsonTemps.length()-1 );
  // String sendTime= String(millis());
  // String strLecturas =  "{\'ID\':" + ID + "," + "\'time\':" + sendTime + "," + "\'records\':[" + strJsonTemps + "]}" ;
  // Serial.print("strLecturas: " );
  // Serial.println(strLecturas);
  
  return 0;       // needed to send it to URLEncode (compiler yells if not)
}

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
  if(i < (sizeof(lstTemp)/sizeof(int))- 1){
    i += 1;
  }else{
    i = 0;  
  }
  if( x < (sizeof(lstTime)/sizeof(unsigned long)) -1){
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
          Serial.print(*encodedMsg);
          encodedMsg++;
    } else {
      *encodedMsg = '%';
      Serial.print(*encodedMsg);
      encodedMsg++;
      *encodedMsg = hex[*msg >> 4];
      Serial.print(*encodedMsg);
      encodedMsg++;
      *encodedMsg = hex[*msg & 15];
      Serial.print(*encodedMsg);
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
  Udp.begin(localPort);
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
  Serial.println(7070);
  Serial.println("---------------");
}

void udpSendme(char *datosTx){
  Udp.beginPacket(server, 7070);
  Udp.write(datosTx);         
  Udp.endPacket();
  Serial.print("Datos Tx: ");
  Serial.println(datosTx);
  Serial.println("------------");
}