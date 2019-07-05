/*
* Author: Wim Vissers
* The purpose of this library is to have an easy way to repond to buttons.
*/
#ifndef buttons_h
#define buttons_h

#include <Arduino.h>

#define BUTTON_OPTION_SHORT     0x01  // React to button short press.
#define BUTTON_OPTION_LONG      0x02  // React to button long press.
#define BUTTON_OPTION_LONGEST   0x04  // React to button extra long press.
#define BUTTON_OPTION_PRESS     0x08  // React to press instantly.
#define BUTTON_OPTION_RELEASE   0x10  // React to release instantly.
#define BUTTON_OPTION_DOUBLE    0x20  // React to double press.
#define BUTTON_OPTION_REPEAT    0x40  // Repeat short press when holding.
#define BUTTON_OPTION_IDLE_LOW  0x80  // Idle button state LOW instead of HIGH.
#define DEFAULT_OPTIONS         BUTTON_OPTION_SHORT + BUTTON_OPTION_LONG + BUTTON_OPTION_DOUBLE

#define BUTTON_PRESS_SHORT      0x01  // Button short press/release.
#define BUTTON_PRESS_LONG       0x02  // Button long pressed.
#define BUTTON_PRESS_LONGEST    0x03  // Button longest pressed.
#define BUTTON_PRESS_DOUBLE     0x04  // Button double short press/release.
#define BUTTON_PRESSED          0x05  // Button pressed.
#define BUTTON_RELEASED         0x06  // Button released.

#define STATE_FLAG_DOUBLE       0x01  // Double click event triggered.
#define STATE_FLAG_REPEAT       0x02  // Repeating events.

#define DEFAULT_DEBOUNCE 10           // Debounce period in #of ticks.
#define DEFAULT_TICK 5                // Tick time (ms).
#define DEFAULT_LONG_MILLIS     3000  // Time in ms for long buttonpress.
#define DEFAULT_LONGEST_MILLIS  10000 // Time in ms for BUTTON_PRESS_LONGEST buttonpress.
#define DEFAULT_HOLDOFF         120   // Min time between PRESSED / RELEASED.
#define DEFAULT_DOUBLEPRESS     370   // Time for double press detection.      
#define DEFAULT_REPEAT          200   // Time to repeat when holding down.       

#ifdef ESP8266
#include <functional>
#define BUTTON_CALLBACK_SIGNATURE std::function<void(byte, byte)> callback
#else
#define BUTTON_CALLBACK_SIGNATURE void (*callback)(byte, byte)
#endif

struct ButtonSettings
{
    unsigned long longpress;    // # of ms for long button press.
    unsigned long longestpress; // # of ms for longest button press.
    unsigned long tick;         // # of ms for 1 tick.
    byte debounce;              // # of ticks to debounce.
    unsigned long holdoff;      // # of ms between PRESSED/RELEASED.
    unsigned long doublepress;  // # of ms before detection of double press.
    unsigned long repeat;       // # of ms between repeating events when holding.
};

class Button
{
  private:
    BUTTON_CALLBACK_SIGNATURE;
    Button *_previous;
    unsigned long _lastChk;      // Last check (millis()).
    unsigned long _lastPress;    // Last button active state.
    unsigned long _lastChange;   // Last button state change.
    byte _lastState;
    byte _id;                    // Button id.
    byte _pin;                   // Hardware pin no.
    byte _state;                 // Current button state HIGH or LOW.
    byte _idleState;             // Idle button state HIGH or LOW.
    byte _cnt;                   // Debounce counter.
    byte _options;               // Mask out options.
    byte _flags;                 // Internal state flags.
    ButtonSettings *_settings;   // The buttonsettings.
  public:
    Button(BUTTON_CALLBACK_SIGNATURE, Button *previous, ButtonSettings *settings, const byte id, const byte pin, const byte idleState, const byte options);
    Button &setCallback(BUTTON_CALLBACK_SIGNATURE);
    Button *getPrevious();
    boolean execute(unsigned long currentMillis);
    boolean isReleased();
    byte getId();
};

class Buttons
{
  private:
    Button *_buttons;
    ButtonSettings _settings;
  public:
    Buttons();
    boolean addButton(const byte id, BUTTON_CALLBACK_SIGNATURE, const byte pin, const byte options);
    boolean addButton(const byte id, BUTTON_CALLBACK_SIGNATURE, const byte pin);
    ButtonSettings *getSettings();
    boolean loop();
    boolean isReleased(const byte id);
};

#endif
