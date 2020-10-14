//#include <SPI.h>
#include <SD.h>

String input = "";
boolean string_is_complete = false;
String header = "GPRMC";

File log_file;

void setup() {
  Serial.begin(9600);
  while(!Serial);
  
  input.reserve(200);

  Serial.println("Initializing SD card...");
  if(!SD.begin(10)) {
    Serial.println("Initialization Failed!");
    while(1);
  }
  Serial.println("Initialization Completed.");
  log_file = SD.open("log.txt", FILE_WRITE);
  if(log_file) {
    log_file.close();
    Serial.println("done.");

  } else {
    Serial.println("error opening test.txt");
  }
}

void loop() {
  while(string_is_complete) {
    if(input.substring(1,6) == header) {
      //UTC Time
      String hour = input.substring(7,9);
      String minute = input.substring(9,11);
      String second = input.substring(11,13);

      // Status, V: Navigation reciever warning; A: Data valid
      String status = input.substring(17,18);

      // Latitude
      ///// to do: remove first 0
      String lat_deg = input.substring(19,21);
      String lat = input.substring(21,29);
      String lat_dir = input.substring(30,31);
      ///// to do: func for convert n to north

      // Longitude
      ///// to do: remove first 0
      String lon_deg = input.substring(32,34);
      String lon = input.substring(34,43);
      String lon_dir = input.substring(44,45);
      ///// to do: func for convert n to north

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
    }

    log_file = SD.open("log.txt", FILE_WRITE);
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
