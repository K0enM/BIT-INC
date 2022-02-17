#include <AllThingsTalk_LoRaWAN.h>                // Library for LoraWan
#include "ATT_GPS.h"                            // Library for the GPS module

#define debugSerial Serial
#define loraSerial Serial1
#define DEBUG_SERIAL_BAUD 57600
#define GPS_FIX_DELAY 30

//OTAA Credentials
uint8_t DEVEUI[8] = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x04, 0xCC, 0xD4 };
uint8_t APPEUI[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t APPKEY[16] = { 0xAC, 0x22, 0xD8, 0x6F, 0xAD, 0x22, 0x90, 0x2C, 0x60, 0x9A, 0x37, 0xCA, 0x33, 0x54, 0xBF, 0xB7 };

CborPayload payload;

OTAACredentials credentials(DEVEUI, APPEUI, APPKEY);

LoRaModem modem(loraSerial, debugSerial, credentials);

ATT_GPS gps(8, 9);

float prevLatitude, prevLongitude;
unsigned long sendNextAt = 0;

void setup() {
  debugSerial.begin(DEBUG_SERIAL_BAUD);

  while((!debugSerial) && (millis()) < 10000){}   // Wait for the Serial monitor to be open (so you can see all output) and if it isn't open in 10 seconds, run the program anyways
  while (!modem.init()) { delay(1000); }          // Initialize the modem. If it fails, retry every 1 second until it succeeds
  
  if (!modem.init()) {                                  // Writes the message below if the modem can't be initialized
    debugSerial.println("Could not initialize the modem. Check your keys.");
    exit(0);                                            // And exits
  } else {                                              // Otherwise prints the text below to Serial
    debugSerial.println("Model initialized succesfully!");
    debugSerial.println("Initializing GPS.....");
    readCoordinates();
    debugSerial.println("GPS Initialized!");
  }  
}

void readCoordinates() {
  while(gps.readCoordinates() == false) {
    debugSerial.print(gps.readCoordinates());
    delay(1000);
  }
  debugSerial.println();  
}


void sendCoordinates() {
  payload.reset();


  GeoLocation geoLocation(gps.latitude, gps.longitude, gps.altitude);
  payload.set("geo_location", geoLocation);

  debugSerial.print("Longitude: ");
  debugSerial.print(gps.longitude, 4);
  debugSerial.print(", Latitude: ");
  debugSerial.print(gps.latitude, 4);
  debugSerial.print(", Altitude: ");
  debugSerial.print(gps.altitude);
  debugSerial.print(", Time: ");
  debugSerial.println(gps.timestamp);

  modem.send(payload);
}

void loop() {
  if (sendNextAt < millis()) {
    prevLatitude = gps.latitude;
    prevLongitude = gps.longitude;
    sendCoordinates();
  }

  sendNextAt = millis() + GPS_FIX_DELAY*1000;
}
