/*
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of
 * the network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2017 Sensnology AB
 * Full contributor list:
 * https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */
#ifndef SensorInterruptedButton_h
#define SensorInterruptedButton_h

/*
SensorInterruptedButton: like a combination of SensorInterrupt and
SensorDigitalIn. it enables sleeping, will be awaken by interrupt, and can
either send the pin value or toggle the send value according to the pin.
Author: Weihua Yi <weihua.yi@gmail.com>
*/

class SensorInterruptedButton : public Sensor {
protected:
  bool _toggle = true;
  int8_t _read_pin = -1;
  int8_t _read_pin_initial_value = -1;

public:
  SensorInterruptedButton(int8_t interrupt_pin, int8_t read_pin = -1,
                          uint8_t child_id = 0)
    : Sensor(interrupt_pin) {
    if (read_pin > -1) {
      _read_pin = read_pin;
    } else {
      _read_pin = interrupt_pin;
    }
    _name = "InterruptedToggleButton";
    children.allocateBlocks(1);
    new Child(this, INT, nodeManager.getAvailableChildId(child_id), S_BINARY,
              V_STATUS, _name);
    setPinInitialValue(HIGH);
    setInterruptMode(FALLING);
    // setReadPinInitialValue(HIGH);
  };

  void setReadPinInitialValue(int8_t value) { _read_pin_initial_value = value; }

  void setToggle(bool toggle) { _toggle = toggle; }

  // define what to do during setup
  void onSetup() {
    // set the pin for input
    pinMode(_pin, INPUT_PULLUP);
    // set internal pull up/down
    if (_initial_value > -1)
      digitalWrite(_pin, _initial_value);

    if (_read_pin != _interrupt_pin) {
      pinMode(_read_pin, INPUT_PULLUP);
      // set internal pull up/down
      if (_read_pin_initial_value > -1)
        digitalWrite(_read_pin, _read_pin_initial_value);
    }
#if defined(CHIP_STM32_ALL)
    // ConfigInterrupt ci;
#endif // defined(CHIP_STM32_ALL)

    // do not average the value
    children.get()->setValueProcessing(NONE);
    // report immediately
    setReportTimerMode(IMMEDIATELY);
  };

    // what to do when receiving an interrupt
#if NODEMANAGER_INTERRUPTS == ON
  void onInterrupt() {
    Child *child = children.get(1);
    int value = !child->getLastValueInt();
    int read_pin_value;
    if (_read_pin != _interrupt_pin) {
      read_pin_value = digitalRead(_read_pin);
    } else {
      read_pin_value = nodeManager.getLastInterruptValue();
    }
    debug(PSTR("onInterrupt[%" PRIu8 "],value=%" PRIu8 "\n"),
          child->getChildId(), value);
    if (_toggle) {
      if (read_pin_value == LOW)
        child->setValue(value);
    } else {
      child->setValue(read_pin_value);
    }
  }
#endif
};
#endif