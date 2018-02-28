/**
 * @file smarsbar.ino
 * @short Basic SMARS robot sample
 *
 * @date Feb 17, 2018
 * @author Tennessee Carmel-Veilleux <tcv -at- ro.boto.ca>
 * @copyright 2018 Tennessee Carmel-Veilleux
 *
 * SEE LICENSE FILE
 */

#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
  // Required for Serial on Zero based boards
  #define Serial SERIAL_PORT_USBVIRTUAL
#endif

#define LEFT_IR_SENSOR BOARD_IR_CH1
#define RIGHT_IR_SENSOR BOARD_IR_CH2
#define ZERO_TIMER_CLOCK (48UL * 1000UL * 1000UL)
#define ZERO_TIMER_PRESCALER (64UL)
#define STEPPER_MAX_SPEED_PPS (500U)
#define SCHED_PERIOD_MILLIS (50U)

#include <Adafruit_ZeroTimer.h>
#include "board_config.h"
#include <UnipolarSequencer.hpp>
#include "LineFollowerController.hpp"

static bool _led_state = false;

static uint32_t _last_sched_tick_ms = 0;

Adafruit_ZeroTimer _left_timer = Adafruit_ZeroTimer(3);
Adafruit_ZeroTimer _right_timer = Adafruit_ZeroTimer(5);
//Adafruit_ZeroTimer _sched_timer = Adafruit_ZeroTimer(5);

static void _freq_setter(Adafruit_ZeroTimer& timer, uint16_t freq) {
    if (0 == freq) { 
        freq = 1;
    }

    uint32_t timer_counts = ((ZERO_TIMER_CLOCK / ZERO_TIMER_PRESCALER) / freq) - 1UL;
    if (UINT32_MAX == timer_counts) {
        timer_counts = 1;
    } else if (timer_counts > UINT16_MAX) {
        timer_counts = UINT16_MAX;
    }
    timer.setPeriodMatch(timer_counts & 0xFFFFUL, 1, 0);
    // FIXME: Maybe handle in-between/partial counts of timer and associated wrap-around, rather
    //        than restart and potentially skip pulses.
    timer.restart();
}

static void _left_freq_setter(uint16_t freq) {
    _freq_setter(_left_timer, freq);
}

static void _right_freq_setter(uint16_t freq) {
    _freq_setter(_right_timer, freq);
}

static UnipolarSequencer _right_seq(BOARD_MOTOR1_PH_A, BOARD_MOTOR1_PH_B,
                                 BOARD_MOTOR1_PH_C, BOARD_MOTOR1_PH_D,
                                 STEPPER_MAX_SPEED_PPS, SCHED_PERIOD_MILLIS * 1000UL,
                                 &_right_freq_setter);
static UnipolarSequencer _left_seq(BOARD_MOTOR2_PH_A, BOARD_MOTOR2_PH_B,
                                 BOARD_MOTOR2_PH_C, BOARD_MOTOR2_PH_D,
                                 STEPPER_MAX_SPEED_PPS, SCHED_PERIOD_MILLIS * 1000UL,
                                 &_left_freq_setter);

static LineFollowerController _controller(_left_seq, _right_seq, LEFT_IR_SENSOR, RIGHT_IR_SENSOR,
                              10 * 1000, SCHED_PERIOD_MILLIS * 1000UL);

// Left sequencer pulse timing
static void Timer3Callback0(struct tc_module *const module_inst) {
    _left_seq.kick_pulse();
}

// Right sequencer pulse timing
static void Timer5Callback0(struct tc_module *const module_inst) {
    _right_seq.kick_pulse();
}

static void setupTimers(void) {
    _left_timer.configure(TC_CLOCK_PRESCALER_DIV64, // prescaler
                  TC_COUNTER_SIZE_16BIT,   // bit width of timer/counter
                  TC_WAVE_GENERATION_MATCH_PWM  // match style
                  );

    _left_timer.setCallback(true, TC_CALLBACK_CC_CHANNEL0, Timer3Callback0); 
    _left_timer.setPeriodMatch(0xFFFFUL, 1, 0);
    _left_timer.enable(true);

    _right_timer.configure(TC_CLOCK_PRESCALER_DIV64, // prescaler
                  TC_COUNTER_SIZE_16BIT,   // bit width of timer/counter
                  TC_WAVE_GENERATION_MATCH_PWM  // match style
                  );

    _right_timer.setCallback(true, TC_CALLBACK_CC_CHANNEL0, Timer5Callback0); 
    _right_timer.setPeriodMatch(0xFFFFUL, 1, 0);
    _right_timer.enable(true);
}

// the setup function runs once when you press reset or power the board
void setup() {
    setup_board();
    Serial.begin(115200);
    _left_seq.set_half_step(false);
    _left_seq.set_cw_positive(false);
    _right_seq.set_half_step(false);
    setupTimers();
    _last_sched_tick_ms = millis();
}

static uint32_t ticks_delta(uint32_t old_ticks, uint32_t new_ticks) {
    return (new_ticks - old_ticks);
}

static bool _did_sched_tick_hit(void) {
    // FIXME: Handle wraparound
    uint32_t new_ticks = millis();

    if (ticks_delta(_last_sched_tick_ms, new_ticks) >= SCHED_PERIOD_MILLIS) {
        _last_sched_tick_ms = new_ticks;
        return true;
    } else {
        return false;
    }
}

#define THRESH 700

static void _handle_scheduler_tick(void) {
    _controller.schedulerKick();
    _left_seq.kick_sched();
    _right_seq.kick_sched();
}

// the loop function runs over and over again forever
void loop() {
    _led_state = !_led_state;
    digitalWrite(BOARD_LED, HIGH);

    if (_did_sched_tick_hit()) {
        _handle_scheduler_tick();
    }

    digitalWrite(BOARD_LED, LOW);
}
