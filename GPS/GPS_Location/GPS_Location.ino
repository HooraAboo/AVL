String input_string = ""; // a string to hold incoming data
boolean string_is_complete = false; // whether the string is complete
String signal = "$GPGLL";

void setup() {
    // initialize serial:
    Serial.begin(9600);
    // reserve 200 bytes for the input_string:
    input_string.reserve(200);
}

void loop() {
    // print the string when a newline arrives:
    if (string_is_complete) {
        String BB = input_string.substring(0, 6);
        if (BB == signal) {
            String LAT = input_string.substring(7, 17);
            int LATperiod = LAT.indexOf('.');
            int LATzero = LAT.indexOf('0');
            if (LATzero == 0) {
                LAT = LAT.substring(1);
            }

            String LON = input_string.substring(20, 31);
            int LONperiod = LON.indexOf('.');
            int LONTzero = LON.indexOf('0');
            if (LONTzero == 0) {
                LON = LON.substring(1);
            }

            Serial.println(LAT);
            Serial.println(LON);

        }

        // Serial.println(input_string);
        // clear the string:
        input_string = "";
        string_is_complete = false;
    }
}

/*
SerialEvent occurs whenever a new data comes in the
hardware serial RX. This routine is run between each
time loop() runs, so using delay inside loop can delay
response. Multiple bytes of data may be available.
*/
void serialEvent() {
    while (Serial.available()) {
        // get the new byte:
        char inChar = (char) Serial.read();
        // add it to the input_string:
        input_string += inChar;
        // if the incoming character is a newline, set a flag
        // so the main loop can do something about it:
        if (inChar == '\n') {
            string_is_complete = true;
        }
    }
}
