/*
    BB_SramStack.cpp - Library to use the SRAM as stack.
    Created by E.Mittermeier, BlueberryE GmbH, Aug. 26, 2015.
    Released into the public domain.
*/

#include "Arduino.h"
#include "BB_SramStack.h"

uint8_t BB_SramStack::_initialized = 0;

// SPIE: SPI Interrupt Enable = 0
// SPE: SPI Enable = 1
// DORD: 0 -> Data order MSB first
// MSTR: Master SPI mode = 1
// CPOL: Clock Polarity
// CPHA: Clock Phase
// CPOL = 0, CPHA = 0 -> SPI Mode 0
// SPR1: SPI Clock Rate 1
// SPR0: SPI Clock Rate 0
// SPR1 = 0, SPR0 = 0 -> fastest
// SPR1 = 1, SPR0 = 1 -> slowest
//const uint8_t BB_SramStack::_spiSettingsSpcr = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0); // slowest
const uint8_t BB_SramStack::_spiSettingsSpcr = (1 << SPE) | (1 << MSTR); // fastest
const uint8_t BB_SramStack::_spiSettingsSpsr = 0;

const int BB_SramStack::_sramSelect = A3;
const byte BB_SramStack::_sramWriteData = 0x02;
const byte BB_SramStack::_sramReadData = 0x03;
const byte BB_SramStack::_sramWriteStatus = 0x01;
const byte BB_SramStack::_sramReadStatus = 0x05;
const unsigned long BB_SramStack::_maxSramCapacity = 0x10000;



// ----- start: Implementation of SramStack -----

    // ---- start: constructor SramStack

BB_SramStack::BB_SramStack(){
    this->_byteMode = true;
    this->_size = _maxSramCapacity;
    this->_startAddress = 0x0000;
    this->_topAddress = this->_startAddress;  // points to the next free address of the stack
    this->_isEmpty = true;
    this->_isFull = false;
}

BB_SramStack::BB_SramStack(char inMode){
    switch (inMode) {
        case 'w': // word mode
            this->_byteMode = false;
            this->_size = _maxSramCapacity / 2; 
            break;
        default: // either inMode == 'b' or wrong user input
            this->_byteMode = true;
            this->_size = _maxSramCapacity;
    }
    this->_startAddress = 0x0000;
    this->_topAddress = this->_startAddress;
    this->_isEmpty = true;
    this->_isFull = false;
}

BB_SramStack::BB_SramStack(word startAddress, unsigned long size){
    this->_byteMode = true;    
    if ((((unsigned long int) startAddress) + size) <= _maxSramCapacity){
        this->_size = size;
        this->_startAddress = startAddress;
        this->_topAddress = this->_startAddress;
        this->_isEmpty = true;
        if (size > 0){
            this->_isFull = false;
        } else {
            this->_isFull = true;
        }
    } else {
        this->_size = 0;
        this->_startAddress = startAddress;
        this->_isEmpty = true;
        this->_isFull = true;
    }
}

BB_SramStack::BB_SramStack(char inMode, word startAddress, unsigned long size){
    switch (inMode) {
        case 'w': // word mode
            this->_byteMode = false;
            if ((((unsigned long int) startAddress) + 2 * size) <= _maxSramCapacity){ 
                this->_size = size;
                this->_startAddress = startAddress;
                this->_topAddress = this->_startAddress;
                this->_isEmpty = true;
                if (size > 0){
                    this->_isFull = false;
                } else {
                    this->_isFull = true;
                }
            } else {
                this->_size = 0;
                this->_startAddress = startAddress;
                this->_isEmpty = true;
                this->_isFull = true;
            }
            break;
        default: // either inMode == 'b' or wrong user input
            this->_byteMode = true;
            if ((((unsigned long int) startAddress) + size) <= _maxSramCapacity){
                this->_size = size;
                this->_startAddress = startAddress;
                this->_topAddress = this->_startAddress;
                this->_isEmpty = true;
                if (size > 0){
                    this->_isFull = false;
                } else {
                    this->_isFull = true;
                }
            } else {
                this->_size = 0;
                this->_startAddress = startAddress;
                this->_isEmpty = true;
                this->_isFull = true;
            }
    }
}
    // ---- end: constructor SramStack

    // ---- start: public methods of SramStack -----
    
void BB_SramStack::begin() {
    uint8_t sreg = SREG;
    if (!_initialized) {
        // Set SS to high so a connected chip will be "deselected" by default
        // Warning: if the SS pin ever becomes a LOW INPUT then SPI
        // automatically switches to Slave, so the data direction of
        // the SS pin MUST be kept as OUTPUT.
        digitalWrite(SS, HIGH);
        digitalWrite(_sramSelect, HIGH);

        // When the SS pin is set as OUTPUT, it can be used as
        // a general purpose output port (it doesn't influence
        // SPI operations).
        pinMode(SS, OUTPUT);
        pinMode(_sramSelect, OUTPUT);
        
        SPCR = _spiSettingsSpcr;

        // Set direction register for SCK and MOSI pin.
        // MISO pin automatically overrides to INPUT.
        // By doing this AFTER enabling SPI, we avoid accidentally
        // clocking in a single bit since the lines go directly
        // from "input" to SPI control.
        // http://code.google.com/p/arduino/issues/detail?id=888
        pinMode(SCK, OUTPUT);
        pinMode(MOSI, OUTPUT);
        //pinMode(MISO, INPUT);
    }
    _initialized++;
    SREG = sreg;
}

boolean BB_SramStack::isEmpty(){
    return this->_isEmpty;
}


boolean BB_SramStack::isFull(){
    return this->_isFull;
}


word BB_SramStack::pop(){
    word cellContent = 0xFFFF;
    if (!this->isEmpty()){
        if (this->_byteMode){
            _setSramStatus('b');
            _beginTransaction();
            digitalWrite(_sramSelect, LOW);
            _transfer(_sramReadData);
            _transfer((byte) (this->_topAddress >> 8));
            _transfer((byte) this->_topAddress);
            cellContent = (word) _transfer(0xFF);
            digitalWrite(_sramSelect, HIGH);
            if (!(this->_topAddress == this->_startAddress)){
                this->_topAddress--;
            } else {
                // we popped the only remaining element of the stack
                // -> we have to set _isEmpty to true
                // -> we will not change the _topAddress
                this->_isEmpty = true;
            }
        } else { // we are in word mode
            _setSramStatus('v');
            _beginTransaction();
            digitalWrite(_sramSelect, LOW);
            _transfer(_sramReadData);
            _transfer((byte) (this->_topAddress >> 8));
            _transfer((byte) this->_topAddress);
            cellContent = _transfer16(0xFFFF);
            digitalWrite(_sramSelect, HIGH);
            if (!(this->_topAddress == this->_startAddress)){
                this->_topAddress = this->_topAddress - 2;
            } else {
                // we popped the only remaining element of the stack
                // -> we have to set _isEmpty to true
                // -> we will not change the _topAddress
                this->_isEmpty = true;
            }
        }
        this->_isFull = false;
    }    
    return cellContent;
}


word BB_SramStack::peek(){
    word cellContent = 0xFFFF;
    if (!this->isEmpty()){
        if (this->_byteMode){
            _setSramStatus('b');
            _beginTransaction();
            digitalWrite(_sramSelect, LOW);
            _transfer(_sramReadData);
            _transfer((byte) (this->_topAddress >> 8));
            _transfer((byte) this->_topAddress);
            cellContent = (word) _transfer(0xFF);
            digitalWrite(_sramSelect, HIGH);
        } else { // we are in word mode
            _setSramStatus('v');
            _beginTransaction();
            digitalWrite(_sramSelect, LOW);
            _transfer(_sramReadData);
            _transfer((byte) (_topAddress >> 8));
            _transfer((byte) _topAddress);
            cellContent = _transfer16(0xFFFF);
            digitalWrite(_sramSelect, HIGH);
        }
    }
    return cellContent;
}


byte BB_SramStack::push(byte inData){
    if (!this->isFull()){
        if (this->_byteMode){
            if (!this->isEmpty()){
                this->_topAddress++;
            }
            _setSramStatus('b');
            _beginTransaction();
            digitalWrite(_sramSelect, LOW);
            _transfer(_sramWriteData);
            _transfer((byte) (this->_topAddress >> 8));
            _transfer((byte) this->_topAddress);
            _transfer(inData);
            digitalWrite(_sramSelect, HIGH);
            this->_isEmpty = false;
            this->_isFull = (this->_topAddress == (this->_startAddress + ((word) (this->_size - 1))));
        } else {
            if (!this->isEmpty()){
                this->_topAddress = this->_topAddress + 2;
            }
            _setSramStatus('v');
            _beginTransaction();
            digitalWrite(_sramSelect, LOW);
            _transfer(_sramWriteData);
            _transfer((byte) (this->_topAddress >> 8));
            _transfer((byte) this->_topAddress);
            _transfer16((word)inData);
            digitalWrite(_sramSelect, HIGH);
            this->_isEmpty = false;
            this->_isFull = (this->_topAddress == (this->_startAddress + ((word) (2 * this->_size - 2))));
        }
        return 0;
    }
    return 1; 
}

byte BB_SramStack::push(word inData){
    if (!this->isFull()){
        if (this->_byteMode){
            if (!this->isEmpty()){
                this->_topAddress++;
            }
            _setSramStatus('b');
            _beginTransaction();
            digitalWrite(_sramSelect, LOW);
            _transfer(_sramWriteData);
            _transfer((byte) (this->_topAddress >> 8));
            _transfer((byte) this->_topAddress);
            _transfer((byte) inData);
            digitalWrite(_sramSelect, HIGH);
            this->_isEmpty = false;
            this->_isFull = (this->_topAddress == (this->_startAddress + ((word) (this->_size - 1))));
        } else {
            if (!this->isEmpty()){
                this->_topAddress = this->_topAddress + 2;
            }
            _setSramStatus('v');
            _beginTransaction();
            digitalWrite(_sramSelect, LOW);
            _transfer(_sramWriteData);
            _transfer((byte) (this->_topAddress >> 8));
            _transfer((byte) this->_topAddress);
            _transfer16((word)inData);
            digitalWrite(_sramSelect, HIGH);
            this->_isEmpty = false;
            this->_isFull = (this->_topAddress == (this->_startAddress + ((word) (2 * this->_size - 2))));
        }
        return 0;
    }
    return 1; 
}

void BB_SramStack::clear(){
    if (_byteMode){
        if ((((unsigned long int) this->_startAddress) + this->_size) <= _maxSramCapacity){
            this->_topAddress = this->_startAddress;
            this->_isEmpty = true;
            if (this->_size > 0){
                this->_isFull = false;
            } else {
                this->_isFull = true;
            }
        } else {
            this->_isEmpty = true;
            this->_isFull = true;
        }
    } else {
        if ((((unsigned long int) this->_startAddress) + 2 * this->_size) <= _maxSramCapacity){
            this->_topAddress = this->_startAddress;
            this->_isEmpty = true;
            if (this->_size > 0){
                this->_isFull = false;
            } else {
                this->_isFull = true;
            }
        } else {
            this->_isEmpty = true;
            this->_isFull = true;
        }
    }
}

BB_StackIterator BB_SramStack::iterator(){
    BB_StackIterator iteratorObj(this);
    return iteratorObj;
}

    // ---- end: public methods of SramStack -----

    // ---- start: private methods of SramStack -----

// Before using _transfer() or asserting chip select pins,
// this function is used to gain exclusive access to the SPI bus
// and configure the correct settings.
void BB_SramStack::_beginTransaction() {
    SPCR = _spiSettingsSpcr;
    SPSR = _spiSettingsSpsr;
}

// After performing a group of transfers and releasing the chip select
// signal, this function allows others to access the SPI bus

// not needed, because this lib does not work with interrupts anyway
// void SramStack::_endTransaction(void) {
//
//}

// Write 8bit to the SPI bus (MOSI pin) and also receive (MISO pin)
byte BB_SramStack::_transfer(byte data){
    SPDR = data;
    /*
     * The following NOP introduces a small delay that can prevent the wait
     * loop form iterating when running at the maximum speed. This gives
     * about 10% more speed, even if it seems counter-intuitive. At lower
     * speeds it is unnoticed.
     */
    asm volatile("nop");
    while (!(SPSR & _BV(SPIF))) ; // wait
    return SPDR;
}

// Write 16bit to the SPI bus (MOSI pin) and also receive (MISO pin)
word BB_SramStack::_transfer16(word data){
    union { word val; struct { byte lsb; byte msb; }; } in, out;
    in.val = data;
    SPDR = in.msb;
    asm volatile("nop"); 
    while (!(SPSR & _BV(SPIF))) ;
    out.msb = SPDR;
    SPDR = in.lsb;
    asm volatile("nop");
    while (!(SPSR & _BV(SPIF))) ;
    out.lsb = SPDR;
    return out.val;
}

void BB_SramStack::_setSramStatus(char inMode){
    switch (inMode) {
        case 'b': // byte mode - no hold: B00000001
            _beginTransaction();
            digitalWrite(_sramSelect, LOW);
            _transfer(_sramWriteStatus);
            _transfer(0x01);
            digitalWrite(_sramSelect, HIGH);
            break;
        case 'v': // virtual chip mode (vrtm) - no hold: B01000001
            _beginTransaction();
            digitalWrite(_sramSelect, LOW);
            _transfer(_sramWriteStatus);
            _transfer(0x41);
            digitalWrite(_sramSelect, HIGH);
            break;
    }
}

    // ---- end: private methods of SramStack -----
    
// ----- end: Implementation of SramStack -----

// ----- start: Implementation of SramStack -----

    // ---- start: constructor StackIterator
BB_StackIterator::BB_StackIterator(BB_SramStack *stack){
    this->_stack = stack;
    this->_currentAddress = stack->_startAddress;
    if (!(stack->isEmpty())){
        this->_hasNext = true;
    } else {
        this->_hasNext = false;
    }
    
}
    // ---- end: constructor StackIterator
    
    // ---- start: public methods of StackIterator -----
boolean BB_StackIterator::hasNext(){
    return this->_hasNext;
}


word BB_StackIterator::next(){
    word cellContent = 0xFFFF;
    if (this->_stack->_byteMode){
        this->_stack->_setSramStatus('b');
        this->_stack->_beginTransaction();
        digitalWrite(this->_stack->_sramSelect, LOW);
        this->_stack->_transfer(_stack->_sramReadData);
        this->_stack->_transfer((byte) (this->_currentAddress >> 8));
        this->_stack->_transfer((byte) this->_currentAddress);
        cellContent = (word) this->_stack->_transfer(0xFF);
        digitalWrite(this->_stack->_sramSelect, HIGH);
        if (this->_currentAddress <= this->_stack->_topAddress){
            if (this->_currentAddress == this->_stack->_topAddress) {
                this->_hasNext = false;
            } else {
                this->_currentAddress++;
            }
        }
    } else {
        this->_stack->_setSramStatus('v');
        this->_stack->_beginTransaction();
        digitalWrite(this->_stack->_sramSelect, LOW);
        this->_stack->_transfer(this->_stack->_sramReadData);
        this->_stack->_transfer((byte) (this->_currentAddress >> 8));
        this->_stack->_transfer((byte) this->_currentAddress);
        cellContent = this->_stack->_transfer16(0xFFFF);
        digitalWrite(this->_stack->_sramSelect, HIGH);
        if (this->_currentAddress <= this->_stack->_topAddress){
            if (this->_currentAddress == this->_stack->_topAddress) {
                this->_hasNext = false;
            } else {
                this->_currentAddress = this->_currentAddress + 2;
            }
        }
    }
    return cellContent;
}
    // ---- end: public methods of StackIterator -----
    // ---- start: private methods of StackIterator -----
    // ---- end: private methods of StackIterator -----

// ----- end: Implementation of SramStack -----
    