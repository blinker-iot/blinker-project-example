#define BLINKER_PRINT	Serial
#define BLINKER_BLE

#define RGB1            "rgb"
#define SLIDER_1        "SliderKey"

#include <Blinker.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN            8
#define NUMPIXELS      9
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
    Serial.begin(115200);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    
    Blinker.begin();
    Blinker.wInit(RGB1, W_RGB);
    Blinker.wInit(SLIDER_1, W_SLIDER);
    pixels.begin();
}

void loop()
{
    Blinker.run();

    if (Blinker.available()) {
        BLINKER_LOG2("Blinker.readString(): ", Blinker.readString());

        Blinker.vibrate();
        
        uint32_t BlinkerTime = millis();
        Blinker.print(BlinkerTime);
        Blinker.print("millis", BlinkerTime);
    }

    uint8_t colorR = Blinker.rgb(RGB1,R);
    uint8_t colorG = Blinker.rgb(RGB1,G);
    uint8_t colorB = Blinker.rgb(RGB1,B);

    uint8_t brightness = Blinker.slider(SLIDER_1);

    BLINKER_LOG2("Red color: ", colorR);
    BLINKER_LOG2("Green color: ", colorG);
    BLINKER_LOG2("Blue color: ", colorB);
    BLINKER_LOG2("brightness: ", brightness);

    colorR = map(colorR, 0, 255, 0, brightness);
    colorG = map(colorG, 0, 255, 0, brightness);
    colorB = map(colorB, 0, 255, 0, brightness);

    for(int i=0;i<NUMPIXELS;i++){
        pixels.setPixelColor(i, pixels.Color(colorR,colorG,colorB));
        pixels.show();
    }

    Blinker.delay(2000);
}