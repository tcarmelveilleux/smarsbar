/**
 * @file board_config.h
 * @short
 *
 * @date Feb 17, 2018
 * @author Tennessee Carmel-Veilleux <tcv -at- ro.boto.ca>
 * @copyright 2018 Tennessee Carmel-Veilleux
 * 
 * SEE LICENSE FILE
 */

#ifndef BOARD_CONFIG_H_
#define BOARD_CONFIG_H_

#include <Arduino.h>

#define BOARD_IS_SMARSBAR_V1

#ifdef BOARD_IS_SMARSBAR_V1
// smarsbar v1 board. Two DS3658 unipolar drivers,
// 3 channels for QRE1113 IR sensor, 1 free GPIO

// Digital GPIOs for two unipolar steppers
#define BOARD_MOTOR1_PH_A 16
#define BOARD_MOTOR1_PH_B 19
#define BOARD_MOTOR1_PH_C 24
#define BOARD_MOTOR1_PH_D 23

#define BOARD_MOTOR2_PH_A 12
#define BOARD_MOTOR2_PH_B 11
#define BOARD_MOTOR2_PH_C 10
#define BOARD_MOTOR2_PH_D 9

// Analog GPIOs for IR sensors
#define BOARD_IR_CH1 A0
#define BOARD_IR_CH2 A1
#define BOARD_IR_CH3 A2

// I2C on #21/#20
#define BOARD_I2C_SCL 21
#define BOARD_I2C_SDA 20

// UART on #1/#0
#define BOARD_UART_TX 1
#define BOARD_UART_RX 0

// LED on #13
#define BOARD_LED 13

static void setup_board(void) {
    pinMode(BOARD_LED, OUTPUT);

    // Set all unipolar pins as output, initially low
    pinMode(BOARD_MOTOR1_PH_A, OUTPUT);
    digitalWrite(BOARD_MOTOR1_PH_A, LOW);
    pinMode(BOARD_MOTOR1_PH_B, OUTPUT);
    digitalWrite(BOARD_MOTOR1_PH_B, LOW);
    pinMode(BOARD_MOTOR1_PH_C, OUTPUT);
    digitalWrite(BOARD_MOTOR1_PH_C, LOW);
    pinMode(BOARD_MOTOR1_PH_D, OUTPUT);
    digitalWrite(BOARD_MOTOR1_PH_D, LOW);

    pinMode(BOARD_MOTOR2_PH_A, OUTPUT);
    digitalWrite(BOARD_MOTOR2_PH_A, LOW);
    pinMode(BOARD_MOTOR2_PH_B, OUTPUT);
    digitalWrite(BOARD_MOTOR2_PH_B, LOW);
    pinMode(BOARD_MOTOR2_PH_C, OUTPUT);
    digitalWrite(BOARD_MOTOR2_PH_C, LOW);
    pinMode(BOARD_MOTOR2_PH_D, OUTPUT);
    digitalWrite(BOARD_MOTOR2_PH_D, LOW);

    // TODO: Init I2C/UART later
}
#else
#error Unsupported board!
#endif

#endif /* BOARD_CONFIG_H_ */
