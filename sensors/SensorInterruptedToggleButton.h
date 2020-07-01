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
#ifndef SensorInterruptedToggleButton_h
#define SensorInterruptedToggleButton_h

/*
SensorInterruptedToggleButton: like a combination of SensorInterrupt and
SensorDigitalIn. it enables sleeping, will be awaken by interrupt, and read the
toggle_pin to toggle the send value.
Author: Weihua Yi <weihua.yi@gmail.com>
*/

class SensorInterruptedToggleButton : public Sensor {
protected:
  int8_t _toggle_pin = -1;
  int8_t _initial_value_toggle = -1;

public:
  SensorInterruptedToggleButton(int8_t toggle_pin, int8_t interrupt_pin,
                                uint8_t child_id = 0)
      : Sensor(interrupt_pin) {
    _toggle_pin = toggle_pin;
    _name = "InterruptedToggleButton";
    children.allocateBlocks(1);
    new Child(this, INT, nodeManager.getAvailableChildId(child_id), S_BINARY,
              V_STATUS, _name);
    setPinInitialValue(HIGH);
    setInterruptMode(FALLING);
    setTogglePinInitialValue(LOW);
  };

  void setTogglePinInitialValue(int8_t value) { _initial_value_toggle = value; }

  // define what to do during setup
  void onSetup() {
    // set the pin for input
    pinMode(_pin, INPUT);
    pinMode(_toggle_pin, INPUT);
    // set internal pull up/down
    if (_initial_value_toggle > -1)
      digitalWrite(_toggle_pin, _initial_value_toggle);

    // do not average the value
    children.get()->setValueProcessing(NONE);
    // report immediately
    setReportTimerMode(IMMEDIATELY);
  };

    // what to do when receiving an interrupt
#if NODEMANAGER_INTERRUPTS == ON
  void onInterrupt() {
    Child *child = children.get(1);
    // read the value of the pin
    // int value = nodeManager.getLastInterruptValue();
    // int value = digitalRead(_pin);
    if (digitalRead(_toggle_pin) == LOW) {
      int value = !child->getLastValueInt();

      child->setValue(value);
      //   debug(PSTR("onInterrupt,%" PRIu8 "\n"), value);
    }
  }
#endif
};
#endif