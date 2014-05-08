// pin declarations

#define PINSENS0 	2
#define PINSENS1 	3
#define PINSENS2 	4

#define SENS0		0
#define SENS1		1
#define SENS2		2


byte g_sensors = B00000000;
byte g_oldSensors = B00000000;

int lstValues[] = {0,0,0,0,0,0,0,0}

void setup(){
	pinMode(PINSENS0, INPUT);
	pinMode(PINSENS1, INPUT);
	pinMode(PINSENS2, INPUT);
	Serial.begin(9600);
	Serial.print("Hello - Init Now");
	Serial.print("----------------\n");
}

void loop(){
	detectSensors();
	if ( g_sensors != g_oldSensors){
		displaySensors();
		g_oldSensors = g_sensors;
	}
	updateValues();
	makeString();
	delay(1000);

}

// check if sensors where attached or removed
void detectSensors(){
	int sense0 = digitalRead(PINSENS0);
	int sense1 = digitalRead(PINSENS1);
	int sense2 = digitalRead(PINSENS2);

	if (sense0 == 1){
		bitSet(g_sensors,SENS0);
		printVariable(g_sensors);
	}else{
		bitClear(g_sensors,SENS0);
	}

	if (sense1 == 1){
		bitSet(g_sensors,SENS1);
		printVariable(g_sensors);
	}else{
		bitClear(g_sensors,SENS1);
	}

	if (sense2 == 1){
		bitSet(g_sensors,SENS2);
		printVariable(g_sensors);
	}else{
		bitClear(g_sensors,SENS2);
	}
}

//Send by serial port the status or g_sensors variable
void displaySensors(){
	for(int i = 0; i < 3; i++){
		Serial.print("Estado sensor ");
		Serial.print(i);
		Serial.print(" :");
		if(bitRead(g_sensors, i) == 1){
			Serial.print("Activo - ");
		}else{
			Serial.print("Inactivo - ");
		}
		Serial.println(bitRead(g_sensors, i));
	}
}

void printVariable(int var){
	Serial.print("Valor: ");
	Serial.print(var, BIN);
	Serial.print("\n");

}

int readSensor(int sensor){
	int rawReading = analogRead(sensor);
	Serial.print("Sensor: ");
	Serial.print(sensor);
	Serial.print(" - Read : ");
	Serial.println(rawReading);
	return rawReading;
}

void updateValues(){
	if (bitRead(g_sensors, SENSE0) == 1){
		lstValues[0] = readSensor(SENSE0);
	}
	if (bitRead(g_sensors, SENSE1) == 1){
		lstValues[1] = readSensor(SENSE1);
	}
	if (bitRead(g_sensors, SENSE2) == 1){
		lstValues[2] = readSensor(SENSE2);
	}
}

void makeString(){
	Serial.print("String: ");
	for (int i = 0; i < 8; i++){
		Serial.print(lstValues[i]);
		Serial.print("- ");
	}
	Serial.print("\n");
}