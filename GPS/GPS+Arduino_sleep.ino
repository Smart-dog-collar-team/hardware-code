#include <avr/power.h>
#include <avr/sleep.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>
//https://github.com/mikalhart/TinyGPS

#define UART_SERIAL_BAUD 1200
#define SS_SERIAL_BAUD 9600
#define GPS_TIMEOUT 0
#define MAX_ON_TIME 3600000
#define GPS_DATA_TRANSMIT_INTERVAL 15000
#define SOFTWARE_SERIAL_RX 3
#define SOFTWARE_SERIAL_TX 2
#define GPS_POWER_PIN 4

TinyGPS gps;
SoftwareSerial ss(SOFTWARE_SERIAL_RX, SOFTWARE_SERIAL_TX); // For GPS module


const byte OutputPowerSavingPins[] = {5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};

unsigned long startMillis, targetTime;

bool requestSleep = true;
byte gpsRequestMode = 0;


void transmit(String datatype, String data) {
  Serial.print(datatype + ":" + data + "\n");
  delay(1000); //TEMP - testing
}

void ArduinoSleep() {


  gpsRequestMode = 0;
  requestSleep = false;
  ss.end();
  digitalWrite(GPS_POWER_PIN, LOW); // Turn off GPS module

  transmit("message", "sleeping");
  delay(2000); // TEMP gives extra time for message to exit serial buffer

  set_sleep_mode(SLEEP_MODE_IDLE); // Sleep mode is set here

  sleep_enable();

  power_adc_disable();
  power_spi_disable();
  power_timer0_disable();
  power_timer1_disable();
  power_timer2_disable();
  power_twi_disable();

  PRR = PRR | 0b00100000;

  sleep_mode();



  sleep_disable();

  PRR = PRR & 0b00000000;

  power_all_enable();

  digitalWrite(GPS_POWER_PIN, HIGH);
  delay(500);
  ss.begin(SS_SERIAL_BAUD);
  startMillis = millis();
}

long readVcc() {

  long result;

  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2);
  ADCSRA |= _BV(ADSC);
  while (bit_is_set(ADCSRA, ADSC));
  result = ADCL;
  result |= ADCH << 8;
  result = 1126400L / result;
  return result;
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  pinMode(GPS_POWER_PIN, OUTPUT);
  digitalWrite(GPS_POWER_PIN, LOW);

  Serial.begin(UART_SERIAL_BAUD);
  ss.begin(SS_SERIAL_BAUD);

  for (byte i = 0; i < (sizeof(OutputPowerSavingPins) / sizeof(byte)); i++) {
    pinMode(OutputPowerSavingPins[i], OUTPUT);
  }

  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}

void loop() {
  if (Serial.available() > 0) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(2000);

    String receivedSerialString = Serial.readStringUntil('\n');

    /
    int indexofcolon = receivedSerialString.indexOf(":");
    int lengthofstring = receivedSerialString.length();
    String datatype = "";
    if (indexofcolon == -1) {
      delay(100);
    } else if (indexofcolon != 0 && indexofcolon != lengthofstring - 1) {

      startMillis = millis();



      datatype = receivedSerialString.substring(0, indexofcolon);
      String datacsv = receivedSerialString.substring(indexofcolon + 1);
      int indexofcomma = datacsv.indexOf(",");
      int i, commacount;
      for (i = 0, commacount = 0; datacsv[i]; i++) {
        commacount += (datacsv[i] == ',');
      }

      String datacsvarray[commacount + 1];

      for (int i = 0; i < commacount + 1; i++) {

        if (datacsv.indexOf(",") > -1) {
          datacsvarray[i] = datacsv.substring(0, datacsv.indexOf(","));
          datacsv.remove(0, datacsv.indexOf(",") + 1);
        } else {
          datacsvarray[i] = datacsv;
          datacsv.remove(0);
        }
      }
      if (datatype == "cell") {
        if (datacsvarray[0] == "volts") {
          transmit("cell", String(readVcc()) + "mV");
        }
      } else if (datatype == "gps") {
        // data = "latitude,longitude,num of satellites,accuracy,speed,direction,age of data,checksum"
        if (datacsvarray[0] == "once") {
          transmit("message", "rcvd gps once");
          gpsRequestMode = 1;
        } else if (datacsvarray[0] == "continuous") {
          transmit("message", "rcvd continuous");
          gpsRequestMode = 2;
        } else if (datacsvarray[0] == "stop") {
          transmit("message", "rcvd gps stop");
          gpsRequestMode = 0;
        }
        targetTime = millis() + GPS_TIMEOUT;
      } else {
        //transmit("message", "datatype invalid");
        delay(100); // Do nothing
      }
    } else {
      //transmit("message", "empty data/dtype");
      delay(100);
    }
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
  }

  if (gpsRequestMode == 0) { // gpsRequestMode will be 0 after waking up or if gps:stop is received
    requestSleep = true; // Request sleep
  } else {
    requestSleep = false; //Set requestSleep to false, just to be sure
    if (targetTime > millis() || GPS_TIMEOUT == 0) {
      bool newData = false;

      for (unsigned long start = millis(); millis() - start < GPS_DATA_TRANSMIT_INTERVAL;) {
        while (ss.available()) {
          char c = ss.read();

          if (gps.encode(c)) // Did a new valid sentence come in?
            newData = true;
        }
      }

      if (newData) {
        targetTime = millis() + GPS_TIMEOUT;

        byte sats;
        unsigned short sentences, failed, acc, spd;
        unsigned long age, chars;
        float flat, flon;
        String dir;
        gps.stats(&chars, &sentences, &failed);
        gps.f_get_position(&flat, &flon, &age);

        flat   = flat               == TinyGPS::GPS_INVALID_F_ANGLE    ? 0.0    : flat;                              // Latitude
        flon   = flon               == TinyGPS::GPS_INVALID_F_ANGLE    ? 0.0    : flon;                              // Longitude
        sats   = gps.satellites()   == TinyGPS::GPS_INVALID_SATELLITES ? 0      : gps.satellites();                  // Satellites
        acc    = gps.hdop()         == TinyGPS::GPS_INVALID_HDOP       ? 0      : gps.hdop();                        // precision / accuracy
        spd    = gps.f_speed_kmph() == TinyGPS::GPS_INVALID_F_SPEED    ? 0      : gps.f_speed_kmph();                // speed
        dir    = gps.f_course()     == TinyGPS::GPS_INVALID_F_ANGLE    ? "*** " : TinyGPS::cardinal(gps.f_course()); // direction
        age    = age                == TinyGPS::GPS_INVALID_AGE        ? 0      : age;                               // age of data
        failed = failed             == 0xFFFFFFFF                      ? 255    : failed;                            // checksum

        transmit("gps",
                 String(flat, 6) + "," +
                 String(flon, 6) + "," +
                 String(sats) + "," +
                 String(acc) + "," +
                 String(spd) + "," +
                 dir + "," +
                 String(age) + "," +
                 String(failed)
                );

        if (gpsRequestMode != 2) {
          gpsRequestMode = 0;
        }
      }
    } else { /
      transmit("message", "Timeout no GPS");
      gpsRequestMode = 0;
    }
  }
  if (millis() - startMillis > MAX_ON_TIME && MAX_ON_TIME != 0) { /
    transmit("message", "Failsafe sleep");
    gpsRequestMode = 0;
    requestSleep = true;

  }
  if (requestSleep == true) {
    ArduinoSleep();
  }
}