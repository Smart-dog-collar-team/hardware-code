#include <SoftwareSerial.h>
#include <TinyGPS++.h>

static const int RXPin = 10, TXPin = 11;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

SoftwareSerial sim(3,2);
int _timeout;
String _buffer;
String number = "+380680082163"; //-> change with your number


float latitude;
float longitude;

void setup() {
  ss.begin(GPSBaud);
  delay(7000); //delay for 7 seconds to make sure the modules get the signal
  Serial.begin(9600);
  _buffer.reserve(50);
  Serial.println("System Started...");
  sim.begin(9600);
  delay(1000);
  Serial.println("Type s to send an SMS");
}
void loop() {

  if (Serial.available() > 0){
    if (Serial.read() == 's')
    {
        sim.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
        delay(1000);
        sim.println("AT+CMGS=\"" + number + "\"\r"); //Mobile phone number to send message
        delay(1000);

        if (!gps.location.isValid()) {
          Serial.print("gps is not connected");
        }

        while (ss.available() > 0){
          gps.encode(ss.read());
          //Serial.print("check");
          if (gps.location.isUpdated()){
            sim.print("https://www.google.com/maps/?q=");
            sim.print(gps.location.lat(), 6);
            sim.print(",");
            sim.print(gps.location.lng(), 6);
            delay(100);
            //delay(1000);
            sim.println((char)26);
            _buffer = _readSerial();
          }
        }
    }
  }
  if (sim.available() > 0)
    Serial.write(sim.read());
}

String _readSerial() {
  _timeout = 0;
  while  (!sim.available() && _timeout < 12000  )
  {
    delay(13);
    _timeout++;
  }
  if (sim.available()) {
    return sim.readString();
  }
}

