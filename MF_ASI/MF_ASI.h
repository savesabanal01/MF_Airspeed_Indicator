#pragma once

#include "Arduino.h"

class MF_ASI
{
public:
    MF_ASI(uint8_t Pin1, uint8_t Pin2);
    void begin();
    void attach(uint16_t Pin3, char *init);
    void detach();
    void set(int16_t messageID, char *setPoint);
    void update();

private:
    bool    _initialised;
    uint8_t _pin1, _pin2, _pin3;
    float scaleValue(float x, float in_min, float in_max, float out_min, float out_max);
    void setAirspeed(float value);
    void setTASAngle(float value);
    void drawGauge();
    void drawLeftGauge();
    void drawRightGauge();

    // Variables

    uint16_t angle = 0;
    uint16_t x_offset = 240;
    float startTIme = 0;
    float endTime = 0;

    // Cessna 172 Default V Speeds in MPH
    uint16_t V_S0 = 54;
    uint16_t V_S1 = 61;
    uint16_t V_FE = 110;
    uint16_t V_N0 = 145;
    uint16_t V_NE = 187;

    // V Speeds Arcs
    uint16_t whiteArcStartAngle;
    uint16_t whiteArcEndAngle;
    uint16_t greenArcStartAngle;
    uint16_t greenArcEndAngle;
    uint16_t yellowArcStartAngle;
    uint16_t yellowArcEndAngle;
    uint16_t V_NEArcStartAngle;
    uint16_t V_NEArcEndAngle;
    int16_t myID = 10;

    // variables
    float airSpeed = 0;
    float airSpeed_MPH = 0;
    float ASIneedleRotation = 0; // angle of rotation of needle based on the Indicated AirSpeed
    float TASAngle = 0;

    bool powerSaveFlag = false;   // Flag for power saving mode
};