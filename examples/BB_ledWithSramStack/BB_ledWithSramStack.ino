/*
  Store 4096 digits of Eulers number e in the Uno335 Serial Sram.
  Display the digits sequentially on a 8x32 RGB LED matrix.
  
  v0.1 created 10 Oct. 2016
  by Engelbert Mittermeier (BlueberryE GmbH)
*/

// Include the library to access the Serial SRAM
#include <BB_SramStack.h>

// Include libraries for the 8x32 LED matrix
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <gamma.h>
#include <Adafruit_NeoPixel.h>

// Define the pin which is used as DataIn on the LED matrix
#define MATRIX_PIN 3

// MATRIX DECLARATION:
// Parameter 1 = width of NeoPixel matrix
// Parameter 2 = height of matrix
// Parameter 3 = pin number (most are valid)
// Parameter 4 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the matrix; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs are arranged in horizontal
//     rows or in vertical columns, respectively; pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns proceed
//     in the same order, or alternate lines reverse direction; pick one.
//   See example below for these values in action.
// Parameter 5 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
 
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(32, 8,  MATRIX_PIN,
  NEO_MATRIX_BOTTOM    + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);
 

// Amount of digits of e
const unsigned long digitsE = 4096;

// Initialize the stack
BB_SramStack stack(0x0000, digitsE);

// Declare a StackIterator variable
BB_StackIterator iter(&stack);

// newSequence is true, if the diplay of the digits of e has to start from
// the beginning
byte newSequence = 1;

// contains the 5 values which are displayed at once 
char values[5];

// define some color codes
#define white 0xFFFF
#define green 0x07E0
#define red 0xF800
#define cyan 0x07FF
#define magenta 0xF81F
#define yellow 0xFFE0
#define blue 0x001F

// contains the colors which are used to display the digits of e
uint16_t colors[] = {yellow, cyan, magenta, blue, red};

void setup() {
    // initialize the LED matrix
    matrix.begin();
    // set the brightness; max = 255
    // as we power the matrix with the Uno335, we will use only 100
    matrix.setBrightness(100); 

    // initialize the SRAM
    BB_SramStack::begin();

    // Display "compu" on the LED matrix to indicate the computation of e
    matrix.setTextWrap(false);
    matrix.setTextColor(red);
    matrix.fillScreen(0);
    matrix.setCursor(0, 0);
    matrix.print(F("compu"));
    matrix.show();
    
    // Write e to the SRAM
    writeEulerDigits(digitsE);
  
    // Display "4096!" on the LED matrix to show that the computation of e
    // is done
    matrix.fillScreen(0);
    matrix.setCursor(0, 0);
    matrix.setTextColor(green);
    matrix.print(String(digitsE) + "!");
    matrix.show();

    delay(2000);
}

void loop() {
    matrix.fillScreen(0);

    if (newSequence < 1){
        // some digits have already been displayed before

        // shift out the first digit
        values[0] = values[1];
        values[1] = values[2];
        values[2] = values[3];
        values[3] = values[4];

        if (iter.hasNext()){
            // add the next digit
            values[4] = iter.next() + '0';   // get the data and convert to char
            // display all 5 digits on the LED matrix
            for (int i = 0; i < sizeof(values); i++) matrix.drawChar(6 * i, 0, values[i], colors[i], 0x0000, 1);
        }
        else {
            // all 4096 digits have been displayed
            // set newSequence = 1 to restart from the beginning
            newSequence = 1;
            delay(2000);
        }
    } else {      
        // set the StackIterator to the first element of the stack
        iter = stack.iterator();

        // display some advertisment
        matrix.setTextWrap(false);
        matrix.setTextColor(blue);

        for (int i = matrix.width(); i >= -59; i--){
            matrix.fillScreen(0);
            matrix.setCursor(i,0);
            matrix.print(F("BlueberryE"));
            matrix.show();
            delay(50);
        }
        delay(500);

        // show the first digits of e
        matrix.fillScreen(0);
        values[0] = 'e';
        values[1] = '=';
        values[2] = '2';
        values[3] = '.';

        if (iter.hasNext()){
            values[4] = iter.next() + '0';          // get the data and convert to char
            newSequence = 0;
        }
        else values[4] = '_';
        for (int i = 0; i < sizeof(values); i++) matrix.drawChar(6 * i, 0, values[i], colors[i], 0x0000, 1);

    }
  
    matrix.show();
    delay(400);
 
}


// n digits of e are written to the Serial SRAM of the Uno335.
// To do this efficiently with the 8-bit Atmega328P with only 2KB SRAM internal memory, we use the 
// Spigot algorithm of A. Sale.
// see: A. H. J. Sale: The calculation of e to many significant digits. The Computer Journal, Vol. 11 (2), 1968. S. 229–230

void writeEulerDigits(unsigned long n){

    int m;
    
    switch (n){
        case 64:
            m = 49;
        break;
        case 128:
            m = 84;
        break;
        case 256:
            m = 146;
        break;
        case 512:
            m = 257;
        break;
        case 1024:
            m = 458;
        break;
        case 2048:
            m = 823;
        break;
        case 4096:
            m = 1493;
        break;
        default:
            return;
        break;
    }

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
        stack.push((byte) carry);  //  write the value to the sram
    }
}


