
/*------------------Sensors status -------------------*/
void detectSensors(){
	sense0 = digitalRead(PINSENS0);
	sense1 = digitalRead(PINSENS1);
	sense2 = digitalRead(PINSENS2);

	if (sense0 == 1){
		bitset(g_sensors,SENS0);
	}else{
		bitclear(g_sensors,SENS0);
	}

	if (sense1 == 1){
		bitset(g_sensors,SENS1);
	}else{
		bitclear(g_sensors,SENS1);
	}

	if (sense2 == 1){
		bitset(g_sensors,SENS2);
	}else{
		bitclear(g_sensors,SENS2);
	}
}


void displaySensors(){
	for(int i = 0; i < 3; i++){
		Serial.print("Estado sensor ");
		Serial.print(i);
		Serial.print(" :");
		if(bitread(g_sensors, i) == 1){
			Serial.print("Activo - ");
		}else{
			Serial.print("Inactivo - ");
		}
		Serial.println(bitread(g_sensors, i));
	}
}

/*-------------- Data Collection --------------*/

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




/*-----------------data sent ---------------------*/

void sendData(int sensor, int read){

}

void stablishConnection(){
	client.connect();
	unsigned long initMillis = millis() ;
	Serial.print("--Stablishing connection --\n");
	while(timerTryConnection(DELAY, initMillis)){
		Serial.print("Connection success to: ")
		Serial.print(server);
		Serial.print(" : ");
		Serial.print(port);
		Serial.print("\n");
		return 1;
	}
	Serial.print("Connection to: ");
	Serial.print(server);
	Serial.print(" : ");
	Serial.print(port);
	Serial.print(" failed.");
	Serial.print("\n");

	return 0;

}

int timerTryConnection(unsigned long delay, unsigned long init){
	unsigned long currentMillis = millis();
	if ((currentMillis - init) >= delay){
		Serial.print("Timer Expired\n");
		return 0;
	}else{
		return 1
	}

}


/*--------------- funciones de soporte -----------------*/

void printVariable(int var){
	Serial.print("Valor: ");
	Serial.println(var, BIN);
}






