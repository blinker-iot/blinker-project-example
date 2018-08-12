#define BLINKER_PRINT	Serial
#define BLINKER_WIFI

#include <Blinker.h>

char ssid[] = "Your WiFi network SSID or name";
char pswd[] = "Your WiFi network WPA password or WEP key";

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN            0
#define NUMPIXELS      9
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define RGB_1 "RGBKey"

BlinkerRGB RGB1(RGB_1);

void rgb1_callback(uint8_t r_value, uint8_t g_value, uint8_t b_value, uint8_t bright_value)
{
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    BLINKER_LOG2("R value: ", r_value);
    BLINKER_LOG2("G value: ", g_value);
    BLINKER_LOG2("B value: ", b_value);
    BLINKER_LOG2("Rrightness value: ", bright_value);

    uint8_t colorR = map(r_value, 0, 255, 0, bright_value);
    uint8_t colorG = map(g_value, 0, 255, 0, bright_value);
    uint8_t colorB = map(b_value, 0, 255, 0, bright_value);

    for(int i = 0; i < NUMPIXELS; i++){
        pixels.setPixelColor(i, pixels.Color(colorR,colorG,colorB));
        pixels.show();
    }
}


void setup()
{
    Serial.begin(115200);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    
    Blinker.begin(ssid, pswd);

    pixels.begin();

    RGB1.attach(rgb1_callback);
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
}
