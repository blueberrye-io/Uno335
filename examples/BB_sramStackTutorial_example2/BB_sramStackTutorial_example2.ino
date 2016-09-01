/*
  Draw "BLUEBERRYE" with ASCII art.
  Pixels for the letters are stored in several stacks on the Uno335 Serial Sram.
  Multiple iterators are used for reading the pixels from the stacks.
  This sketch uses the BB_SramStack library

  created 10 Aug. 2016
  by Engelbert Mittermeier (BlueberryE GmbH)
*/

#include <BB_SramStack.h> // include the library


// define some useful constants
const char fill = '#';    // pattern used to draw a pixel
const char space = '.';   // pattern used to draw a space
const byte letWidth = 6;  // width of one letter
const byte letHeight = 7; // height of one letter
// pixels needed for one letter:
const unsigned long pixCount =(unsigned long) (letWidth * letHeight);
  
// define the stacks for the letters
BB_SramStack letB(0x0000, pixCount); // stack for letter "B"
BB_SramStack letL(0x0100, pixCount); // stack for letter "L"
BB_SramStack letU(0x0200, pixCount); // stack for letter "U"
BB_SramStack letE(0x0300, pixCount); // stack for letter "E"
BB_SramStack letR(0x0400, pixCount); // stack for letter "R"
BB_SramStack letY(0x0500, pixCount); // stack for letter "Y"


void setup(){
    Serial.begin(9600);
    BB_SramStack::begin();
    initLetterB(); // write the "pixels" into the stack for B
    initLetterL(); // write the "pixels" into the stack for L
    initLetterU(); // write the "pixels" into the stack for U
    initLetterE(); // write the "pixels" into the stack for E
    initLetterR(); // write the "pixels" into the stack for R
    initLetterY(); // write the "pixels" into the stack for Y
}

void loop(){
    // One line consists of 9 columns. Each columns will contain one letter.
    // We use one iterator for each column
    BB_StackIterator pos0 = letB.iterator();
    BB_StackIterator pos1 = letL.iterator();
    BB_StackIterator pos2 = letU.iterator();
    BB_StackIterator pos3 = letE.iterator();
    BB_StackIterator pos4 = letB.iterator();
    BB_StackIterator pos5 = letE.iterator();
    BB_StackIterator pos6 = letR.iterator();
    BB_StackIterator pos7 = letR.iterator();
    BB_StackIterator pos8 = letY.iterator();
    BB_StackIterator pos9 = letE.iterator();

    // print the line
    for (byte j = 0; j < letHeight; j++){
        for (byte i = 0; i < letWidth; i++) Serial.print((char) pos0.next());
        Serial.print(space);
        Serial.print(space);
        for (byte i = 0; i < letWidth; i++) Serial.print((char) pos1.next());
        Serial.print(space);
        Serial.print(space);
        for (byte i = 0; i < letWidth; i++) Serial.print((char) pos2.next());
        Serial.print(space);
        Serial.print(space);
        for (byte i = 0; i < letWidth; i++) Serial.print((char) pos3.next());
        Serial.print(space);
        Serial.print(space);
        for (byte i = 0; i < letWidth; i++) Serial.print((char) pos4.next());
        Serial.print(space);
        Serial.print(space);
        for (byte i = 0; i < letWidth; i++) Serial.print((char) pos5.next());
        Serial.print(space);
        Serial.print(space);
        for (byte i = 0; i < letWidth; i++) Serial.print((char) pos6.next());
        Serial.print(space);
        Serial.print(space);
        for (byte i = 0; i < letWidth; i++) Serial.print((char) pos7.next());
        Serial.print(space);
        Serial.print(space);
        for (byte i = 0; i < letWidth; i++) Serial.print((char) pos8.next());
        Serial.print(space);
        Serial.print(space);
        for (byte i = 0; i < letWidth; i++) Serial.print((char) pos9.next());
        Serial.println();
    }
    Serial.println("-------------------------------------------------------------------------------");
    delay(3000);

}

// The functions which write the pixels for the letters into the stacks:

void initLetterB(){
    for (byte i = 0; i < 5; i++) letB.push((byte) fill);
    letB.push((byte) space);
    letB.push((byte) fill);
    for (byte i = 0; i < 4; i++) letB.push((byte) space);
    for (byte i = 0; i < 2; i++) letB.push((byte) fill);
    for (byte i = 0; i < 4; i++) letB.push((byte) space);
    letB.push((byte) fill);
    for (byte i = 0; i < 5; i++) letB.push((byte) fill);
    letB.push((byte) space);
    letB.push((byte) fill);
    for (byte i = 0; i < 4; i++) letB.push((byte) space);
    for (byte i = 0; i < 2; i++) letB.push((byte) fill);
    for (byte i = 0; i < 4; i++) letB.push((byte) space);
    letB.push((byte) fill);
    for (byte i = 0; i < 5; i++) letB.push((byte) fill);
    letB.push((byte) space);
}
 
void initLetterL(){
    for (byte i = 0; i < 6; i++){
        letL.push((byte) fill);
        for (byte j = 0; j < 5; j++) letL.push((byte) space);
    }
    for (byte i = 0; i < 6; i++) letL.push((byte) fill);
}

void initLetterU(){
    for (byte i = 0; i < 6; i++){
        letU.push((byte) fill);
        for (byte j = 0; j < 4; j++) letU.push((byte) space);
        letU.push((byte) fill);
    }
    for (byte i = 0; i < 6; i++) letU.push((byte) fill);
}

void initLetterE(){
    for (byte i = 0; i < 6; i++) letE.push((byte) fill);
    for (byte i = 0; i < 2; i++){
        letE.push((byte) fill);
        for (byte j = 0; j < 5; j++) letE.push((byte) space);
    }
    for (byte i = 0; i < 4; i++) letE.push((byte) fill);
    for (byte i = 0; i < 2; i++) letE.push((byte) space);
    for (byte i = 0; i < 2; i++){
        letE.push((byte) fill);
        for (byte j = 0; j < 5; j++) letE.push((byte) space);
    }
    for (byte i = 0; i < 6; i++) letE.push((byte) fill);
}

void initLetterR(){
    for (byte i = 0; i < 5; i++) letR.push((byte) fill);
    letR.push((byte) space);
    for (byte i = 0; i < 2; i++){
        letR.push((byte) fill);
        for (byte j = 0; j < 4; j++) letR.push((byte) space);
        letR.push((byte) fill);
    }
    for (byte i = 0; i < 6; i++) letR.push((byte) fill);
    for (byte i = 0; i < 2; i++){
        letR.push((byte) fill);
        for (byte j = 0; j < 2; j++)letR.push((byte) space);
    }
    letR.push((byte) fill);
    for (byte i = 0; i < 3; i++) letR.push((byte) space);
    letR.push((byte) fill);
    letR.push((byte) space);
    letR.push((byte) fill);
    for (byte i = 0; i < 4; i++) letR.push((byte) space);
    letR.push((byte) fill);
}

void initLetterY(){
    letY.push((byte) fill);
    for (byte i = 0; i < 4; i++) letY.push((byte) space);
    letY.push((byte) fill);
    letY.push((byte) space);
    letY.push((byte) fill);
    for (byte i = 0; i < 2; i++) letY.push((byte) space);
    letY.push((byte) fill);
    letY.push((byte) space);
    for (byte i = 0; i < 5; i++){
        for (byte j = 0; j < 2; j++) letY.push((byte) space);
        letY.push((byte) fill);
        for (byte j = 0; j < 3; j++) letY.push((byte) space);
    }
}

