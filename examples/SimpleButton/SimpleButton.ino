/*
   Example for using the buttons library. To use this example
   upload it to an Arduino or ESP8266 device of your choice.
   There should be at least one button attached to a pin of
   your choice, connected to ground (0V).

   Open the Serial monitor after uploading this example to
   see the button handler response. Set it to 9600 baud.

   This example code is in the public domain.

*/
#include "buttons.h"

// Wire the device with a button attached to a single digital
// pin, connecting it to ground when pressed. Change the line
// below to match the pin used.
#define BUTTON_PIN   D4

// Define a unique id for the button for easy reference. This
// may be any value in the range 0x00..0xFF hexadecimal. If
// you don't know what to use, just leave it at 0x00. If you
// add more than one button, just choose another number for
// each subsequent button.
#define BUTTON_ID    0x00

// The Buttons object contains the button handlers.
Buttons buttons;

void setup() {

  // Initialize the serial port.
  Serial.begin(9600);
  Serial.println("Press the button....");

  // Initialize the button to respond to default options. The default options are:
  // BUTTON_OPTION_SHORT   Press short and release.
  // BUTTON_OPTION_LONG    Press for more than 3 seconds and release.
  // BUTTON_OPTION_LONGEST Press for more than 10 seconds.
  buttons.addButton(BUTTON_ID, buttonAction, BUTTON_PIN);

}

// The hanler for the button. Parameters are:
// id         The button id, as defined in the setup routine above.
// pressType  The type of action detected.
void buttonAction(byte id, byte pressType) {
  Serial.print("Button_id: ");
  Serial.print(id);
  Serial.print(", Press-type: ");
  switch (pressType) {
    case BUTTON_PRESS_SHORT:
      Serial.println("BUTTON_PRESS_SHORT");
      break;
    case BUTTON_PRESS_LONG:
      Serial.println("BUTTON_PRESS_LONG");
      break;
    case BUTTON_PRESS_LONGEST:
      Serial.println("BUTTON_PRESS_LONGEST");
      break;
    default:
      Serial.println("Unknown");
  }
}

void loop() {

  // The loop function in non blocking. It shoud be called frequently and just
  // transfers control to the defined handler(s) when triggered by the button
  // press.
  buttons.loop();

}
