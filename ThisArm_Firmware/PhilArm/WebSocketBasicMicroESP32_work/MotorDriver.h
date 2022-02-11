/*
 * Arduino motor driver library
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public 
 * License as published by the Free Software Foundation; either  
 * version 2.1 of the License, or (at your option) any later version.
 *   
 * Created 26 Feb 2020 Miroslav Kirillin
 */
#ifndef MotorDriver_h
#define MotorDriver_h


#define DIRECTION_A 1
#define DIRECTION_B 0

#define SPEED_MAX 255
#define SPEED_MIN 0
#define SPEED_CURRENT -1


// ШИМ настройки
#define PWM_CHANNEL 15 // ШИМ канал
#define PWM_PRECISSION 8 // разрядность 8-13 ~ 255-8192, 10=1024
#define PWM_FREQUENCY 820 // частота ШИМ

// #define DAC_MAX 255
#define STATUS_ON 1
#define STATUS_OFF 0



#include <inttypes.h>


class MotorDriver {

public:
    void begin(int8_t pinINA, int8_t pinINB);
    void begin(int8_t pinINA, int8_t pinINB, int8_t pinPWM);
    void begin(int8_t pinINA, int8_t pinINB, int8_t pinPWM, int8_t pinEN);

    void start(uint16_t speedFrom, uint16_t speedTo, uint16_t durationChange) { setSpeed(speedTo, durationChange, speedFrom); }
    void start(uint16_t speedTo, uint16_t durationChange = 0){ setSpeed(speedTo, durationChange); }

    void setSpeed(int16_t speedTo = SPEED_MAX, int16_t durationChange = 0, int16_t speedFrom = SPEED_CURRENT);
    
    void setDirection(uint8_t direction);

    void stop(uint16_t durationChange = 0) {setSpeed(0, durationChange);}

    void runtime();

private:
    int8_t _pinINA = -1;
    int8_t _pinINB = -1;
    int8_t pinEN = -1;
    int8_t _pinENB = -1;
    int8_t _pinPWM = -1;
    int8_t _pinCS = -1;
//   int8_t _pinLimit = -1;

    uint8_t status = STATUS_OFF;

    uint8_t speed = SPEED_MIN;

    uint8_t direction = 5;
    uint8_t directionFlag = 0;
    uint32_t directionTime = 1UL;
    
    
    uint8_t speedTo = 0;
    uint8_t smoothly = false;
    uint32_t timePeriod = 0;
    uint32_t timePrevious = 1UL;

};

#endif
#if 0
https://github.com/creatormir/MotorDriver
//example
#include <MotorDriver.h>

#define DEF_PIN_DRIVERH_INA 19 // IN A.
#define DEF_PIN_DRIVERH_INB 18 // IN B.
#define DEF_PIN_DRIVERH_ENAB 17 // ON/OFF
#define DEF_PIN_DRIVERH_PWM 21 // for PWM

MotorDriver motor1;

motor1.begin(DEF_PIN_DRIVERH_INA, DEF_PIN_DRIVERH_INB, DEF_PIN_DRIVERH_PWM, DEF_PIN_DRIVERH_ENAB); // initial

// set direction: DIRECTION_A or DIRECTION_B
motor1.setDirection(DIRECTION_A); // DIRECTION_A/DIRECTION_B

motor1.runtime(); // for loop

// motor1.setSpeed(speedTo, durationChange, speedFrom);
// speedTo = Final speed
// durationChange = Duration(milliseconds) of reaching the final speed. Default zero, that is instant change.
// speedFrom = initial speed. Default current speed.

motor1.setSpeed(150); // Sharp change of speed
motor1.setSpeed(255); // Sharp change of speed
motor1.setSpeed(SPEED_MAX); // Sharp change of speed

motor1.setSpeed(150, 3000, 0); // Smooth acceleration from zero in 3 seconds.
motor1.setSpeed(255, 500, 30); // Smooth acceleration from 30 in 2.3 seconds.

motor1.setSpeed(240);
motor1.setSpeed(50, 1500); // Smooth deceleration from current(240) speed


// stop(duration) // equivalent motor1.setSpeed( duration );
// duration Smooth deceleration. default 0
#endif
