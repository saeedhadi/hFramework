/**
 * Copyright (c) 2013-2017 Husarion Sp. z o.o.
 * Distributed under the MIT license.
 * For full terms see the file LICENSE.md.
 */

#define NO_USING_HFRAMEWORK
#include <hFramework.h>

// bases
const int HEX = 16;
const int DEC = 10;
const int OCT = 8;
const int BIN = 2;

// digital states
const int LOW = 0;
const int HIGH = 1;

// pin modes
const int INPUT = 1;
const int OUTPUT = 2;
const int INPUT_PULLUP = 3;

const int A0 = 40;
const int A1 = 41;
const int A2 = 42;
const int A3 = 43;
const int A4 = 44;
const int A5 = 45;
const int A6 = 46;
const int A7 = 47;
const int A8 = 48;
const int A9 = 49;
const int A10 = 50;

namespace hFramework {
#if BOARD(CORE2)
static auto pins = std::make_tuple(hExt.serial.pinRx, hExt.serial.pinTx,
                                   hExt.spi.pinSck, hExt.spi.pinMiso, hExt.spi.pinMosi,
                                   hExt.i2c.pinSda, hExt.i2c.pinScl,
                                   0, 0, 0, 0, 0, 0,
                                   hSens1.pin2, hSens1.pin3, hSens1.pin4, hSens2.pin2,
                                   hSens2.pin3, hSens2.pin4, hSens3.pin2, hSens3.pin3,
                                   hSens3.pin4, hSens4.pin2, hSens4.pin3, hSens4.pin4,
                                   hSens5.pin2, hSens5.pin3, hSens5.pin4, hSens6.pin2,
                                   hSens6.pin3, hSens6.pin4,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   hExt.pin1, hExt.pin2, hExt.pin3, hExt.pin4, hExt.pin5,
                                   hSens1.pin1, hSens2.pin1, hSens3.pin1,
                                   hSens4.pin1, hSens5.pin1, hSens6.pin1);
#define H_DIGITAL_PINS 0, 1, 2, 3, 4, 5, 6, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30
#define H_ANALOG_PINS A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10
#else
#define H_DIGITAL_PINS
#define H_ANALOG_PINS
static auto pins = std::make_tuple();
#endif

template <typename T>
__attribute__((noreturn)) T& getPin(int index, const char* error) {
    sys.fail_log("ERROR: pin %d doesn't support %s", index, error);
    sys.fault_handler();
    abort();
}

template <typename T, int pin, int... allowed>
T& getPin(int index, const char* error) { // this should result in unrolled loop
    if (index == pin) {
        return std::get<pin>(pins);
    } else {
        return getPin<T, allowed...>(index, error);
    }
}

void digitalWrite(int pinIndex, int value) {
    hGPIO& gpio = getPin<hGPIO, H_DIGITAL_PINS>(pinIndex, "digitalWrite");
    gpio.write(value);
}

int digitalRead(int pinIndex) {
    hGPIO& gpio = getPin<hGPIO, H_DIGITAL_PINS>(pinIndex, "digitalRead");
    return gpio.read();
}

int analogRead(int pinIndex) {
    hGPIO_adc& gpio = getPin<hGPIO_adc, H_ANALOG_PINS>(pinIndex, "analogRead");
    gpio.enableADC();
    return gpio.analogReadRaw() / 4; // 0..4095 -> 0..1023
}

void pinMode(int pinIndex, int value) {
    hGPIO& gpio = getPin<hGPIO, H_DIGITAL_PINS>(pinIndex, "pinMode");
    switch (value) {
    case INPUT:
        gpio.setIn();
        break;
    case OUTPUT:
        gpio.setOut();
        break;
    case INPUT_PULLUP:
        gpio.setIn_pu();
        break;
    default:
        sys.fail_log("ERROR: invalid pin mode %d", value);
        sys.fault_handler();
    }
}

#undef H_DIGITAL_PINS
#undef H_ANALOG_PINS

struct ArduinoSerial {
    hSerial& hserial;

    ArduinoSerial(hSerial& s): hserial(s) {}

    int available() {
        return hserial.available();
    }

    void begin(int baudrate) {
        hserial.init(baudrate);
    }

    void print(int num, int base=10) {
        switch (base) {
        case 2:
            hserial.printf("%b", num);
            break;
        case 8:
            hserial.printf("%o", num);
            break;
        case 10:
            hserial.printf("%d", num);
            break;
        case 16:
            hserial.printf("%x", num);
            break;
        }
    }

    void print(double num) {
        hserial.printf("%f", num);
    }

    void print(char c) {
        hserial.printf("%c", c);
    }

    void print(const char* s) {
        hserial.printf("%s", s);
    }

    operator bool() {
        return true;
    }
};

void delay(int ms) {
    sys.delay(ms);
}

}

void setup();
void loop();

hFramework::ArduinoSerial Serial(hFramework::Serial);

using hFramework::digitalWrite;
using hFramework::digitalRead;
using hFramework::analogRead;
using hFramework::pinMode;
