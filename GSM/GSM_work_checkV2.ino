#include <SoftwareSerial.h>

SoftwareSerial mySerial(3, 2); // RX, TX
String apn = "internet";
String url = "https://portal.azure.com/?l=ru.ru-ua#@edu.lpnu.ua/resource/subscriptions/62269bc9-1a7c-428f-b99e-3bd7ebff3580/resourcegroups/b59iot/providers/Microsoft.Devices/IotHubs/dogCollar/Overview";
String data = "param1=value1 & param2=value2";
String httpData = "";

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  mySerial.begin(9600);
  delay(1000);

  mySerial.println("AT");
  delay(1000);
  if (!mySerial.find("OK")) {
    Serial.println("Error: GSM module not responding");
    while (true);
  }

  mySerial.println("AT+CGATT=1");
  delay(2000);
  mySerial.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
  delay(2000);
  mySerial.print("AT+SAPBR=3,1,\"APN\",\"");
  mySerial.print(apn);
  mySerial.println("\"");
  delay(2000);
  mySerial.println("AT+SAPBR=1,1");
  delay(2000);
  mySerial.println("AT+HTTPINIT");
  delay(2000);
}

void loop() {
  mySerial.println("AT+HTTPPARA=\"CID\",1");
  delay(1000);
  mySerial.print("AT+HTTPPARA=\"URL\",\"");
  mySerial.print(url);
  mySerial.println("\"");
  delay(1000);
  mySerial.print("AT+HTTPDATA=");
  mySerial.print(data.length());
  mySerial.println(",120000");
  delay(1000);
  mySerial.print(data);
  delay(1000);
  mySerial.println();
  mySerial.println("AT+HTTPACTION=1");
  delay(5000);

  mySerial.println("AT+HTTPREAD");
  delay(1000);
  while (mySerial.available()) {
    char c = mySerial.read();
    httpData += c;
  }
  Serial.println(httpData);
  httpData = "";

  mySerial.println("AT+HTTPTERM");
  delay(1000);

  delay(60000);
}