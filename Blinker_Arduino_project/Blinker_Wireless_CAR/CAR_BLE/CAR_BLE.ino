/*
 * Blinker BLE CAR, support Arduino UNO
 */

#define BLINKER_PRINT	Serial
#define BLINKER_BLE

#include <Blinker.h>

#define L_DIR_PIN       7
#define L_PWM_PIN       6
#define R_DIR_PIN       4
#define R_PWM_PIN       5
#define C_BAR_PIN       9

uint32_t car_os_time = millis();
bool isWarn = false;

#define JOY_1 "JOYKey"

BlinkerJoystick JOY1(JOY_1);

void joystick1_callback(uint8_t xAxis, uint8_t yAxis)
{
    BLINKER_LOG2("Joystick1 X axis: ", xAxis);
    BLINKER_LOG2("Joystick1 Y axis: ", yAxis);

    uint8_t L_PWM = 0;
    uint8_t R_PWM = 0;
    bool    L_DIR = false;
    bool    R_DIR = false;

    blinker_car_parse(xAxis, yAxis, L_PWM, L_DIR, R_PWM, R_DIR);

    blinker_car_control(L_PWM, L_DIR, R_PWM, R_DIR);
}

void blinker_car_init()
{
    pinMode(L_DIR_PIN, OUTPUT);
    pinMode(L_PWM_PIN, OUTPUT);
    pinMode(R_DIR_PIN, OUTPUT);
    pinMode(R_PWM_PIN, OUTPUT);

    pinMode(C_BAR_PIN, INPUT_PULLUP);
}

void blinker_car_parse(uint8_t xAxis, uint8_t yAxis, uint8_t &_L_PWM, bool &_L_DIR, uint8_t &_R_PWM, bool &_R_DIR)
{
    bool isStop = false;

    if (yAxis < 128) {
        _L_DIR = true;
        _R_DIR = true;

        _L_PWM = map(abs(128-yAxis), 0, 128, 128, 255);
        _R_PWM = map(abs(128-yAxis), 0, 128, 128, 255);
    }
    else if (yAxis > 128) {
        _L_DIR = false;
        _R_DIR = false;

        _L_PWM = map(abs(128-yAxis), 0, 128, 128, 255);
        _R_PWM = map(abs(128-yAxis), 0, 128, 128, 255);
    }
    else {
        isStop = true;
        
        _L_PWM = 0;
        _R_PWM = 0;
    }

    if (xAxis < 128) {
        if (isStop) {
            _L_DIR = false;
            _R_DIR = true;

            _L_PWM = map(abs(128-xAxis), 0, 128, 128, 255);
            _R_PWM = map(abs(128-xAxis), 0, 128, 128, 255);
        }
        else {
            _L_PWM -= map(abs(128-xAxis), 0, 128, 0, 128);
        }
    }
    else if (xAxis > 128) {
        if (isStop) {
            _L_DIR = true;
            _R_DIR = false;
        }
        else {
            _R_PWM -= map(abs(128-xAxis), 0, 128, 0, 128);
        }
    }
    else {
        if (isStop) {
            _L_PWM = 0;
            _R_PWM = 0;
        }
    }
}

void blinker_car_control(uint8_t cl_pwm, bool cl_dir, uint8_t cr_pwm, bool cr_dir)
{
    digitalWrite(L_DIR_PIN, cl_dir);
    digitalWrite(R_DIR_PIN, cr_dir);
    analogWrite(L_PWM_PIN, cl_pwm);
    analogWrite(R_PWM_PIN, cr_pwm);
}

void blinker_car_detect()
{
    if (!digitalRead(C_BAR_PIN)) {
        if (!isWarn) {
            Blinker.vibrate();
            isWarn = true;
            car_os_time = millis();
        }
        else if (millis() - car_os_time >= 5000) {
            isWarn = false;
        }
    }
    else {
        isWarn = false;
    }
}

void setup()
{
    Serial.begin(115200);

    blinker_car_init();

    Blinker.begin();

    JOY1.attach(joystick1_callback);
}

void loop()
{
    Blinker.run();

    blinker_car_detect();
    
#if defined(BLINKER_CAR_DEBUG)
    BLINKER_LOG4("L_PWM: ", L_PWM, "  | L_DIR: ", L_DIR);
    BLINKER_LOG4("R_PWM: ", R_PWM, "  | R_DIR: ", R_DIR);
    Blinker.delay(2000);
#endif
}
