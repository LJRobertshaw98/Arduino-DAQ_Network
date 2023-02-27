#include <SPI.h>

#define opcPin 4
#define txLength 2
#define SDO 12                         // set pin 12 as SDO

volatile byte input = 0;               // value received from chain
volatile byte pos = 0;                 // our current position in the byte array
volatile unsigned int sensorValue = 0; // 2 bytes: 0 to 65,535.
volatile byte tx[txLength];            // 2 byte transmit buffer

void setup() {
  // turn SPI on in 'peripheral' mode: no equivalent of SPI.begin() for peripherals
  pinMode(opcPin, OUTPUT);                     // operation complete output
  digitalWrite(opcPin, HIGH);                  // set high: operation is indeed complete
  pinMode(SDO, OUTPUT);                        // have to send on peripheral SDO
  SPCR |= _BV(SPE);                            // turn on SPI in peripheral mode
  SPCR |= _BV(SPIE);                           // turn on interrupts
  attachInterrupt (0, CS_falling, FALLING);    // interrupt CS falling: selected to transact
  attachInterrupt (1, Trigger, FALLING);       // interrupt trigger falling edge
  sensorValue = analogRead(A0);                // initialise ADC
  sensorValue = analogRead(A0);                
  //Serial.begin(115200);
  //Serial.write("Dear Python, Good day to you! Yours sincerely, Arduino"); 
}

// Interrupt service routine when trigger goes low [attach trigger to interrupt1: pin D3]
void Trigger() {
  digitalWrite(opcPin, LOW);         // operation is NOT complete!
  sensorValue = analogRead(A0);      // read ADC: no multiplexing, no crosstalk
  tx[0] = lowByte(sensorValue);      // mask least significant byte
  tx[1] = highByte(sensorValue);     // shift right and mask most significant byte
  pos = 0;                           // prepare to transmit first byte of message
  //Serial.println(sensorValue);
  digitalWrite(opcPin, HIGH);        // assert operation complete line
}

// Interrupt service routine when CS goes low [attach CS to interrupt0: pin D2]
void CS_falling() { // message placed in register in response to CS
  SPDR = tx[pos];   // place relevant byte in SPI register
  pos += 1;         // move to next position in transmission buffer
}

//SPI interrupt service routine
ISR (SPI_STC_vect) {
  // Nothing needs to be done here:
  // - reading is placed in SPDR in response to CS
  // - data that is transferrred in will be transferred straight back out
}


void loop() {}
