#include <SPI.h>

#define peripherals 2               // number of peripherals
#define triggerPin 2
#define opcPin 3
#define rxlength 2                  // 2 bytes of data from peripherals
#define SPI_freq 500000             // Frequency of SPI bus clock SCK - 5 MHz

const int CS = 10;                  // Chip-select - single common value
const int txlength = peripherals*rxlength + 2;    // data length: 2 bytes for time delta

byte output = 0;                    // byte to send to peripheral
char tx[txlength];                  // transmit buffer -> USB to PC

unsigned int dTime=0;
unsigned long timeNow=0;
unsigned long lastTime=0;

void setup() {
  Serial.begin(115200);                  // data back to PC as fast as possible
  pinMode(opcPin, INPUT);                // Initialise opcPin as an input
  pinMode(triggerPin, OUTPUT);           // Initialise triggerPin as an output
  pinMode(CS, OUTPUT);                   // initialise the CS pin as an output
  SPI.begin();                           // Initialise SPI as 'controller'
  digitalWrite(CS, HIGH);                // disable CS - addressed on FALLING
  digitalWrite(triggerPin, HIGH);        // untrigger measurement
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
                                            // last 2 bytes of tx contain the timestamp:
  tx[txlength-2] = lowByte(dTime);          // least significant byte
  tx[txlength-1] = highByte(dTime);         // most significant byte

  digitalWrite(triggerPin, HIGH);           // untrigger measurement
  while (digitalRead(opcPin)==LOW) {}       // wait for OPC asserted by peripheral(s)

  for (int i=0; i<rxlength; i++) {          // for each byte in expected message length:
    digitalWrite(CS, LOW);                  // address peripheral devices for transaction    
    delayMicroseconds(6);                   // allow peripherals to respond to CS
    for (int n=0; n < peripherals; n++) {   // loop over peripheral devices - 1 byte from each
      delayMicroseconds(6);                 // a short pause between bytes to reduce errors
      tx[i+(2*n)] = SPI.transfer(output);   // read in the next byte, simultaneously 
    }                                       // sending 'output', place data in tx buffer 
    digitalWrite(CS, HIGH);                 // disable CS
    delayMicroseconds(6);                   // a short pause to ensure the CS is unset
  }  
  Serial.write(tx, txlength);               // output message to PC
}
