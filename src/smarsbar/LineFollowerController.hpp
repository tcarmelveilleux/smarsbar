/**
 * @file LineFollowerController.hpp
 * @short Basic Logic for line follower bot
 *
 * @date 2018-02-27
 * @author Tennessee Carmel-Veilleux <tcv -at- ro.boto.ca>
 * @copyright 2018 Tennessee-Carmel-Veilleux
 *
 * See LICENSE file. 
 */

#if !defined(__LINE_FOLLOWER_CONTROLLER_H)
#define __LINE_FOLLOWER_CONTROLLER_H

#include <Arduino.h>
#include <UnipolarSequencer.hpp>

class LineFollowerController {
public:
    LineFollowerController(UnipolarSequencer &leftSeq, UnipolarSequencer &rightSeq, 
                           int leftIrSensor, int rightIrSensor,
                           int32_t kpMillis, uint32_t schedPeriodUs);

    void schedulerKick(void);

protected:
    UnipolarSequencer &_leftSeq;
    UnipolarSequencer &_rightSeq;
    int _leftIrSensor;
    int _rightIrSensor;
    int32_t _kpMillis;
    uint32_t _schedPeriodUs;
    int32_t _alignmentError;
    uint16_t _maxSpeedLeftPps;
    uint16_t _maxSpeedRightPps;
    uint16_t _rightIrAdc;
    uint16_t _leftIrAdc;
    bool _lastSeenWasLeft;
    int32_t _speedLeft;
    int32_t _speedRight;

    void _dumpStateToSerial(void);
    void _measureError(void);
    void _setMotorSpeeds(void);
};

#endif /* defined(__LINE_FOLLOWER_CONTROLLER_H) */
