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
#define MULTIPLIER 1000


//configuracion de interface de red
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 3);
IPAddress dnServer(8,8,4,4);
IPAddress gw(192,168,1,1);

// direccion de servidor

//IPAddress server(198, 58, 124, 96);
IPAddress server(192, 168, 1, 100);

unsigned int rPort = 7070;

//Se crea el objeto udp y asignamos un valor al puerto local 
EthernetUDP Udp;
unsigned int localPort = 8888;

//Identificador equipo

String ID = "1";
int lstTemp[] = {0,0,0,0,0,0,0,0,0,0,0,0};
unsigned long lstTime[] = {0,0,0,0,0,0,0,0,0,0,0,0};
int *ptrTemp = lstTemp;
unsigned long *ptrTime = lstTime;

unsigned long initMillisRead = 0;
unsigned long initMillisSend = 0;


// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,


void setup() {
  // start the Ethernet,UDP and Serial:
  Serial.begin(9600);
  delay(200);
  Serial.println("Iniciando");
  
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
  initMillisRead = millis();
  initMillisSend = millis();
  Serial.print("initMillisRead: ");
  Serial.println(initMillisRead);
  Serial.print("initMillisSend: ");
  Serial.println(initMillisSend);
}

void loop() {
  if (timer(READ_TIME, initMillisRead) == 1){
    takeRead(SENS);
    initMillisRead = millis();
  }
  if (timer(SEND_TIME, initMillisSend) == 1){
    //udpSendme(makeString());
    printValues();
    initMillisSend = millis();
  }
}

String makeString(){
  String strJsonTemps = "{'t1':1333456, 'T1':45}"

  String strLecturas =  "{\'ID\':" + ID + "," 
                          + "\'time\':" + String(millis()) + ","
                          + "\'records\':[" + strJsonTemps + "}" ;
  return strLecturas;
}



void udpSendme(String datos){
  char datosTx[32];
  datos.toCharArray(datosTx, 32);
  Udp.beginPacket(server, 7070);
  Udp.write(datosTx);         
  Udp.endPacket();
  Serial.print("Datos Tx: ");
  Serial.println(datosTx);
  Serial.println("------------");
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
  Serial.print("Lectura: ");
  Serial.print(lstTemp[i]);
  Serial.print("-");
  Serial.println(lstTime[x]); 
  if(i < (sizeof(lstTemp)/sizeof(int))+ 1){
    i += 1;
  }else{
    i = 0;  
  }
  if( x < (sizeof(lstTime)/sizeof(unsigned long)) +1){
    x += 1;
  }else{
    x = 0;
  }
}

void printValues(){
  Serial.println("---Send timer expired---");
  for ( int i = 0; i < 12 ; i++ ){
    Serial.print("Read ");
    Serial.print(i);
    Serial.print(":");
    Serial.println(lstTemp[i]);
  }
}

