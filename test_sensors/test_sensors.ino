#include <AllThingsTalk_LoRaWAN.h>
#include <AirQuality.h>
#include "ATT_GPS.h"
#define debugSerial Serial
#define loraSerial Serial1


//OTAA Credentials
uint8_t DEVEUI[8] = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x04, 0xCC, 0xD4 };
uint8_t APPEUI[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t APPKEY[16] = { 0xAC, 0x22, 0xD8, 0x6F, 0xAD, 0x22, 0x90, 0x2C, 0x60, 0x9A, 0x37, 0xCA, 0x33, 0x54, 0xBF, 0xB7 };

CborPayload payload;

OTAACredentials credentials(DEVEUI, APPEUI, APPKEY);

LoRaModem modem(loraSerial, debugSerial, credentials);

int buttonPin = 20;
int lightSensorPin = A0;
int gpsPin1 = 4;
int gpsPin2 = 5;

AirQuality airQualitySensor;
ATT_GPS gps(gpsPin1, gpsPin2);

void setup() {
  debugSerial.begin(57600);

  while (!modem.init()) {
    debugSerial.println("Error with initializing modem");  
  }                                             // Otherwise prints the text below to Serial
  debugSerial.println("Modem initialized succesfully!");
  pinMode(buttonPin, INPUT);                            // Initialize the button pin on the board
  airQualitySensor.init(A4);                         // Initialize the air quality sensor
//  debugSerial.println("Initiliazing GPS...");
//  readCoordinates();
//  debugSerial.println("GPS Initialized");
}

void loop() {
    modem.wakeUp();
    bool buttonState = digitalRead(buttonPin);
    debugSerial.println("Now sending...");
    payload.reset();                                  // Resets the payload in case there's anything left
    payload.set("button", buttonState);                    // Sets the payload with our entity name "3" and the value of our button (true/1)
    payload.set("air_quality", airQualitySensor.slope());
    gps.readCoordinates();
//    GeoLocation geoLocation(gps.latitude, gps.longitude, gps.altitude);
//    debugSerial.println(gps.latitude);
//    payload.set("geo_location", geoLocation);
    int lightValue = analogRead(lightSensorPin);
    debugSerial.println(lightValue);
    payload.set("light_value", lightValue);
    bool res = modem.send(payload);
    debugSerial.println(res);
    debugSerial.println(modem.getDataRate());
    modem.sleep(30000);
    delay(35000);
}

void readCoordinates() {
  while(gps.readCoordinates() == false) {
    debugSerial.print(gps.readCoordinates());
    delay(1000);
  }
  debugSerial.println();  
}


ISR(TIMER2_OVF_vect) {
  if (airQualitySensor.counter = 122) {
    airQualitySensor.last_vol = airQualitySensor.first_vol;
    airQualitySensor.first_vol = analogRead(A4);
    airQualitySensor.counter = 0;
    airQualitySensor.timer_index = 1;
    PORTB=PORTB ^ 0x20;
  } else {
      airQualitySensor.counter++;
    }
}
