//#include <SPI.h>
#include <SD.h>


#define NMEA "NMEA.txt"
#define GPS_Data "Data.txt"
#define signal_header  "GPRMC"
#define cspin 10

String input = "";
boolean string_is_complete = false;
long latitude;
long longitude;

File log_file;

void setup() {
  Serial.begin(9600);
  while(!Serial);
  
  input.reserve(200);

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

  //warmup_gsm();
  //connect_tcp();
}

void loop() {
  while(string_is_complete) {
    if(input.substring(1,6) == signal_header) {
      parse_nmea();
      //send_to_server();
    }

    log_file = SD.open(NMEA, FILE_WRITE);
    log_file.print(input);
    log_file.close();
    
    input = "";
    string_is_complete = false;
  }
  
}

void serialEvent() {
  while(Serial.available()) {
    char inChar = (char) Serial.read();
    input += inChar;
    if(inChar == '\n') {
      string_is_complete = true;
    }
  }
}

void parse_nmea() {
  /*String temp1 = input.substring(53,59);
  String temp2 = input.substring(7,13);
  strcmp(time, temp1);
  strcat(time, temp2);
  Serial.println(time);*/
  
  latitude = input.substring(19,29).toFloat()*100000;
  longitude = input.substring(32,43).toFloat()*100000;

  Serial.println(latitude);   //## ##.#####
  Serial.println(longitude);  //### ##.#####
}

void parse_nmea_detail() {
  //UTC Time
  String hour = input.substring(7,9);
  String minute = input.substring(9,11);
  String second = input.substring(11,13);

  // Status, V: Navigation reciever warning; A: Data valid
  String status = input.substring(17,18);

  // Latitude
  String lat_deg = input.substring(19,21);
  String lat = input.substring(21,29);
  String lat_dir = input.substring(30,31);
 
  // Longitude
  String lon_deg = input.substring(32,35);
  String lon = input.substring(35,43);
  String lon_dir = input.substring(44,45);

  // Speed over Ground in Knots
  String SOG = input.substring(46,451);

  // Course over ground in degrees
  String COG = input.substring(49,54);

  //Date
  String day = input.substring(53,55);
  String month = input.substring(55,57);
  String year = input.substring(57,59);

  // Mode Indicator, N: No Fix; A: Autonomous GNSS Fix; D: Differential GNSS Fix; E = Estimated/Dead Reckoning Fix
  String mode = input.substring(62,63);
  // Checksum
  String CS = input.substring(64,66);

  // Print Data
  Serial.println(day + "/" + month + "/" + year);
  Serial.println("UTC Time: " + hour + ":" + minute + ":" + second);
  Serial.println(lat_deg + " deg, " + lat + "'" + lat_dir);
  Serial.println(lon_deg + " deg, " + lon + "'" + lon_dir);

  // Save Data
  log_file = SD.open(GPS_Data, FILE_WRITE);
  log_file.print(year + "/" + month + "/" + day);
  log_file.print(" UTC Time: " + hour + ":" + minute + ":" + second);
  log_file.print(lat_deg + " deg, " + latitude + "'" + lat_dir);
  log_file.println(lon_deg + " deg, " + longitude + "'" + lon_dir);
  log_file.close();
}
