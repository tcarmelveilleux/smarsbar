# smarsbar
Arduino-based sample code for SMARS robot platform.

The smarsbar robot is based pn 28BYJ-48 unipolar stepper motors and QRE1113 IR line sensors.

The SMARS robot is designed by Kevin Thomas. You can find the SMARS robot files here:
- Main parts: https://www.thingiverse.com/thing:2662828
- 28BYJ-48 body mod: https://www.thingiverse.com/thing:2778904

The firmware targets the Adafruit Feather M0. It is based on Atmel SAM D21G (AKA "samd") with an ARM Cortex M0 and is likely compatible with the Arduino Zero, but I have not tried.

This early version is **not compatible with AVR-based Arduinos yet**, but it would be easy enough to adapt. The code expects to have certain timers to drive the steppers and I have written it to work with what I have. 

The main logic of the robot is portable and the project is scaffolded such that different boards or Arduinos should be easy to support.

## Hardware Setup

The current setup of the smarsbar robot is as follows:

- 2x [28BYJ-48 stepper motors](https://www.banggood.com/28YBJ-48-DC-5V-4-Phase-5-Wire-Stepper-Motor-With-ULN2003-Driver-Board-p-74397.html).
- 2x Stepper motor drivers (usually the 28BYJ-48 motors come with ULN2003A darlington drivers).
- 2x QRE1113 module, tied open collector and connected to ADCs. I used [Sparkfun ROB-09453 modules](https://www.sparkfun.com/products/9453).
- 1x [Adafruit Feather M0 basic proto](https://www.adafruit.com/product/2772) (similar to Arduino Zero).
- 1x 5V USB Li-Ion Cell phone power bank with at least 1A output
  - Used it to power motors and circuits. Handy and rechargeable.

No other parts are needed, you just need to hook it all up. Schematic to be provided later. If you can run Arduino code, you usually can hook this up.

## Pinouts for SMARS

TODO: WIRE COLORS

| Feather pin | Name in code | Function |
| ----------- | ---- | -------- |
| 16 | BOARD_MOTOR1_PH_A | Right motor phase A |
| 19 | BOARD_MOTOR1_PH_B | Right motor phase B |
| 24 | BOARD_MOTOR1_PH_C | Right motor phase C |
| 23 | BOARD_MOTOR1_PH_D | Right motor phase D |
| 12 | BOARD_MOTOR2_PH_A | Left motor phase A |
| 11 | BOARD_MOTOR2_PH_B | Left motor phase B |
| 10 | BOARD_MOTOR2_PH_C | Left motor phase C |
| 9  | BOARD_MOTOR2_PH_D | Left motor phase D |
| 13 | BOARD_LED         | LED for debug      |
| A0 | BOARD_IR_CH1      | Left IR sensor     |
| A1 | BOARD_IR_CH2      | Right IR sensor    |
| 21 | BOARD_I2C_SCL     | I2C expansion SCL (future use) |
| 20 | BOARD_I2C_SDA     | I2C expansion SDA (future use) |

## Building the code

To build the code with Arduino IDE, you must install some libraries. The libraries needed are in src/smarsbar/libs/libs.zip. You need to unzip this in your Arduino Libraries folder (on Windows: \<My Documents>\Arduino\libraries). See [Adafruit's All About Arduino Libraries tutorial](https://learn.adafruit.com/adafruit-all-about-arduino-libraries-install-use/arduino-libraries) for more details.

The libraries used are:
- [Adafruit_ZeroTimer](https://github.com/adafruit/Adafruit_ZeroTimer) (MODIFIED by me! If you already have it installed, be careful. The modifications are 100% backwards compatible though...).
- [Adafruit_ASFCore](https://github.com/adafruit/Adafruit_ASFcore). Required by Adafruit_ZeroTimer.
- [UnipolarSequencer](https://github.com/tcarmelveilleux/UnipolarSequencer). I made this library.

To install SAMD support for the Feather M0 in Arduino IDE, see [the tutorial on Adafruit's website](https://learn.adafruit.com/adafruit-feather-m0-basic-proto/setup).

Otherwise, just load and build smarsbar.ino. I tested the build under Visual Studio Code with Arduino plugin installed, and also in Arduino IDE 1.8.2.

## Basics of the code
- The main module `smarsbar.ino` sets up the peripherals and a scheduler loop running at 20Hz.
  - The steppers are driven by sequencer code that is running in TC3 and TC5 interrupts (using the Adafruit_ZeroTimer library).
  - The scheduler loop delegates all processing to `LineFollowerController.cpp::schedulerKick()`.
- The `LineFollowerController.cpp` module contains the sensing and line-following logic. It's a very very basic sample using a proportional controller on the error with quantized error function. It is not high performance, but requires very little calibration.

# TODO: Provide more details later


