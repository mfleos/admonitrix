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
int lstTemp[] = {0,0,0,0,0,0};
unsigned long lstTime[] = {0,0,0,0,0,0};
unsigned long initMillisRead = 0;
int lstTempSize = sizeof(lstTemp)/sizeof(int);
int lstTimeSize = sizeof(lstTime)/sizeof(unsigned long);
int numReads = 0;


void setup() {
  Serial.begin(9600);
  delay(200);
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
      printValues();
      
      String json;
      char jsonEncoded[500];
          
      Serial.println("-----------------------");
      json = makeJson();
      Serial.println("-----------------------");
      encodeJson(json, jsonEncoded);
      Serial.println("-----------------------");
      numReads = 0;
  }
}

String makeJson(){
  String strJsonTemps = "";
  for(int iterTime = 0, iterTemp = 0; (iterTime < lstTimeSize) && (iterTemp < lstTempSize); iterTime++, iterTemp++){
    String tempEntry = "{\'t"+ String(iterTime) + "\':" + String(lstTime[iterTime]) + "," + "\'T"+ String(iterTemp) + "\':" + String(lstTemp[iterTemp]) + "}," ; 
    strJsonTemps.concat(tempEntry);
  }
  strJsonTemps = strJsonTemps.substring(0, strJsonTemps.length()-1 );
  String sendTime= String(millis());
  String strLecturas =  "{\'ID\':" + ID + "," + "\'time\':" + sendTime + "," + "\'records\':[" + strJsonTemps + "]}" ;
  Serial.print("strLecturas: " );
  Serial.println(strLecturas);
  
  return strLecturas;       // needed to send it to URLEncode (compiler yells if not)
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

char *encodeJson(String toEncode, char *encodedMsg){
  char jsonChar[300];                           //size is massive, to hold the full json char array
  toEncode.toCharArray(jsonChar,300);
  
  const char *hex = "0123456789abcdef";
  char *msg = jsonChar;
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
  return encodedMsg;
}

