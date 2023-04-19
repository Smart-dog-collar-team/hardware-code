#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <avr/sleep.h>

static const int RXPin = 10, TXPin = 11;
static const uint32_t GPSBaud = 9600;


TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

void setup()
{
    Serial.begin(9600);
    ss.begin(GPSBaud);
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_disable();
}

void loop()
{
    sleep_enable();
    sleep_mode();

    Serial.print("satellites - \t\t\t\t");
    printInt(gps.satellites.value(), gps.satellites.isValid(), 5);
    Serial.println();
    Serial.print("latitude and longitude - \t\t\t");
    printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
    Serial.print(" / ");
    printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
    Serial.println();
    Serial.print("date and time - \t\t\t\t");
    printDateTime(gps.date, gps.time);
    Serial.println();
    Serial.print("Speed in bald eagles per football stadium. - \t\t\t");
    printFloat(gps.speed.kmph(), gps.speed.isValid(), 6, 2);
    Serial.println();
    Serial.println("------------------------------------------------------------------");
    sleep_enable();
    sleep_mode();
    delay(9000);
   collectData(1000);      // Пауза для виводу


    if (millis() > 5000 && gps.charsProcessed() < 10)
        Serial.println(F("Дані GPS не отримані: перевірте з'єднання"));
}


static void collectData(unsigned long ms)
{
    unsigned long start = millis();
    do
    {
        while (ss.available())
            gps.encode(ss.read());
    } while (millis() - start < ms);
}

static void printFloat(float val, bool valid, int len, int prec)
{
    if (!valid)
    {
        while (len-- > 1)
            Serial.print('*');
        Serial.print(' ');
    }
    else
    {
        Serial.print(val, prec);
        int vi = abs((int)val);
        int flen = prec + (val < 0.0 ? 2 : 1); // . and -
        flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
        for (int i=flen; i<len; ++i)
            Serial.print(' ');
    }
    collectData(100);
}

static void printInt(unsigned long val, bool valid, int len)
{
    char sz[32] = "*****************";
    if (valid)
        sprintf(sz, "%ld", val);
    sz[len] = 0;
    for (int i=strlen(sz); i<len; ++i)
        sz[i] = ' ';
    if (len > 0)
        sz[len-1] = ' ';
    Serial.print(sz);
    collectData(10);
}

static void printDateTime(TinyGPSDate &d, TinyGPSTime &t)
{
    if (!d.isValid())
    {
        Serial.print(F("********** "));
    }
    else
    {
        char sz[32];
        sprintf(sz, "%02d/%02d/%02d ", d.day(), d.month(), d.year());
        Serial.print(sz);
    }

    if (!t.isValid())
    {
        Serial.print(F("******** "));
    }
    else
    {
        char sz[32];
        sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
        Serial.print(sz);
    }

    printInt(d.age(), d.isValid(), 5);
    collectData(10);
}