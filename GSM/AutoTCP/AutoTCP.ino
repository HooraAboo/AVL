#include <SoftwareSerial.h>
#include <ArduinoJson.h>

#define deviceID "dfjghlrfyuf"

//StaticJsonDocument<512> doc;
SoftwareSerial Serial_GSM(3, 2);

unsigned char buffer[64];
int count = 0;

long latitude;
long longitude;

void setup()
{
  Serial_GSM.begin(9600);
  Serial.begin(9600);
  //DynamicJsonDocument doc(1024);
  //delay(3000);
  while(!Serial || !Serial_GSM);
  //Serial.println("Serials begin");

  warmup_gsm();

}

void loop()
{
  while (Serial_GSM.available())
  {
    buffer[count++] = Serial_GSM.read();
    if (count == 64) break;
  }

  Serial.write(buffer, count);
  clear_buffer_array();
  count = 0;

  

      
  if (Serial.available())
  {
    byte b = Serial.read();
 
    if (b == '^') {
      
    }
    else
      Serial_GSM.write(b);
  }
}

void ShowSerialData()
{
  while (Serial_GSM.available() != 0)
    Serial.write(Serial_GSM.read());
}

void clear_buffer_array()
{
  for (int i = 0; i < count; i++)
  {
    buffer[i] = NULL;
  }
}

void warmup_gsm() {
  Serial_GSM.println("AT+CIPSHUT");
  delay(1000);
  ShowSerialData();

  Serial_GSM.println("AT+CSTT=\"MCINET\"");
  delay(1000);
  ShowSerialData();

  Serial_GSM.println("AT+CIICR");
  delay(1000);  
  ShowSerialData();

  Serial_GSM.println("AT+CIFSR");
  delay(1000);  
  ShowSerialData();
}

void connect_tcp() {
  Serial_GSM.println("AT+CIPSTART=\"TCP\",\"server.fanaplab.com\",5000");
  delay(3000);
  ShowSerialData(); 
}

void send_to_server() {
  //const int capacity = JSON_ARRAY_SIZE(2) + 4 * JSON_OBJECT_SIZE(2);
  StaticJsonDocument<256> doc;

  // Header
  JsonArray header = doc.createNestedArray("header");

  JsonObject header1 = header.createNestedObject();
  header1["key"] = "Server-Access-Token";
  header1["value"] = "YWRtaW5AZ21haWwuY29tTU1UUlI5Tk9nb2dxMFgxTFAyQm55NjZ0UU1mbXR6";
  header1["description"] = "توکن دسترسی به سرور - ضروری";
  header1["type"] = "text";

  // Body
  JsonArray body = doc.createNestedArray("body");

  JsonObject body1 = body.createNestedObject();
  body1["key"] = "code";
  body1["value"] = deviceID;
  body1["description"] = "شناسه اتوبوس - الزامی - از نوع استرینگ";
  body1["type"] = "text";

  JsonObject body2 = body.createNestedObject();
  body2["key"] = "lat";
  body2["value"] = latitude;
  body2["description"] = "طول جغرافیایی - الزامی - از نوع استرینگ";
  body2["type"] = "text";

  JsonObject body3 = body.createNestedObject();
  body3["key"] = "long";
  body3["value"] = longitude;
  body3["description"] = "عرض جغرافیایی - الزامی - از نوع استرینگ";
  body3["type"] = "text";

  //serializeJson(doc, Serial);
  String sendtoserver;
  serializeJsonPretty(doc, Serial_GSM);
  //delay(4000);

  
  ShowSerialData();

  Serial_GSM.write(0x1a);
}
