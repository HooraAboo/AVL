#include <SD.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

#define NMEA "NMEA.txt"
#define GPS_Data "Data.txt"
#define signal_header  "$GPRMC"
#define busID "dfjghlrfyuf"

String gps_input = "";
boolean string_is_complete = false;
float latitude;
float longitude;

SoftwareSerial Serial_GSM(3, 2);
//StaticJsonDocument<256> doc;

File log_file;

void setup() {
  Serial.begin(9600);
  Serial_GSM.begin(9600);
  while(!Serial);
  
  gps_input.reserve(200);


  Serial.println("Initializing SD card...");
  while (!SD.begin(10)) {
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
    if(gps_input.substring(0,6) == signal_header) {
      parse_nmea();
      send_to_server();
    }

    log_file = SD.open(NMEA, FILE_WRITE);
    log_file.print(gps_input);
    log_file.close();
    
    gps_input = "";
    string_is_complete = false;
  }
}

void serial_event() {
  while(Serial.available()) {
    char inChar = (char) Serial.read();
    gps_input += inChar;
    if(inChar == '\n') {
      string_is_complete = true;
    }
  }
}

void parse_nmea () {
    //UTC Time
    String hour = gps_input.substring(7,9);
    String minute = gps_input.substring(9,11);
    String second = gps_input.substring(11,13);

    // Status, V: Navigation reciever warning; A: Data valid
    String status = gps_input.substring(17,18);

    // Latitude
    ///// to do: remove first 0
    String lat_deg = gps_input.substring(19,21);
    String lat = gps_input.substring(21,29);
    latitude = lat.toFloat();
    String lat_dir = gps_input.substring(30,31);
    ///// to do: func for convert n to north

    // Longitude
    ///// to do: remove first 0
    String lon_deg = gps_input.substring(32,35);
    String lon = gps_input.substring(35,43);
    longitude = lon.toFloat();
    String lon_dir = gps_input.substring(44,45);
    ///// to do: func for convert n to north

    // Speed over Ground in Knots
    String SOG = gps_input.substring(46,451);
    // Course over ground in degrees
    String COG = gps_input.substring(49,54);

    //Date
    String day = gps_input.substring(53,55);
    String month = gps_input.substring(55,57);
    String year = gps_input.substring(57,59);

    // Mode Indicator, N: No Fix; A: Autonomous GNSS Fix; D: Differential GNSS Fix; E = Estimated/Dead Reckoning Fix
    String mode = gps_input.substring(62,63);

    // Checksum
    String CS = gps_input.substring(64,66);

    // Save GPS Data
    log_file = SD.open(GPS_Data, FILE_WRITE);
    log_file.println(hour + ":" + minute + ":" + second);
    log_file.println(lat_deg + " deg, " + lat + "'" + lat_dir);
    log_file.println(lon_deg + " deg, " + lon + "'" + lon_dir);
    log_file.close();
      
    // Print GPS Data
    Serial.println("UTC Time: " + hour + ":" + minute + ":" + second);
    Serial.println(lat_deg + " deg, " + lat + "'" + lat_dir);
    Serial.println(lon_deg + " deg, " + lon + "'" + lon_dir);

}

void warmup_gsm() {
  Serial_GSM.println("AT+CIPSHUT");
  ShowSerialData();

  Serial_GSM.println("AT+CSTT=\"MCINET\"");
  ShowSerialData();

  Serial_GSM.println("AT+CIICR");
  ShowSerialData();

  Serial_GSM.println("AT+CIFSR");
  ShowSerialData();
}

void connect_tcp() {
  Serial_GSM.println("AT+CIPSTART=\"TCP\",\"server.fanaplab.com\",5000");
  ShowSerialData(); 
}
void send_to_server() {
  Serial_GSM.println("AT+CIPSEND");
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
  body2["value"] = latitude;
  body2["description"] = "طول جغرافیایی - الزامی - از نوع استرینگ";
  body2["type"] = "text";
  JsonObject body3 = body.createNestedObject();
  body3["key"] = "long";
  body3["value"] = longitude;
  body3["description"] = "عرض جغرافیایی - الزامی - از نوع استرینگ";
  body3["type"] = "text";
  
  serializeJsonPretty(doc, Serial_GSM);
  ShowSerialData();
}

void ShowSerialData()
{
  while (Serial_GSM.available() != 0)
    Serial.write(Serial_GSM.read());
}

/*void update_serial() {
  delay(500);

  //Forward what Software Serial received to Serial Port
  while (gsm_serial.available()) {
    Serial.write(gsm_serial.read());
  }

  //Forward what Serial received to Software Serial Port
  while(Serial.available()) {
//    gsm_serial.write(Serial.read());
  }
}*/
