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
  if (numReads == 6){                            // when reads counter reach the specified value generates the code
      printValues();
      
      String json;
      String jsonEncoded;
          
      Serial.println("-----------------------");
      json = makeJson();
      Serial.println("-----------------------");
      jsonEncoded = encodeJson(json);
      Serial.println("-----------------------");



      numReads = 0;
      
  }
}

String makeJson(){
  String strJsonTemps = "";
  for(int iterTime = 0, iterTemp = 0; (iterTime < (sizeof(lstTime)/sizeof(unsigned long))) && (iterTemp < (sizeof(lstTemp)/sizeof(int))); iterTime++, iterTemp++){
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
  for ( int i = 0; i < 6 ; i++ ){
    Serial.print("Read ");
    Serial.print(i);
    Serial.print(":");
    Serial.println(lstTemp[i]);
  }
}

String encodeJson(String toEncode){
  char jsonChar[300];
  toEncode.toCharArray(jsonChar,300);
  
  const char *hex = "0123456789abcdef";
  String encodedMsg = "";
  char *msg = jsonChar;

  while (*msg!='\0'){
    if( ('a' <= *msg && *msg <= 'z')
        || ('A' <= *msg && *msg <= 'Z')
        || ('0' <= *msg && *msg <= '9') ) {
          encodedMsg += *msg;
    } else {
      encodedMsg += '%';
      encodedMsg += hex[*msg >> 4];
      encodedMsg += hex[*msg & 15];
      }
    msg++;
  }
  Serial.print("encodedMsg: ");
  Serial.println(encodedMsg);

  return encodedMsg;
}

