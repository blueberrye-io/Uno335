/**
 * BB_SramStack.h - Library which provides the basic functionality to the SRAM as 
 * a LIFO (Last In First Out) stack.
 * 
 * The stack can be used in two modi:
 *    1. "byte mode": one stack cell contains one byte. The max. stack capacity is 64K stack cells.
 *    2. "word mode": one stack cell contains one word. The max. stack capacity is 32K stack cells.
 *
 * The major methods of a typical stack implementation are provided:
 *    push  -> write new data to the stack
 *    pop() -> return the last data which was written to the stack and remove 
 *             it from the stack
 *    peek() -> return the last data which was written to the stack without
 *              removing it
 * 
 * The constructor allows the definition of the stack mode, of a starting address and a stack size.
 *
 * The SRAM has to be initialized in the setup() section using
 *    SramStack::begin();
 *
 * Additionally, stack objects can deliver an iterator object which allow to iterate
 * all elements of the stack from the first element inserted to the last element
 * inserted. After instantiation using the SramStack method iterator(), the
 * iterator refers to the first element of the stack. The major iterator methods 
 * are provided:
 *     hasNext() -> true if the stack has an element which can be received by next()
 *     next() -> provides the data which is stored on the stack address to
 *               which the iterator is currently referring to
 *
 * The parts which cover the SPI communication are taken from the SPI library.
 * 
 * Created by E. Mittermeier, BlueberryE GmbH, Aug. 26, 2015.
 * Released into the public domain.
 * 
 * !!!!!TODO: THINK ABOUT THE FOLLOWING NOTE in the SPI lib.!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * All AVR based boards have an SS pin that is useful when they act as a slave controlled by an external master.
 * Since this library supports only master mode, this pin should be set always as OUTPUT otherwise the SPI interface
 * could be put automatically into slave mode by hardware, rendering the library inoperative.
 * Schau mal in void SramStack::begin()  -> dort wird pinMode(_sramSelect, OUTPUT); gesetzt!!!
 * Der PIN SS kann nur noch als general purpose output port genutzt werden, wenn diese Lib eingesetzt wird!!!!!
 * 
**/

#ifndef BB_SramStack_h
#define BB_SramStack_h

#include "Arduino.h"

class BB_StackIterator;
    
/**
 * BB_SramStack objects allow the usage of (parts of) the Serial SRAM as stack.
 * The stack has to be defined either as "byte mode stack" or as "word mode stack".
 * In byte mode, each cell of the stack contains one byte (8bit) of data. The maximum stack capacity is 64K bytes.
 * In word mode, each cell of the stack contains one word (16bit) of data. The maximum stack capacity is 32K words.
**/
class BB_SramStack
{
    public:
        /**
         * Initializes the serial SRAM. This static method needs to be called in the setup() of the sketch.
        **/
        static void begin();
    
        /**
         * Initiates a SramStack object which uses the full capacity (64K bytes) of the serial SRAM.
         * Each stack cell contains one byte (8bit) of data ("byte mode"). 
        **/
        BB_SramStack();

        /**
         * Initiates a SramStack object which uses the full capacity (64K bytes or 32K words) of the serial SRAM.
         * "mode" defines the capacity (byte or word) of the stack cells.
         * @param mode if 'b'; each stack cell contains one byte (8 bit) of data. The capacity of the stack is 64K bytes.
         *             if 'w': each stack cell contains one word (16bit) of data. The capacity of the stack is 32K words.
         *             else: same as 'b'
        **/
        BB_SramStack(char inMode);
        
        /**
         * Initiates a SramStack object which uses the defined capacity starting at a defined SRAM address.
         * Each stack cell contains one byte one data ("byte mode").
         * If the starting address + size > physical SRAM capacity , the stack object will be flagged as full, i.e. it cannot be used. 
         * @param startAddress the 16bit address of the first memory cell of the stack.
         * @param size the amount of stack cells.
        **/
        BB_SramStack(word startAddress, unsigned long size);
    
        /**
         * Initiates a SramStack object which uses the defined capacity starting at a defined SRAM address.
         * If the starting address + size > physical SRAM capacity , the stack object will be flagged as full, i.e. it cannot be used. 
         * @param mode if 'b'; the stack cells contain one byte of data.
         *             if 'w': the stack cells contain one word of data.
         *             else: same as 'b'
         * @param startAddress the 16bit address of the first memory cell of the stack.
         * @param size the amount of stack cells.
        **/
        BB_SramStack(char mode, word startAddress, unsigned long size);
        
        /**
         * Checks if the stack is empty.
         * @return true if the stack has no elements
         *         false else
        **/
        boolean isEmpty();
    
        /**
         * Checks if the stack is completely filled with data.
         * @return true if all cells of the stack are filled with valid data
         *         false else
        **/
        boolean isFull();
    
        //word getCapacity(); // returns the total capacity of the stack (i.e. memSize)
    
        /**
         * return the content of the last data which was written to the stack and remove 
         * it from the stack. In byte mode, the first 8 bits are not valid.
         * @return the content of the top stack cell.
        **/
        word pop();
        
        /**
         * return the content of the last data which was written to the stack.
         * In byte mode, the first 8 bit will be 0.
         * @return the content of the top stack cell.
        **/
        word peek();
    
        /**
         * Puts one byte of data on top of the stack. In word mode, inData will be extended to
         * a word with leading 0s.
         * @param inData the 8bit bit pattern which will be put on top of the stack.
         * @return 0 if the data could be written onto the stack
         *         >0 if the data could not be written because the stack is full.
        **/
        byte push(byte inData);

        /**
         * Puts one word of data on top of the stack. In byte mode, only the last 8 bit will be stored.
         * @param inData the 16bit bit pattern which will be put on top of the stack.
         * @return 0 if the data could be written onto the stack
         *         >0 if the data could not be written because the stack is full.
        **/
        byte push(word inData);
        
        /**
         * Resets the stack, i.e. isEmpty() will be true after this operation.
        **/
        void clear();
    
        /**
         * Creates and returns a StackIterator which can be used to iterate from the 
         * first to the last element of the the stack.
         * @return an iterator refering to the first stack element.
        **/
        BB_StackIterator iterator();
    
    private:
        boolean _byteMode; /* true if the stack cells have a capacity of 1 byte */
        unsigned long _size; /* the stack capacity in Bytes */
        word _startAddress; /* the first valid address of the stack */
        word _topAddress; /* if not _isEmpty: he current address which contains valid data */
        boolean _isEmpty; /* true if the stack does not contain any data */
        boolean _isFull; /* true if last cell of the stack contains valid data */
        
        static uint8_t _initialized; /* counts the number of begin() calls */
    
        static const int _sramSelect; /* the pin used for the chip select signal */
        static const byte _sramWriteData; /* the serial Sram command for writing data into the SRAM */
        static const byte _sramReadData; /* the serial Sram command for reading data from the SRAM */
        static const byte _sramWriteStatus; /* the serial Sram command for setting the status register of the serial SRAM */
        static const byte _sramReadStatus; /* the serial Sram command for reading the content of the status register of the serial SRAM */
        static const unsigned long _maxSramCapacity; /* the physically available SRAM capacity in bytes (currently 0x10000) */
      
        static const uint8_t _spiSettingsSpcr; /* the bit pattern which is needed for the SPI SPCR (control) register */
        static const uint8_t _spiSettingsSpsr; /* the bit pattern which is needed for the SPI SPSR (status) register */

        /**
         * Set the content of the status register of the serial SRAM.
         * Available modes are: byte mode, page mode, page sequential mode, virtual chip mode.
         * Currently only byte mode is supported.
        * @param inMode 'b' sets the serial SRAM to byte mode
        **/
        static void _setSramStatus(char inMode); /* set the content of the status register of the serial SRAM */
    
      
        /**
         * Initializes a communication on the SPI bus.
        **/
        static void _beginTransaction();
      
        // static void _endTransaction(); // not needed, because this lib does not work with interrupts anyway
      
        /**
         * Transfers one byte of data on the SPI bus.
        **/
        static byte _transfer(byte data);
    
        /**
         * Transfers one word (2 bytes) of data on the SPI bus.
        **/
        static word _transfer16(word data);
        
        friend class BB_StackIterator;
};

/**
 * BB_SramIterator objects can be used to iterate over all elements of a BB_SramStack object.
**/
class BB_StackIterator
{
    public:
        /**
         * Initiates a BB_StackIterator object which can be used to iterate over all elements of a 
         * BB_SramStack object.
         * After instantiation, the BB_StackIterator object refers to the first element of the stack.
         * Note: Do not use this constructor!! Use the iterator() method of a BB_SramStack object instead!!
         * @param *stack a reference to the stack over which the iterator will iterate.
        **/
        BB_StackIterator(BB_SramStack *stack);
        
        /**
         * Checks if the stack has an element which can be received by next().
         * @return true if the stack has an element which can be received by next().
        **/
        boolean hasNext();
        
        /**
         * Provides the data which is stored on the stack address to which the iterator is 
         * currently referring to. If the stack is in byte mode, the first 8 bits are not valid. 
         * @return the current element on the stack to which the iterator is referring to.
        **/
        word next();
    
    private:
        BB_SramStack *_stack;  /* a reference to the stack for which the iterator is used */
        word _currentAddress; /* the current position on the stack */
        boolean _hasNext; /* indicates if the next element can be received using next() */
        
};

#endif