 /*
  Remote temperatur monitor
  Arduino/ Ethernet based

  Written by: Eden Candelas 

*/

#define SENS 0
#define READ_TIME 5
#define SEND_TIME 60
#define MULTIPLIER 100




//Identificador equipo

String ID = "1";
int lstTemp[] = {0,0,0,0,0,0,0,0,0,0,0,0};
unsigned long lstTime[] = {0,0,0,0,0,0,0,0,0,0,0,0};
unsigned long initMillisRead = 0;
unsigned long initMillisSend = 0;

void setup() {
  // start the Ethernet,UDP and Serial:
  Serial.begin(9600);
  delay(200);
  Serial.println("Iniciando");
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
    String info = makeString();
    Serial.println(info);
    initMillisSend = millis();
    Serial.println(initMillisSend);
  }
}


String makeString(){
  //String strJsonTemps = "{'t1':1333456, 'T1':45}";
  String strJsonTemps;
  for(int iterTime = 0, iterTemp = 0; (iterTime < (sizeof(lstTime)/sizeof(unsigned long))) && (iterTemp < (sizeof(lstTemp)/sizeof(int))); iterTime++, iterTemp++){
    String tempEntry = "{\'t"+ String(iterTime) + "\':" + String(lstTime[iterTime]) + "," + "\'T"+ String(iterTemp) + "\':" + String(lstTemp[iterTemp]) + "}," ; 
    strJsonTemps.concat(tempEntry);
  }
  strJsonTemps = strJsonTemps.substring(0, strJsonTemps.length()-1 );

  String strLecturas =  "{\'ID\':" + ID + "," + "\'time\':" + String(millis()) + ","+ "\'records\':[" + strJsonTemps + "]}" ;
  return strLecturas;
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

