#include <SPI.h>

#define triggerPin 2                // output used for trigger
#define opcPin 3                    // input operation complete
#define rxlength 2                  // receive 2 bytes of data from peripheral
#define SPI_freq 5000000            // Frequency of SPI bus clock SCK - 5 MHz

const int CS[] = {10};              // Chip-select - array to add peripherals easily
const int peripherals = sizeof(CS)/sizeof(CS[0]); // number of peripherals
const int txlength = peripherals*rxlength + 2;    // data length: 2 bytes for time delta
char tx[txlength];                  // transmit buffer -> USB to PC

byte output = 0;                    // byte to send to peripheral (ignored)
unsigned int dTime=0;               // timestamping
unsigned long timeNow=0;
unsigned long lastTime=0;

void setup() {
  Serial.begin(115200);                  // data back to PC as fast as possible
  pinMode(opcPin, INPUT);                // Initialise opcPin as an input
  pinMode(triggerPin, OUTPUT);           // Initialise triggerPin as an output
  for (int i=0; i<peripherals; i++) {    // for each peripheral:
    pinMode(CS[i], OUTPUT);              //    initialise the CS pin as an output
  }
  SPI.begin();                           // Initialise SPI as 'controller'
  for (int i=0; i<peripherals; i++) {    // for each peripheral:
      digitalWrite(CS[i], HIGH);         //     disable CS - addressed on FALLING
  }     
  digitalWrite(triggerPin, HIGH);        // untrigger measurement: trigger on FALLING
                                         // Start SPI communications with specified settings
  SPI.beginTransaction(SPISettings(SPI_freq, MSBFIRST, SPI_MODE0)); 
  Serial.write("Dear Python, Good day to you! Yours sincerely, Arduino"); 
  lastTime=micros();                
}

void loop() {
  digitalWrite(triggerPin, LOW);            // trigger measurement(s)
  timeNow=micros();                         // generate difference timestamp
  dTime = (unsigned int)(timeNow-lastTime); 
  lastTime=timeNow;
                                            // LAST 2 bytes of tx contain timestamp:
  tx[txlength-2] = lowByte(dTime);          // least significant byte
  tx[txlength-1] = highByte(dTime);         // most significant byte

  digitalWrite(triggerPin, HIGH);           // untrigger measurement(s)
  while (digitalRead(opcPin)==LOW) {}       // wait for OPC asserted by peripheral(s)

  for (int j=0; j<peripherals; j++) {       // for each peripheral:
    digitalWrite(CS[j], LOW);               // select peripheral device as listener    
    delayMicroseconds(6);                   // allow peripheral to respond to CS
    for (int i=0; i<rxlength; i++) {        // loop over length of expected message
      delayMicroseconds(6);                 // a short pause between bytes to reduce errors
      tx[rxlength*j + i] = SPI.transfer(output); // read in the next byte, simultaneously 
    }                                       // sending 'output', place data in tx buffer 
    digitalWrite(CS[j], HIGH);              // disable CS - unaddress peripheral
  }
  Serial.write(tx, txlength);               // output message to PC
}
