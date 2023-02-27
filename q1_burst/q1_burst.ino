#define channels 2
#define dataLength 200

unsigned long timeNow=0;
unsigned long lastTime=0;
unsigned int dTime[channels][dataLength];
int sensorValue[channels][dataLength];

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(115200); // initialize serial communication at 115200 bits per second:
  sensorValue[0][0] = analogRead(A0);
  sensorValue[0][0] = analogRead(A0);
  sensorValue[1][0] = analogRead(A1);
  sensorValue[1][0] = analogRead(A1);
  Serial.write("Yours sincerely, Arduino"); // A distinctive startup message 
  lastTime=micros(); // start time
}

// the loop routine runs over and over again forever:
void loop() {
  
  for (int i=0; i < channels; i++) { // read in the sensor values
    for (int j=0; j < dataLength; j++){
      sensorValue[i][j] = analogRead(i);
      timeNow=micros();
      dTime[i][j] = (unsigned int)(timeNow-lastTime); // small difference between (potentially) large values
      lastTime=timeNow;
    }
  }

  for (int i=0; i< channels; i++) { // write out the data
    for (int j=0; j < dataLength; j++){
      // send the two bytes corresponding to (unsigned) integer A0 reading
      Serial.write(lowByte(sensorValue[i][j]));
      Serial.write(highByte(sensorValue[i][j]));
      Serial.write(lowByte(dTime[i][j]));
      Serial.write(highByte(dTime[i][j]));
    }
  }  
}
