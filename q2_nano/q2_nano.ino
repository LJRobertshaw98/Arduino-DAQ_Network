#include <SPI.h>

#define opcPin 4                       // OPC asserted on Operation Complete
#define txLength 2                     // data length for SPI transmission
#define SDO 12                         // set pin 12 as SDO

volatile byte input = 0;               // value received from controller
volatile byte pos = 0;                 // current position in the tx byte array
volatile unsigned int sensorValue = 0; // 2 bytes: 0 to 65,535.
volatile byte tx[txLength];            // 2 byte transmit buffer

void setup() {
  // turn SPI on in 'peripheral' mode: no equivalent of SPI.begin() for peripherals
  pinMode(opcPin, OUTPUT);                     // operation complete output
  digitalWrite(opcPin, HIGH);                  // set high: operation is indeed complete
  pinMode(SDO, OUTPUT);                        // send on peripheral SDO
  SPCR |= _BV(SPE);                            // turn on SPI in peripheral mode
  SPCR |= _BV(SPIE);                           // turn on interrupts
  attachInterrupt (0, CS_falling, FALLING);    // interrupt CS falling: selected to transact
  attachInterrupt (1, Trigger, FALLING);       // interrupt trigger falling edge
  sensorValue = analogRead(A0);                // initialise ADC
  sensorValue = analogRead(A0); 
  //Serial.begin(115200);                       // uncomment for debugging purposes
  //Serial.write("Dear Python, Good day to you! Yours sincerely, Arduino"); 
}

// Interrupt service routine when trigger goes low [attach trigger to interrupt1: pin D3]
void Trigger() {
  digitalWrite(opcPin, LOW);         // unassert OPC: operation is NOT complete!
  sensorValue = analogRead(A0);      // read ADC: single channel, no crosstalk
  tx[0] = lowByte(sensorValue);      // least significant byte -> transmit buffer
  tx[1] = highByte(sensorValue);     // most significant byte -> transmit buffer
  //Serial.println(sensorValue);
  digitalWrite(opcPin, HIGH);        // assert operation complete
}

// Interrupt service routine when CS goes low [attach CS to interrupt0: pin D2]
void CS_falling() { // prepare SPI communication:
  SPDR = tx[0];     // put first byte of message into SPI transfer register
  pos = 1;          // next position in (arbitrary length) transmit buffer
}

//SPI interrupt service routine: each byte of SPI transfer
ISR (SPI_STC_vect) {
  input = SPDR;         // input from SPI data transfer register - ignored
  if (pos < txLength) { // while position within allocated message:
    SPDR = tx[pos];     //     put next byte into SPI transfer register
    pos += 1;           //     increment position in transmit buffer
  }
}

void loop() {}          // nothing to see here!
