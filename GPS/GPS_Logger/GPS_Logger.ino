#include <SD.h>
#include<SPI.h>
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(3,2);

Adafruit_GPS GPS(&mySerial);


String NMEA1;
String NMEA2;

char c;

int chipSelect = 10;

File mySensorData;


void setup() {
  Serial.begin(115200);
  GPS.begin(9600);
  GPS.sendCommand("$PGCMD,33,0*6D");
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  delay(1000); 
  
  SD.begin(chipSelect);

  readGPS();
  if (SD.exists("NMEA.txt")) {
    Serial.println("nmea.txt exist");
    mySensorData = SD.open("NMEA.txt", FILE_WRITE);
    mySensorData.print(GPS.hour);
    mySensorData.print(GPS.minute);
    mySensorData.print(GPS.seconds);
    mySensorData.close();
  }
}

void loop() {
  Serial.println("in loop");
  readGPS();
  mySensorData = SD.open("NMEA.txt", FILE_WRITE);
  mySensorData.println(NMEA1);
  mySensorData.println(NMEA2);
  mySensorData.close();
}

void readGPS() {
  
  clearGPS();
  Serial.println("here");
  while(!GPS.newNMEAreceived()) {
    c=GPS.read();
  }
  GPS.parse(GPS.lastNMEA());
  NMEA1=GPS.lastNMEA();

   while(!GPS.newNMEAreceived()) {
    c=GPS.read();
  }
  GPS.parse(GPS.lastNMEA());
  NMEA2=GPS.lastNMEA();

  Serial.println(NMEA1);
  Serial.println(NMEA2);
  
}

void clearGPS() {  
  while(!GPS.newNMEAreceived()) {
    c=GPS.read();
  }
  GPS.parse(GPS.lastNMEA());
  
  while(!GPS.newNMEAreceived()) {
    c=GPS.read();
  }
  GPS.parse(GPS.lastNMEA());

  while(!GPS.newNMEAreceived()) {
    c=GPS.read();
  }
  GPS.parse(GPS.lastNMEA());

}
