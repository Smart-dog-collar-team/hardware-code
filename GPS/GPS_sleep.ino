#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <avr/sleep.h>

static const int RXPin = 10, TXPin = 11;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
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
  // Зчитуємо дані з GPS-модуля
  while (ss.available())
    gps.encode(ss.read());

  // Виводимо отримані дані
  Serial.print("satelits - \t\t\t\t");
  printInt(gps.satellites.value(), gps.satellites.isValid(), 5);          //- інформація про супутники
  Serial.println();                 
  Serial.print("latitude and longitude - \t\t\t");
  printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
  Serial.print(" / ");
  printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
  Serial.println();
  Serial.print("date and time - \t\t\t\t");
  printDateTime(gps.date, gps.time);
  Serial.println();
  Serial.print("Speed in km. - \t\t\t");
  printFloat(gps.speed.kmph(), gps.speed.isValid(), 6, 2);
  Serial.println();
  Serial.println("------------------------------------------------------------------");

  // Увімкнення режиму сплячки для економії енергії
  sleep_enable();
  sleep_mode();

  // Затримка перед наступним зчитуванням
  delay(8000);
  
  // Перевірка на отримання даних GPS-модуля
  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("Дані GPS не отримані: перевірте з'єднання"));
}

// Функція для виведення дійсного числа з плаваючою точкою
static void printFloat(float val, bool valid, int len, int prec)
{
  // Вивід зірочок, якщо значення недійсне
  if (!valid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    char format[10];
snprintf(format, sizeof(format), "%%%dd", len);
char buf[10];
snprintf(buf, sizeof(buf), format, val);
Serial.print(buf);
collectData(100);
}
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
  collectData(100);
}


// Функція для виведення дати та часу
static void printDateTime(TinyGPSDate &d, TinyGPSTime &t)
{
// Вивід зірочок, якщо дата недійсна
if (!d.isValid())
{
Serial.print(F("********** "));
}
else
{
// Виведення дати в форматі "dd/mm/yy"
char sz[32];
sprintf(sz, "%02d/%02d/%02d ", d.day(), d.month(), d.year());
Serial.print(sz);
}

// Вивід зірочок, якщо час недійсний
if (!t.isValid())
{
Serial.print(F("******** "));
}
else
{
// Виведення часу в форматі "hh:mm:ss"
char sz[32];
sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
Serial.print(sz);
}

// Виведення віку дати
printInt(d.age(), d.isValid(), 5);
collectData(100);
}
    
