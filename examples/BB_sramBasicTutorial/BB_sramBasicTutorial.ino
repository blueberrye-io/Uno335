/*
  Calculate Eulers number e and store 2048 digits in the Uno335 Serial SRAM.

  created 10 Aug. 2016
  by Engelbert Mittermeier (BlueberryE GmbH)
*/

#include <SPI.h>

// Define the SRAM select pins:
const byte sramSelect = A3;

// Define the operation codes:
const byte sramWriteData = 0x02;
const byte sramReadData = 0x03;
const byte sramWriteStatus = 0x01;

// Define the status modi:
const byte sramByteMode = 0x01;
const byte sramVirtualChipMode = 0x41;

// The SPI settings for the SRAM:
const SPISettings settings4Sram(16000000, MSBFIRST, SPI_MODE0);
 
// variables used to control the writing and reading of the digits of e
word sramMaxUsedAddr = 0x0000; 
word sramCurrentAddr = 0x0000;
byte restart = 1;
const byte maxDigitCount = 4;
byte values[maxDigitCount];
byte i;
word avail;

void setup() {
    Serial.begin(9600);

    // Set the SRAM select pin
    digitalWrite(sramSelect, HIGH);
    pinMode(sramSelect, OUTPUT);
    
    // Set the SS pin
    digitalWrite(SS, HIGH);
    pinMode(SS, OUTPUT);

    // Initialize the SPI library
    SPI.begin();

    // Write e to the SRAM
    writeEulerDigits();

    // set the SRAM to VIRUTAL CHIP mode
    SPI.beginTransaction(settings4Sram);
    digitalWrite(sramSelect, LOW);
    Serial.println("Set SRAM to Virtual Chip Mode");
    SPI.transfer(sramWriteStatus);         // SET STATUS REGISTER command
    SPI.transfer(sramVirtualChipMode);     // new 8-bit pattern for the status register
    digitalWrite(sramSelect, HIGH);
    SPI.endTransaction();
}

void loop() {
    if (!restart){
        avail = (sramMaxUsedAddr - sramCurrentAddr) + 1;
        if (avail > maxDigitCount) avail = maxDigitCount;
        else restart = 1;
        // Read the next digits
        SPI.beginTransaction(settings4Sram);
        digitalWrite(sramSelect, LOW);
        SPI.transfer(sramReadData);                    // Read DATA command
        SPI.transfer((byte) (sramCurrentAddr >> 8));   // MSB of address
        SPI.transfer((byte) sramCurrentAddr);          // LSB of address
        for (i = 0; i < avail; i++) values[i] = SPI.transfer(0xFF); 
        digitalWrite(sramSelect, HIGH);
        SPI.endTransaction();
        // Print the digits
        Serial.print("      ");
        for (i = 0; i < avail; i++) Serial.print(values[i], DEC);
        Serial.println();
        sramCurrentAddr = sramCurrentAddr + maxDigitCount;
    } else {
        Serial.println("\nE = 2.");
        restart = 0;
        sramCurrentAddr = 0x0000;
    }
    delay(200);
}

// 4096 digits of e are written to the Serial SRAM of the Uno335.
// To do this efficiently with the 8-bit Atmega328P with only 2KB SRAM internal memory, we use the 
// Spigot algorithm of A. Sale.
// see: A. H. J. Sale: The calculation of e to many significant digits. The Computer Journal, Vol. 11 (2), 1968. S. 229–230
void writeEulerDigits(){
    int n = 4096;
    int m = 1493;
    Serial.print("Writing ");Serial.print(n, DEC); Serial.println(" digits of E to the SRAM");
     
    // set the SRAM to BYTE mode
    SPI.beginTransaction(settings4Sram);
    digitalWrite(sramSelect, LOW);
    Serial.println("Set SRAM to Byte Mode");
    SPI.transfer(sramWriteStatus);  // SET STATUS REGISTER command
    SPI.transfer(sramByteMode);     // new 8-bit pattern for the status register
    digitalWrite(sramSelect, HIGH);
    SPI.endTransaction();

    byte coef[m + 1];
    for (int j = 2; j <= m; j++) coef[j] = 1;

    int carry;
    int temp;
    for (int i = 1; i <= n; i++){
        carry = 0;
        for (int j = m; j >= 2; j--){
            temp = coef[j] * 10 + carry;
            carry = temp / j;
            coef[j] = temp - carry * j;
        }
        Serial.print((byte) carry);
        SPI.beginTransaction(settings4Sram);
        digitalWrite(sramSelect, LOW);
        SPI.transfer(sramWriteData);             // WRITE DATA command
        SPI.transfer((byte) (sramCurrentAddr >> 8));      // MSB of address0
        SPI.transfer((byte) sramCurrentAddr);             // LSB of address0
        SPI.transfer((byte) carry);                       //  the value
        digitalWrite(sramSelect, HIGH);
        SPI.endTransaction();
        sramCurrentAddr = sramCurrentAddr + 1;
    }
    sramMaxUsedAddr = sramCurrentAddr - 1;
    Serial.println("\ndone");
}
