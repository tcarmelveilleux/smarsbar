/**
 * @file LineFollowerController.cpp
 * @short Basic Logic for line follower bot
 *
 * @date 2018-02-27
 * @author Tennessee Carmel-Veilleux <tcv -at- ro.boto.ca>
 * @copyright 2018 Tennessee-Carmel-Veilleux
 *
 * See LICENSE file. 
 */

#include <stdio.h>
#include "LineFollowerController.hpp"

#define DARK_THRESH 700

LineFollowerController::LineFollowerController(UnipolarSequencer &leftSeq, UnipolarSequencer &rightSeq, 
                           int leftIrSensor, int rightIrSensor,
                           int32_t kpMillis, uint32_t schedPeriodUs):
    _leftSeq(leftSeq),
    _rightSeq(rightSeq),
    _leftIrSensor(leftIrSensor),
    _rightIrSensor(rightIrSensor),
    _kpMillis(kpMillis),
    _schedPeriodUs(schedPeriodUs),
    _alignmentError(0),
    _maxSpeedLeftPps(leftSeq.get_speed_max_pps()),
    _maxSpeedRightPps(rightSeq.get_speed_max_pps()),
    _rightIrAdc(0),
    _leftIrAdc(0),
    _lastSeenWasLeft(false),
    _speedLeft(0),
    _speedRight(0)
{

}

void LineFollowerController::_measureError(void) {
    // Get reflectance from an open-collector-based IR sensor.
    // ADC: 0=full reflectance, transform to 1023 reflectance
    //      1023=no reflectance, transform to 0 reflectance
    _leftIrAdc = (1023 - analogRead(_leftIrSensor));
    bool leftDark = _leftIrAdc <= DARK_THRESH;

    _rightIrAdc = (1023 - analogRead(_rightIrSensor));
    bool rightDark = _rightIrAdc <= DARK_THRESH;

    // Keep state about which single sensor was the last seen to go back
    // to it when we lose track completely.
    if (leftDark) {
        _lastSeenWasLeft = true;
    } else if (rightDark) {
        _lastSeenWasLeft = false;
    }

    // Negative error means go left
    // Positive error means go right
    _alignmentError = (((int32_t)_rightIrAdc * 1000) / 1023) -
                       (((int32_t)_leftIrAdc * 1000) / 1023);
}

void LineFollowerController::_setMotorSpeeds(void) {
    int32_t baseSpeedLeft = 2 * _maxSpeedLeftPps / 3;
    int32_t baseSpeedRight = 2 * _maxSpeedRightPps / 3;

    int32_t _speedLeft = baseSpeedLeft + (((1000 * _alignmentError) * _kpMillis) / 1000);
    if (_speedLeft > 32767) _speedLeft = 32767;
    if (_speedLeft < -32768) _speedLeft = -32768;

    int32_t _speedRight = baseSpeedRight - (((1000 * _alignmentError) * _kpMillis) / 1000);
    if (_speedRight > 32767) _speedRight = 32767;
    if (_speedRight < -32768) _speedRight = -32768;

    _leftSeq.set_speed_pps((int16_t)_speedLeft);
    _rightSeq.set_speed_pps((int16_t)_speedRight);
}

void LineFollowerController::_dumpStateToSerial(void) {
    char serial_buf[128];
    snprintf(&serial_buf[0], sizeof(serial_buf), "%u,%u,%ld,%ld,%ld\n", _leftIrAdc, _rightIrAdc, _speedLeft, _speedRight, _alignmentError);
    Serial.write(serial_buf);
}

void LineFollowerController::schedulerKick(void) {
    _measureError();
    _setMotorSpeeds();
    _dumpStateToSerial();
}
