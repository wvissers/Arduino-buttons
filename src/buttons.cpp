/*
* Author: Wim Vissers
* The purpose of this library is to have an easy way to repond to buttons.
*/
#include "Arduino.h"
#include "buttons.h"

Button::Button(BUTTON_CALLBACK_SIGNATURE, Button *previous, ButtonSettings *settings, const byte id, const byte pin, const byte idleState, const byte options) : _id(id), _pin(pin), _idleState(idleState), _options(options), _settings(settings), _flags(0)
{
    this->callback = callback;
    this->_previous = previous;
    this->_lastChk = millis() + _settings->longpress;
    this->_lastPress = millis() + _settings->longpress;
    this->_lastChange = millis();
    this->_cnt = _settings->debounce + 1;
    this->_state = digitalRead(_pin);
    this->_lastState = this->_state;
}

Button &Button::setCallback(BUTTON_CALLBACK_SIGNATURE)
{
    this->callback = callback;
    return *this;
}

boolean Button::execute(unsigned long currentMillis)
{
    if ((currentMillis - _lastChk) > _settings->tick)
    {
        _lastChk = currentMillis;
        byte btn = digitalRead(_pin);
        if (_state != btn)
        {
            // State changed
            _cnt = 0;
            _state = btn;
            if (_state != _idleState)
            {
                _lastPress = currentMillis;
            }
        }
        else if (_cnt < _settings->debounce)
        {
            // In debounce period
            _cnt++;
        }
        else if (_cnt == _settings->debounce)
        {
            // Perform action
            if ((currentMillis - _lastChange) > _settings->holdoff)
            {
                // Debounced, state changed and after holdoff
                if (_state != _lastState)
                {
                    if (_state == _idleState)
                    {
                        if (_options & BUTTON_OPTION_RELEASE)
                        {
                            callback(_id, BUTTON_RELEASED);
                        }
                        _flags = _flags & ~STATE_FLAG_REPEAT;
                    }
                    else
                    {
                        if (_options & BUTTON_OPTION_PRESS)
                        {
                            callback(_id, BUTTON_PRESSED);
                        }
                        // Now check for double click, if enabled
                        if (_options & BUTTON_OPTION_DOUBLE)
                        {
                            if ((currentMillis - _lastChange) < _settings->doublepress)
                            {
                                callback(_id, BUTTON_PRESS_DOUBLE);
                                _flags = _flags | STATE_FLAG_DOUBLE;
                                _cnt++;
                            }
                        }
                    }
                    _lastState = _state;
                    _lastChange = currentMillis;
                }
            }
            if (_state == _idleState && (currentMillis - _lastPress) < _settings->longestpress)
            {
                // Check for expiry of double check time
                if (_options & BUTTON_OPTION_DOUBLE)
                {
                    if ((currentMillis - _lastPress) > _settings->doublepress)
                    {
                        if (!(_flags & STATE_FLAG_DOUBLE) && (_options & BUTTON_OPTION_SHORT))
                        {
                            if ((currentMillis - _lastPress) < _settings->longpress)
                            {
                                callback(_id, BUTTON_PRESS_SHORT);
                            }
                        }
                        _cnt++;
                        _flags = _flags & ~STATE_FLAG_DOUBLE;
                    }
                }
                else if ((_options & BUTTON_OPTION_SHORT) && (currentMillis - _lastPress) < _settings->longpress)
                {
                    // Short press when idle
                    callback(_id, BUTTON_PRESS_SHORT);
                    _cnt++;
                }
                if ((_options & BUTTON_OPTION_LONG) && (currentMillis - _lastPress) > _settings->longpress)
                {
                    // Long press when idle
                    callback(_id, BUTTON_PRESS_LONG);
                    _cnt++;
                }
            }
            else
            {
                if (_state != _idleState)
                {
                    // Check for long press
                    if ((currentMillis - _lastPress) > _settings->longestpress)
                    {
                        if (_options & BUTTON_OPTION_LONGEST)
                        {
                            callback(_id, BUTTON_PRESS_LONGEST);
                            // Only call it once, and inhibit next checks by incrementing cnt
                        }
                        if (_options & BUTTON_OPTION_REPEAT)
                        {
                            _flags = _flags | STATE_FLAG_REPEAT;
                        }
                        // Only call it once, and inhibit next checks by incrementing cnt
                        _cnt++;
                    }
                    else if ((currentMillis - _lastPress) > _settings->longpress)
                    {
                        if (_options & BUTTON_OPTION_REPEAT)
                        {
                            callback(_id, BUTTON_PRESS_SHORT);
                            _flags = _flags | STATE_FLAG_REPEAT;
                            _cnt++;
                        }
                    }
                }
            }
        }
        else if ((_state != _idleState) && (_flags & STATE_FLAG_REPEAT))
        {
            if ((currentMillis - _lastPress) > _settings->repeat)
            {
                callback(_id, BUTTON_PRESS_SHORT);
                _lastPress = currentMillis;
            }
        }
    }
    return true;
}

boolean Button::isReleased()
{
    return digitalRead(_pin) == this->_idleState;
}

byte Button::getId()
{
    return _id;
}

Button *Button::getPrevious()
{
    return _previous;
}

Buttons::Buttons()
{
    this->_buttons = NULL;
    this->_settings = {DEFAULT_LONG_MILLIS, DEFAULT_LONGEST_MILLIS, DEFAULT_TICK, DEFAULT_DEBOUNCE, DEFAULT_HOLDOFF, DEFAULT_DOUBLEPRESS, DEFAULT_REPEAT};
}

boolean Buttons::loop()
{
    Button *button = _buttons;
    unsigned long currentMillis = millis();
    while (button)
    {
        button->execute(currentMillis);
        button = button->getPrevious();
    }
    return true;
}

boolean Buttons::addButton(const byte id, BUTTON_CALLBACK_SIGNATURE, const byte pin, const byte options)
{
    byte state = (options & BUTTON_OPTION_IDLE_LOW) ? LOW : HIGH;
    Button *button = new Button(callback, _buttons, &_settings, id, pin, state, options);
    _buttons = button;
    pinMode(pin, INPUT_PULLUP);
    return true;
}

boolean Buttons::addButton(const byte id, BUTTON_CALLBACK_SIGNATURE, const byte pin)
{
    return addButton(id, callback, pin, DEFAULT_OPTIONS);
}

boolean Buttons::isReleased(const byte id)
{
    Button *button = _buttons;
    while (button)
    {
        if (button->getId() == id)
        {
            return button->isReleased();
        }
        button = button->getPrevious();
    }
    return true;
}

ButtonSettings *Buttons::getSettings()
{
    return &_settings;
}
