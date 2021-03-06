/**
 * @file LineFollowerController.cpp
 * @short Basic Logic for line follower bot
 *
 * @date 2018-02-27
 * @author Tennessee Carmel-Veilleux <tcv -at- ro.boto.ca>
 * @copyright 2018 Tennessee-Carmel-Veilleux
 *
 * MIT License. See LICENSE file. 
 */

#include <stdio.h>
#include <math.h>
#include "LineFollowerController.hpp"

// Thresholds should be calibrated. The raw ADC values are the first and second values
// printed on each line, each time the control loop runs
#define DARK_THRESH 300
#define SUPER_DARK_THRESH 150
#define MAX_ERROR (10 * 1000)

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
    bool leftSuperDark = _leftIrAdc <= SUPER_DARK_THRESH;

    _rightIrAdc = (1023 - analogRead(_rightIrSensor));
    bool rightDark = _rightIrAdc <= DARK_THRESH;
    bool rightSuperDark = _rightIrAdc <= SUPER_DARK_THRESH;

    // Keep state about which single sensor was the last seen to go back
    // to it when we lose track completely.
    if (leftDark) {
        _lastSeenWasLeft = true;
    } else if (rightDark) {
        _lastSeenWasLeft = false;
    }

    // Negative error means go left
    // Positive error means go right#if 0
    if (rightDark && leftDark) {
        _alignmentError = 0;
    } else if (rightSuperDark) {
        _alignmentError = 1000;
    } else if (rightDark) {
        _alignmentError = 5000;
    } else if (leftSuperDark) {
        _alignmentError = -1000;
    }  else if (leftDark) {
        _alignmentError = -5000;
    } else if (!_lastSeenWasLeft) {
        _alignmentError = 10000;
    } else if (_lastSeenWasLeft) {
        _alignmentError = -10000;
    }
}

static int32_t clip(int32_t val, int32_t minVal, int32_t maxVal) {
    val = val > maxVal ? maxVal : val;
    val = val < minVal ? minVal : val;
    return val;
}

#define sign(x) (((x) > 0) - ((x) < 0))

void LineFollowerController::_setMotorSpeeds(void) {
    if (abs(_alignmentError) >= MAX_ERROR) {
        // If we are off the light, rotate to go towards the last seen sensor.
        // The error computation has the correct sign for that as it
        // records the last time at least one sensor saw the dark line.
        _speedLeft = sign(_alignmentError) * _maxSpeedLeftPps;
        _speedRight = -sign(_alignmentError) * _maxSpeedRightPps;
    } else {
        // For on-the-line case, start with a base speed,
        // then apply a delta based on error: simple proportional controller.
        int32_t baseSpeedLeft = 2 * _maxSpeedLeftPps / 3;
        int32_t baseSpeedRight = 2 * _maxSpeedRightPps / 3;

        // Clipping limit to avoid error run-away
        int32_t limit = 300;

        int32_t leftDelta = ((_alignmentError * _kpMillis) / 1000);
        leftDelta = clip(leftDelta, -limit, limit);
        _speedLeft = baseSpeedLeft + leftDelta;
        _speedLeft = clip(_speedLeft, INT16_MIN, INT16_MAX);

        int32_t rightDelta = ((_alignmentError * _kpMillis) / 1000);
        rightDelta = clip(rightDelta, -limit, limit);
        _speedRight = baseSpeedRight - rightDelta;
        _speedRight = clip(_speedRight, INT16_MIN, INT16_MAX);
    }
    _leftSeq.set_speed_pps((int16_t)_speedLeft);
    _rightSeq.set_speed_pps((int16_t)_speedRight);
}

void LineFollowerController::_dumpStateToSerial(void) {
    // Dump all important variables on a line (over USB Serial). Important for debugging and 
    // for calibration.
    char serial_buf[128];
    snprintf(&serial_buf[0], sizeof(serial_buf), "%u,%u,%ld,%ld,%ld\n", _leftIrAdc, _rightIrAdc, _speedLeft, _speedRight, _alignmentError);
    Serial.write(serial_buf);
}

void LineFollowerController::schedulerKick(void) {
    _measureError();
    _setMotorSpeeds();
    _dumpStateToSerial();
}
