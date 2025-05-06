#include "MF_ASI.h"
#include "allocateMem.h"
#include "commandmessenger.h"
#include "include/main_gauge.h"
#include "include/needle.h"
#include "include/number_tape.h"
#include "include/4inchLCDConfig_Waveshare.h"

#define TFT_SKYBLUE 0x07FF

// Global definitions for LCD screen
static LGFX lcd;
static LGFX_Sprite canvas(&lcd);
static LGFX_Sprite mainGaugeSpr(&canvas);
static LGFX_Sprite numberTapeSpr(&canvas);
static LGFX_Sprite needleSpr(&canvas);

/* **********************************************************************************
    This is just the basic code to set up your custom device.
    Change/add your code as needed.
********************************************************************************** */

MF_ASI::MF_ASI(uint8_t Pin1, uint8_t Pin2)
{
    _pin1 = Pin1;
    _pin2 = Pin2;
}

void MF_ASI::begin()
{
    Serial.begin(115200);
    lcd.init();
    if (lcd.width() < lcd.height())
    {
        lcd.setRotation(lcd.getRotation() ^ 1);
    }

    lcd.fillScreen(TFT_BLUE);
    delay(5000);
    canvas.createSprite(240, 480);
    mainGaugeSpr.setBuffer(const_cast<std::uint16_t *>(main_gauge), 480, 480, 16);
    numberTapeSpr.setBuffer(const_cast<std::uint16_t *>(number_tape), 480, 480, 16);
    needleSpr.setBuffer(const_cast<std::uint16_t *>(needle), NEEDLE_WIDTH, NEEDLE_HEIGHT, 16);
    lcd.fillScreen(TFT_BLACK);
}

void MF_ASI::attach(uint16_t Pin3, char *init)
{
    _pin3 = Pin3;


}

void MF_ASI::detach()
{
    if (!_initialised)
        return;
    canvas.deleteSprite();
    mainGaugeSpr.deleteSprite();
    numberTapeSpr.deleteSprite();
    needleSpr.deleteSprite();
    _initialised = false;
}

void MF_ASI::set(int16_t messageID, char *setPoint)
{
    /* **********************************************************************************
        Each messageID has it's own value
        check for the messageID and define what to do.
        Important Remark!
        MessageID == -2 will be send from the board when PowerSavingMode is set
            Message will be "0" for leaving and "1" for entering PowerSavingMode
        MessageID == -1 will be send from the connector when Connector stops running
        Put in your code to enter this mode (e.g. clear a display)

    ********************************************************************************** */
    int32_t  data = atoi(setPoint);
    uint16_t output;

    // do something according your messageID
    switch (messageID) {
    case -1:
        // tbd., get's called when Mobiflight shuts down
        break;
    case -2:
        // tbd., get's called when PowerSavingMode is entered
        break;
    case 0:
        setAirspeed(atof(setPoint));
        break;
    case 1:
        /* code */
        setTASAngle(atof(setPoint));
        break;
    case 2:
        /* code */
        break;
    default:
        break;
    }
    drawGauge();
}

void MF_ASI::update()
{
    // Do something which is required regulary

}

void MF_ASI::drawGauge()
{

    if (airSpeed > 210)   // Max Airspeed in indicator
        airSpeed = 210;
    // Do something which is required regulary
    if (airSpeed <= 40)
        ASIneedleRotation = scaleValue(airSpeed, 0, 40, 0, 25);
    else if (airSpeed > 40 && airSpeed <= 200)
        ASIneedleRotation = scaleValue(airSpeed, 41, 210, 26, 348);
    else if (airSpeed > 210)
        airSpeed = 210;

    airSpeed_MPH = airSpeed/1.151;

    // ASIneedleRotation = angle;
    whiteArcStartAngle = (int)scaleValue(V_S0, 40, 220, 21, 309);
    whiteArcEndAngle = (int)scaleValue(V_FE, 40, 220, 21, 309);
    greenArcStartAngle = (int)scaleValue(V_S1, 40, 220, 21, 309);
    greenArcEndAngle = (int)scaleValue(V_N0, 40, 220, 21, 309);

    canvas.fillScreen(TFT_BLACK);
    needleSpr.setPivot(19, 221);

    drawLeftGauge();
    drawRightGauge();

}
void MF_ASI::drawLeftGauge()
{
    // Draw left half
    // numberTapeSpr.pushSprite(&canvas, 0, 0, TFT_SKYBLUE);
    canvas.setPivot(240, 240);
    numberTapeSpr.setPivot(240, 240);
    numberTapeSpr.pushRotated(&canvas, TASAngle, TFT_BLACK);
    mainGaugeSpr.pushSprite(&canvas, 0, 0, TFT_SKYBLUE);
    needleSpr.pushRotated(&canvas, ASIneedleRotation , TFT_BLACK);
    // Draw White Arc
    canvas.fillArc(240, 240, 224, 214, whiteArcStartAngle - 90, whiteArcEndAngle - 90, TFT_WHITE);

    // Draw Green Arc
    if (V_N0 < 134)
        canvas.fillArc(240, 240, 240, 226, greenArcStartAngle - 90, greenArcEndAngle - 90, TFT_GREEN);
    else
    {
        greenArcStartAngle = (int)scaleValue(V_S1, 40, 220, 21, 309);
        greenArcEndAngle = (int)scaleValue(134, 40, 220, 21, 309);
        canvas.fillArc(240, 240, 235, 225, greenArcStartAngle - 90, greenArcEndAngle - 90, TFT_GREEN);
        greenArcStartAngle = (int)scaleValue(135, 40, 220, 21, 309);
        greenArcEndAngle = (int)scaleValue(V_N0, 40, 220, 21, 309);
        canvas.fillArc(240, 240, 190, 180, greenArcStartAngle - 90, greenArcEndAngle - 90, TFT_GREEN);
    }

    // Draw Yellow Arc
    yellowArcStartAngle = (int)scaleValue(V_N0, 40, 220, 21, 309);
    yellowArcEndAngle = (int)scaleValue(180, 40, 220, 21, 309);
    canvas.fillArc(240, 240, 190, 180, yellowArcStartAngle - 90, yellowArcEndAngle - 90, TFT_YELLOW);
    yellowArcStartAngle = (int)scaleValue(181, 40, 220, 21, 309);
    yellowArcEndAngle = (int)scaleValue(V_NE, 40, 220, 21, 309);
    canvas.fillArc(240, 240, 235, 225, yellowArcStartAngle - 90, yellowArcEndAngle - 90, TFT_YELLOW);

    // Draw Red Line for V_NE
    V_NEArcStartAngle = (int)scaleValue(V_NE, 40, 220, 21, 309);
    V_NEArcEndAngle = (int)scaleValue(V_NE, 40, 220, 21, 309) + 2;
    canvas.fillArc(240, 240, 240, 190,  V_NEArcStartAngle - 90,   V_NEArcEndAngle - 90, TFT_RED);
    canvas.pushSprite(&lcd, 0, 0);


}

void MF_ASI::drawRightGauge()
{

        // Draw right half
    canvas.setPivot(240 - x_offset, 240);
    // numberTapeSpr.pushSprite(&canvas, -x_offset, 0, TFT_SKYBLUE);
    numberTapeSpr.pushRotated(&canvas, TASAngle, TFT_BLACK );
    mainGaugeSpr.pushSprite(&canvas, -x_offset, 0, TFT_SKYBLUE);
    needleSpr.pushRotated(&canvas, ASIneedleRotation , TFT_BLACK);

    // Draw White Arc
    canvas.fillArc(0, 240, 224, 214, whiteArcStartAngle - 90, whiteArcEndAngle - 90, TFT_WHITE);
    // Draw Green Arc
    if (V_NE < 134)
        canvas.fillArc(0, 240, 235, 225, greenArcStartAngle - 90, greenArcEndAngle - 90, TFT_GREEN);
    else
    {
        greenArcStartAngle = (int)scaleValue(V_S1, 40, 220, 21, 309);
        greenArcEndAngle = (int)scaleValue(134, 40, 220, 21, 309);
        canvas.fillArc(0, 240, 235, 225, greenArcStartAngle - 90, greenArcEndAngle - 90, TFT_GREEN);
        greenArcStartAngle = (int)scaleValue(135, 40, 220, 21, 309);
        greenArcEndAngle = (int)scaleValue(V_N0, 40, 220, 21, 309);
        canvas.fillArc(0, 240, 190, 180, greenArcStartAngle - 90, greenArcEndAngle - 90, TFT_GREEN);
    }

        // Draw Yellow Arc
    yellowArcStartAngle = (int)scaleValue(V_N0, 40, 220, 21, 309);
    yellowArcEndAngle = (int)scaleValue(180, 40, 220, 21, 309);
    canvas.fillArc(0, 240, 190, 180, yellowArcStartAngle - 90, yellowArcEndAngle - 90, TFT_YELLOW);
    yellowArcStartAngle = (int)scaleValue(181, 40, 220, 21, 309);
    yellowArcEndAngle = (int)scaleValue(V_NE, 40, 220, 21, 309);
    canvas.fillArc(0, 240, 235, 225, yellowArcStartAngle - 90, yellowArcEndAngle - 90, TFT_YELLOW);
        // Draw Red Line for V_NE
    V_NEArcStartAngle = (int)scaleValue(V_NE, 40, 220, 21, 309);
    V_NEArcEndAngle = (int)scaleValue(V_NE, 40, 220, 21, 309) + 2;
    canvas.fillArc(0, 240, 240, 190,  V_NEArcStartAngle - 90,   V_NEArcEndAngle - 90, TFT_RED);

    canvas.pushSprite(&lcd, x_offset, 0);


}

void MF_ASI::setAirspeed(float value)
{
    airSpeed = value;
}

void MF_ASI::setTASAngle(float value)
{
    TASAngle = scaleValue(value, -1, 1, -40, 65);
}


// Scale Function
float MF_ASI::scaleValue(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}