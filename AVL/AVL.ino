#include <SD.h>

#include <SoftwareSerial.h>
#include <ArduinoJson.h>

#define busID "dqCUPXuQtA1";

SoftwareSerial gsm_serial(3, 2);

String input = "";
boolean string_is_complete = false;
//String header = "GPRMC";

// NMEA Parameters
float lat = 0;
float lon = 0;


File log_file;

void setup() {
  Serial.begin(9600);
  gsm_serial.begin(9600);
  
  while(!Serial || !gsm_serial);

  input.reserve(200);
  
  Serial.println("Initializing SD card...");
  while(!SD.begin(10)) {
    Serial.println("Initialization Failed!");
  }
  Serial.println("Initialization Completed.");
  /*log_file = SD.open("log.txt", FILE_WRITE);
  if(log_file) {
    log_file.close();
    Serial.println("done.");
  } else {
    Serial.println("error opening test.txt");
  }*/
  connect_tcp();
}

void loop() {
  while(string_is_complete) {
    if(input.substring(1,6) == "GPRMC") {
      parse_nmea();
      send_data();
    }

    log_file = SD.open("log.txt", FILE_WRITE);
    log_file.print(input);
    log_file.close();
    
    input = "";
    string_is_complete = false;
  }
  
}

void serial_event() {
  while(Serial.available()) {
    char inChar = (char) Serial.read();
    input += inChar;
    if(inChar == '\n') {
      string_is_complete = true;
    }
  }
}

void connect_tcp() {
  gsm_serial.println("AT+CIPSHUT");
  delay(2000);
  ShowSerialData();

  gsm_serial.println("AT+CSTT=\"MCINET\"");
  ShowSerialData();

  gsm_serial.println("AT+CIICR");
  delay(3000);
  ShowSerialData();

  gsm_serial.println("AT+CIFSR");
  ShowSerialData();

  gsm_serial.println("AT+CIPSTART=\"TCP\",\"server.fanaplab.com\",5000");
  delay(3000);
  ShowSerialData();
}

void close_tcp() {
  gsm_serial.println("AT+CIFSR");
  delay(3000);
  ShowSerialData();
}

void send_data() {
  gsm_serial.println("AT+CIPSEND");
  delay(3000);
  ShowSerialData();
  
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
  body1["value"] = busID;
  body1["description"] = "شناسه اتوبوس - الزامی - از نوع استرینگ";
  body1["type"] = "text";

  JsonObject body2 = body.createNestedObject();
  body2["key"] = "lat";
  body2["value"] = lat;
  body2["description"] = "طول جغرافیایی - الزامی - از نوع استرینگ";
  body2["type"] = "text";

  JsonObject body3 = body.createNestedObject();
  body3["key"] = "long";
  body3["value"] = lon;
  body3["description"] = "عرض جغرافیایی - الزامی - از نوع استرینگ";
  body3["type"] = "text";

  //serializeJson(doc, Serial);
  serializeJsonPretty(doc, gsm_serial);
  ShowSerialData();

  //end of send message CTRL+Z
  gsm_serial.write(0x1a);
}

void parse_nmea() {
  //UTC Time
  String time = input.substring(7,13);
  /*hour = input.substring(7,9);
  minute = input.substring(9,11);
  second = input.substring(11,13);*/
  
  // Status, V: Navigation reciever warning; A: Data valid
  String status = input.substring(17,18);

  // Latitude
  ///// to do: remove first 0
  String lat_deg = input.substring(19,21);
  String latitude = input.substring(21,29);
  lat = latitude.toFloat();
  String lat_dir = input.substring(30,31);
  ///// to do: func for convert n to north

  // Longitude
  ///// to do: remove first 0
  String lon_deg = input.substring(32,34);
  String longitude = input.substring(34,43);
  lon = longitude.toFloat();
  String lon_dir = input.substring(44,45);
  ///// to do: func for convert n to north

  // Speed over Ground in Knots
  String SOG = input.substring(46,451);

  // Course over ground in degrees
  String COG = input.substring(49,54);

  //Date
  String date = input.substring(53,59);
  /*String day = input.substring(53,55);
  String month = input.substring(55,57);
  String year = input.substring(57,59);*/
  
  // Mode Indicator, N: No Fix; A: Autonomous GNSS Fix; D: Differential GNSS Fix; E = Estimated/Dead Reckoning Fix
  String mode = input.substring(62,63);

  // Checksum
  String CS = input.substring(64,66);

  /*// Print Data
  Serial.println(day + "/" + month + "/" + year);
  Serial.println("UTC Time: " + hour + ":" + minute + ":" + second);
  Serial.println(lat_deg + " deg, " + lat + "'" + lat_dir);
  Serial.println(lon_deg + " deg, " + lon + "'" + lon_dir);*/
}

void ShowSerialData()
{
  // Print serial data
  while (gsm_serial.available() != 0)
    Serial.write(gsm_serial.read());
}

void manage_state() {
  while(gsm_serial.available()) {
    if (gsm_serial.available() == ""){
      
    } else if (gsm_serial.available() == "") {
      
    } else if (gsm_serial.available() == "") {
      
    } else if (gsm_serial.available() == "") {
      
    } else if (gsm_serial.available() == "") {
      
    } else if (gsm_serial.available() == "") {
      
    } else if (gsm_serial.available() == "") {
      
    } else if (gsm_serial.available() == "") {
      
    }
  }
}
