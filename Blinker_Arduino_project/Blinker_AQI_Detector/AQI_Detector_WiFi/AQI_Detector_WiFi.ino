/*
 * Blinker AQI Detector
 */

#define BLINKER_PRINT	Serial
#define BLINKER_WIFI

#include <Blinker.h>

char ssid[] = "<Your WiFi network SSID or name>";
char pswd[] = "<Your WiFi network WPA password or WEP key>";

// U8g2lib, https://github.com/olikraus/U8g2_Arduino
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /*reset Pin*/ 13);

// BLINKER_PMSX003ST, https://github.com/i3water/Blinker_PMSX003ST
#include <BLINKER_PMSX003ST.h>

#if defined(ESP32)
HardwareSerial pmsSerial(2);// Pins 16(RX),17(TX)
#else
#include <SoftwareSerial.h>
SoftwareSerial pmsSerial(14, 15);
#endif

BLINKER_PMSX003ST pms;

uint32_t os_time_ms;

#define PM_1    "PM1.0"
#define PM_2_5  "PM2.5"
#define PM_10   "PM10"
#define PMS_FA  "FA"
#define PMS_RH  "RH"
#define PMS_TP  "TP"

BlinkerNumber PM1(PM_1);
BlinkerNumber PM25(PM_2_5);
BlinkerNumber PM10(PM_10);
BlinkerNumber HCHO(PMS_FA);
BlinkerNumber HUMI(PMS_RH);
BlinkerNumber TEMP(PMS_TP);

void drawAQI()
{
    u8g2.setFont(u8g2_font_helvR10_te);
    u8g2.setCursor(0, 13);
    u8g2.print("PM1.0:" + String(pms.getPmAto(1.0)));
    u8g2.setCursor(0, 27);
    u8g2.print("PM2.5:" + String(pms.getPmAto(2.5)));
    u8g2.setCursor(0, 41);
    u8g2.print("PM10:" + String(pms.getPmAto(10)));

    u8g2.setCursor(75, 13);
    u8g2.print("FA:" + String(pms.getForm()));
    u8g2.setCursor(75, 27);
    u8g2.print("RH:" + String((int)pms.getHumi()) + "%");
    u8g2.setCursor(75, 41);
    u8g2.print("TP:" + String((int)pms.getTemp()) + "°");

    u8g2.drawLine(0, 46, 128, 46);

    u8g2.setFont(u8g2_font_helvR10_te);
    u8g2.setCursor(0, 63);
    u8g2.print("Blinker AQI detector");
}

void getPMS()
{
    if (millis() - os_time_ms > 1000UL) {
#ifndef ESP32
        if (!pmsSerial.isListening()) {
            pmsSerial.listen();
            delay(100);
        }
#endif

        pms.request();
        if(!pms.read()){
            return;
        }

        os_time_ms = millis();
    }
}

void detectorDisplay()
{
    getPMS();

    u8g2.firstPage();
    do {
        drawAQI();
    } while ( u8g2.nextPage() );
}

void u8g2Init()
{
    u8g2.begin();
    u8g2.setFlipMode(0);
    u8g2.clearBuffer();
    u8g2.enableUTF8Print();
}

void pmsInit()
{
    pmsSerial.begin(9600);
    pms.begin(pmsSerial);
    pms.setMode(PASSIVE);
}

void setup()
{
    Serial.begin(115200);

    u8g2Init();
    pmsInit();

    Blinker.begin(ssid, pswd);
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

    detectorDisplay();

    PM1.unit("ug/m3");
    PM1.print(pms.getPmAto(1.0));
    PM25.unit("ug/m3");
    PM25.print(pms.getPmAto(2.5));
    PM10.unit("ug/m3");
    PM10.print(pms.getPmAto(10));
    HCHO.unit("ug/m3");
    HCHO.print(pms.getForm());
    HUMI.unit("%");
    HUMI.print(pms.getHumi());
    TEMP.unit("°C");
    TEMP.print(pms.getTemp());

    Blinker.delay(1000);
}