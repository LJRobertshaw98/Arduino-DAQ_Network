#define channels 2

unsigned long timeNow=0;
unsigned long lastTime=0;
unsigned int dTime [channels];
int sensorValue [channels];

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(115200); // initialize serial communication at 115200 bits per second:
  Serial.write("Dear Python, Good day to you! Yours sincerely, Arduino"); // A distinctive startup message 
  lastTime=micros(); // start time
}

// the loop routine runs over and over again forever:
void loop() {
  for (int i=0; i < channels; i++) { // read in the sensor values
    // read the input on analog pin i and discard:
    sensorValue[i] = analogRead(i);
    // read the input on analog pin i:
    sensorValue[i] = analogRead(i);
    // generate timestamp
    timeNow=micros();
    dTime[i] = (unsigned int)(timeNow-lastTime); // small difference between (potentially) large values
    lastTime=timeNow;     
  }

  for (int i=0; i< channels; i++) { // write out the data
    // send the two bytes corresponding to (unsigned) integer A0 reading
    Serial.write(lowByte(sensorValue[i]));
    Serial.write(highByte(sensorValue[i]));
    // send the two bytes corresponding to (unsigned) integer time difference
    Serial.write(lowByte(dTime[i]));
    Serial.write(highByte(dTime[i]));
  }  
}
