#include <SD.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

#define NMEA "NMEA.txt"
#define GPS_Data "Data.txt"
#define signal_header  "GPRMC"
#define deviceID "dfjghlrfyuf"
#define cspin 10

String GPS_input = "";
boolean string_is_complete = false;
long latitude;
long longitude;

SoftwareSerial Serial_GSM(3, 2);

File log_file;

void setup() {
  Serial.begin(9600);
  Serial_GSM.begin(9600);
  while(!Serial);
  
  GPS_input.reserve(200);

  Serial.println("Initializing SD card...");
  if (!SD.begin(cspin)) {
    Serial.println("Initialization Failed!");
    delay(5000);
  }
  Serial.println("Initialization Completed.");
  
  log_file = SD.open(NMEA, FILE_WRITE);
  log_file.println("<------------------------- New Session ------------------------->");
  log_file.close();

  log_file = SD.open(GPS_Data, FILE_WRITE);
  log_file.println("<------------------------- New Session ------------------------->");
  log_file.close();

  warmup_gsm();
  connect_tcp();
}

void loop() {
  while(string_is_complete) {
    if(GPS_input.substring(1,6) == signal_header) {
      parse_nmea();
      send_to_server();
    }

    log_file = SD.open(NMEA, FILE_WRITE);
    log_file.print(GPS_input);
    log_file.close();
    
    GPS_input = "";
    string_is_complete = false;
  } 
}

void serialEvent() {
  while(Serial.available()) {
    char inChar = (char) Serial.read();
    GPS_input += inChar;
    if(inChar == '\n') {
      string_is_complete = true;
    }
  }
}

void parse_nmea() {
  /*String temp1 = GPS_input.substring(53,59);
  String temp2 = GPS_input.substring(7,13);
  strcmp(time, temp1);
  strcat(time, temp2);
  Serial.println(time);*/
  
  latitude = GPS_input.substring(19,29).toFloat()*100000;
  longitude = GPS_input.substring(32,43).toFloat()*100000;

  Serial.println(latitude);   //## ##.#####
  Serial.println(longitude);  //### ##.#####
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
  Serial_GSM.println("AT+CIPSEND");
  ShowSerialData();

  StaticJsonDocument<128> doc;
  
  // Header
  JsonArray header = doc.createNestedArray("header"); 
  JsonObject header1 = header.createNestedObject();
  header1["key"] = "Server-Access-Token";
  header1["value"] = "YWRtaW5AZ21haWwuY29tTU1UUlI5Tk9nb2dxMFgxTFAyQm55NjZ0UU1mbXR6";
  
  // Body
  JsonArray body = doc.createNestedArray("body");
  JsonObject body1 = body.createNestedObject();
  body1["key"] = "code";
  body1["value"] = deviceID;
  JsonObject body2 = body.createNestedObject();
  body2["key"] = "lat";
  body2["value"] = latitude;
  JsonObject body3 = body.createNestedObject();
  body3["key"] = "long";
  body3["value"] = longitude;
  
  serializeJsonPretty(doc, Serial_GSM);
  ShowSerialData();
}

void ShowSerialData() {
  while (Serial_GSM.available() != 0)
    Serial.write(Serial_GSM.read());
}

void parse_nmea_detail() {
  //UTC Time
  String hour = GPS_input.substring(7,9);
  String minute = GPS_input.substring(9,11);
  String second = GPS_input.substring(11,13);

  // Status, V: Navigation reciever warning; A: Data valid
  String status = GPS_input.substring(17,18);

  // Latitude
  String lat_deg = GPS_input.substring(19,21);
  String lat = GPS_input.substring(21,29);
  String lat_dir = GPS_input.substring(30,31);
 
  // Longitude
  String lon_deg = GPS_input.substring(32,35);
  String lon = GPS_input.substring(35,43);
  String lon_dir = GPS_input.substring(44,45);

  // Speed over Ground in Knots
  String SOG = GPS_input.substring(46,451);

  // Course over ground in degrees
  String COG = GPS_input.substring(49,54);

  //Date
  String day = GPS_input.substring(53,55);
  String month = GPS_input.substring(55,57);
  String year = GPS_input.substring(57,59);

  // Mode Indicator, N: No Fix; A: Autonomous GNSS Fix; D: Differential GNSS Fix; E = Estimated/Dead Reckoning Fix
  String mode = GPS_input.substring(62,63);
  // Checksum
  String CS = GPS_input.substring(64,66);

  // Print Data
  Serial.println(day + "/" + month + "/" + year);
  Serial.println("UTC Time: " + hour + ":" + minute + ":" + second);
  Serial.println(lat_deg + " deg, " + lat + "'" + lat_dir);
  Serial.println(lon_deg + " deg, " + lon + "'" + lon_dir);

  // Save Data
  log_file = SD.open(GPS_Data, FILE_WRITE);
  log_file.print(year + "/" + month + "/" + day);
  log_file.print(" UTC Time: " + hour + ":" + minute + ":" + second);
  log_file.print(lat_deg + " deg, " + lat + "'" + lat_dir);
  log_file.println(lon_deg + " deg, " + lon + "'" + lon_dir);
  log_file.close();
}
