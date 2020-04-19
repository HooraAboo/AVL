String input = "";
bool string_is_complete = false;
String header = "GPRMC";

void setup() {
  Serial.begin(9600);
  input.reserve(200);
}

void loop() {
  if(!string_is_complete) {
    Serial.println("kir");
    if(input.substring(1,6) == header) {
      //UTC Time
      String hour = input.substring(7,9);
      String minute = input.substring(9,11);
      String second = input.substring(11,13);
      Serial.println("UTC Time: " + hour + ":" + minute + ":" + second);

      // Status, V: Navigation reciever warning; A: Data valid
      String status = input.substring(14,15);

      // Latitude
      ///// to do: remove first 0
      String lat_deg = input.substring(16,18);
      String lat = input.substring(18,26);
      String lat_dir = input.substring(27,28);
      ///// to do: func for convert n to north
      Serial.println(lat_deg + "," + lat + "'" + lat_dir);

      // Longitude
      ///// to do: remove first 0
      String lon_deg = input.substring(29,31);
      String lon = input.substring(31,40);
      String lon_dir = input.substring(41,42);
      ///// to do: func for convert n to north
      Serial.println(lat_deg + "," + lat + "'" + lat_dir);

      // Speed over Ground in Knots
      String SOG = input.substring(43,48);

      // Course over ground in degrees
      String COG = input.substring(49,54);

      //Date
      String day = input.substring(55,57);
      String month = input.substring(57,59);
      String year = input.substring(59,60);
      Serial.println(day + "/" + month + "/" + "20" + year);

      // Mode Indicator, N: No Fix; A: Autonomous GNSS Fix; D: Differential GNSS Fix; E = Estimated/Dead Reckoning Fix
      String mode = input.substring(63,64);

      // Checksum
      String CS = input.substring(65,67);
    }
  }
}
