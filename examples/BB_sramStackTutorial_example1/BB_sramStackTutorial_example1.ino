/*
  Store 4 digits of Eulers number e in the Uno335 Serial Sram.
  This sketch uses the BB_SramStack library

  created 10 Aug. 2016
  by Engelbert Mittermeier (BlueberryE GmbH)
*/

#include <BB_SramStack.h> // include the library

// Instantiate a stack object
// the stack will be in byte mode and will use the total SRAM capacity
BB_SramStack stack8_1;
   
void setup() {
    Serial.begin(9600);
     BB_SramStack::begin();
}
  
void loop() {
    // Check the status of the stack
    Serial.print("Stack is empty = ");
    Serial.println(stack8_1.isEmpty());
    Serial.print("Stack is full = ");
    Serial.println(stack8_1.isFull());
    Serial.println();
  
    Serial.println("Writing 2.718 to the sram\n");
    stack8_1.push((byte) '2');
    stack8_1.push((byte) '.');
    stack8_1.push((byte) '7');
    stack8_1.push((byte) '1');
    stack8_1.push((byte) '8');

    // Check the status of the stack
    Serial.print("Stack is empty = ");
    Serial.println(stack8_1.isEmpty());
    Serial.print("Stack is full = ");
    Serial.println(stack8_1.isFull());
    Serial.println();

    // Reading the content of the top cell of the stack using peek()
    Serial.print("Content of the last cell of the Stack = ");
    Serial.println((char) stack8_1.peek());
    Serial.println();

    // Read the content of the stack using an iterator
    Serial.println("Reading the content of the stack using an iterator");
    Serial.println("Sequence is: from first element to the last element");
    BB_StackIterator iterStack8_1 = stack8_1.iterator();
    Serial.print("e = ");
    while (iterStack8_1.hasNext()){
        Serial.print((char) iterStack8_1.next());
    }
    Serial.println();

    // Check the status of the stack
    Serial.print("Stack is empty = ");
    Serial.println(stack8_1.isEmpty());
    Serial.println();

    // Read the content of the stack using pop()
    Serial.println("Reading the content of the stack using pop()");
    Serial.println("Sequence is: from last element to the first element");
    while(!stack8_1.isEmpty()) Serial.println((char) stack8_1.pop());
    
    // Check the status of the stack
    Serial.print("Stack is empty = ");
    Serial.println(stack8_1.isEmpty());
    Serial.println();

    Serial.println("-------------------------------");
      delay(3000);
}
