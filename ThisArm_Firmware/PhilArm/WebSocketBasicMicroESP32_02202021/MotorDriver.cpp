/*
 * motor driver library
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public 
 * License as published by the Free Software Foundation; either  
 * version 2.1 of the License, or (at your option) any later version.
 *   
 * Created 26 Feb 2020 Miroslav Kirillin
 */

#include <Arduino.h>
#include "MotorDriver.h"

#ifdef ARDUINO_ARCH_ESP32
    // #include <analogWrite.h>
#endif

void MotorDriver::begin(int8_t pinINA, int8_t pinINB, int8_t pinPWM, int8_t pinEN) {

    pinMode(pinINA, OUTPUT);
    digitalWrite( pinINA, 0 );
    this->_pinINA = pinINA;
    
    pinMode(pinINB, OUTPUT);
    digitalWrite( pinINB, 0 );
    this->_pinINB = pinINB;
    
    pinMode(pinEN, OUTPUT);
    digitalWrite( pinEN, 1 );
    this->pinEN = pinEN;
    
    pinMode(pinPWM, OUTPUT);
    #ifdef ARDUINO_ARCH_ESP32
        ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_PRECISSION);
        ledcAttachPin(pinPWM, PWM_CHANNEL);
        ledcWrite(PWM_CHANNEL, 0);
        this->_pinPWM = PWM_CHANNEL;
    #elif ARDUINO
        analogWrite( pinPWM, 0 );
        this->_pinPWM = pinPWM;
    #endif

    setDirection(DIRECTION_A);
}

// 
void MotorDriver::setSpeed(int16_t speedTo, int16_t durationChange, int16_t speedFrom) {
    if(speedFrom < 0) speedFrom = this->speed;
    if(speedTo > SPEED_MAX) speedTo = SPEED_MAX;
    if(speedTo < 0) speedTo = SPEED_MIN;

    if(this->speed == speedTo) return;

    if(durationChange >= 0) this->smoothly = false;

    if(durationChange > 0){
        // smoothly change of speed

        uint8_t tmp = abs(speedTo - speedFrom);
        if(tmp != 0){
            this->speed = speedFrom;
            this->speedTo = speedTo;
            
            this->timePeriod = durationChange/tmp;
            if(this->timePeriod < 1) this->timePeriod = 1;
            
            this->smoothly = true;
        }
    }else{
        // sharp change of speed

        #ifdef ARDUINO_ARCH_ESP32
            ledcWrite(this->_pinPWM, speedTo);
        #elif ARDUINO
            analogWrite(this->_pinPWM, speedTo);
        #endif

        this->speed = speedTo;
    }


    if(this->speed == 0 && this->status == STATUS_ON){
        this->status = STATUS_OFF;

        this->smoothly = false;
        digitalWrite(this->pinEN, HIGH);
        
        // log_d("STATUS_OFF" );
    }else
    if(speedTo != 0 && this->status == STATUS_OFF){
        this->status = STATUS_ON;
        digitalWrite(this->pinEN, LOW);
        
        // log_d("STATUS_ON" );
    }
    
}


// DIRECTION_A DIRECTION_B
void MotorDriver::setDirection(uint8_t direction) {
    if( this->direction == direction ){ return; }
    digitalWrite(this->_pinINA, LOW);
    digitalWrite(this->_pinINB, LOW);
    delayMicroseconds(600);

    if (direction) {
        digitalWrite(this->_pinINA, HIGH);
    } else {
        digitalWrite(this->_pinINB, HIGH);
    }
    delayMicroseconds(300);
    
    this->direction = direction;
    // this->directionFlag = true;
    // this->directionTime = micros();
}


// вызывается из основного цикла
void MotorDriver::runtime() {

    // if(this->directionFlag && micros() - this->directionTime >= 1800){
    //     this->directionFlag = false;

    //     if (this->direction) {
    //         digitalWrite(this->_pinINA, HIGH);
    //     } else {
    //         digitalWrite(this->_pinINB, HIGH);
    //     }
    // }

    
    if(this->smoothly && millis() - this->timePrevious >= this->timePeriod){
        this->timePrevious = millis();
        int16_t diffSpeed = this->speedTo - this->speed;

        // log_d("this->speed %d", this->speed);
        int16_t speed = this->speed;

        if(diffSpeed > 0){
            speed++;
        }else
        if(diffSpeed < 0){
            speed--;
        }
        
        setSpeed( speed, -1 );
        
        if( this->speedTo - this->speed == 0 ){
            this->smoothly = false;
        }
    }
}


// uint16_t MotorDriver::current() {
//   if (this->_pinCS < 0) return 0;
//  return analogRead(this->_pinCS);
// }
