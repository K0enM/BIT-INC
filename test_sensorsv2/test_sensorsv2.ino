#include <AllThingsTalk_LoRaWAN.h>
#include <Wire.h>
//#include <MMA7660.h>
#include <Adafruit_BME280.h>
#include <Seeed_BME280.h>

#define debugSerial Serial
#define loraSerial Serial1

//OTAA Credentials
uint8_t DEVEUI[8] = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x04, 0xCC, 0xD4 };
uint8_t APPEUI[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t APPKEY[16] = { 0xAC, 0x22, 0xD8, 0x6F, 0xAD, 0x22, 0x90, 0x2C, 0x60, 0x9A, 0x37, 0xCA, 0x33, 0x54, 0xBF, 0xB7 };

CborPayload payload;

OTAACredentials credentials(DEVEUI, APPEUI, APPKEY);

LoRaModem modem(loraSerial, debugSerial, credentials);

//MMA7660 accelerometer;

const int motionPin = 20;

float soundValue, lightValue, temperature, humidity, pressure, airValue;    // Variables used to store our sensor data
unsigned int sendEvery = 300;                           // Creates a delay so the data is not constantly sent. 
uint8_t sensorType; 

Adafruit_BME280 tph1;                                   // Create an "tph" object which will be our TPH (Temperature, Pressure and Humidity) sensor
BME280 tph2;


void setup() {
  debugSerial.begin(57600);
  while (!modem.init()) {
    debugSerial.println("Error with initializing modem");  
  }                                             // Otherwise prints the text below to Serial
  modem.setAdaptiveDataRate(true);
  debugSerial.println("Modem initialized succesfully!");
  pinMode(motionPin, INPUT);
//  accelerometer.init();
  initTphSensor();
}

void initTphSensor() {
  if (tph1.begin()) {
    // Sensor will use Adafruit Library
    sensorType = 1;
  } else if (tph2.init()) {
    // Sensor will use Seeed Library
    sensorType = 2;
  } else {    
    debugSerial.println("Could not initialize TPH sensor, please check wiring.");
    sensorType = 0;
  }
}

void readSensors() {                                    // Function that we'll call when we want to read the data from all sensor
  if (sensorType == 1) {
    temperature = tph1.readTemperature();               // Read the temperature data from the BME280 (TPH) Sensor and save it into the "temperature" variable
    humidity    = tph1.readHumidity();                  // Read the humidity data from the BME280 (TPH) Sensor and save it into the "humidity" variable
    pressure    = tph1.readPressure()/100.0;            // Read the pressure data from the BME280 (TPH) Sensor, divide it by 100 and save it into the "pressure" variable
  } else if (sensorType == 2) {
    temperature = tph2.getTemperature();                // Read the temperature data from the BME280 (TPH) Sensor and save it into the "temperature" variable
    humidity    = tph2.getHumidity();                   // Read the humidity data from the BME280 (TPH) Sensor and save it into the "humidity" variable
    pressure    = tph2.getPressure()/100.0;             // Read the pressure data from the BME280 (TPH) Sensor, divide it by 100 and save it into the "pressure" variable
  } else {
    temperature = 0;
    humidity = 0;
    pressure = 0;
    debugSerial.println("TPH Sensor seems to be disconnected. Temperature/Humidity/Pressure Values sent will be 0.");
  }
}

void displaySensorValues() {                            // Function we'll call to output all the collected data via debug Serial (to see when you open Serial Monitor)
  debugSerial.println("-----------------------");       // A simple line to distinguish between old and new data easily

  debugSerial.print("Temperature: ");
  debugSerial.print(temperature);
  debugSerial.println(" °C");

  debugSerial.print("Humidity: ");
  debugSerial.print(humidity);
  debugSerial.println(" %");

  debugSerial.print("Pressure: ");
  debugSerial.print(pressure);
  debugSerial.println(" hPa");
  
  delay(200);                                           // A necessary delay so the serial output isn't cut off while outputting
}

void loop() {
  bool motion = digitalRead(motionPin);
  readSensors();
  displaySensorValues();

//  int8_t x;
//  int8_t y;
//  int8_t z;
//  float ax,ay,az;
//  accelerometer.getXYZ(&x,&y,&z);
//
//  debugSerial.print("x = ");
//  debugSerial.println(x); 
//  debugSerial.print("y = ");
//  debugSerial.println(y);   
//  debugSerial.print("z = ");
//  debugSerial.println(z);
//
//  accelerometer.getAcceleration(&ax,&ay,&az);
//  debugSerial.println("accleration of X/Y/Z: ");
//  debugSerial.print(ax);
//  debugSerial.println(" g");
//  debugSerial.print(ay);
//  debugSerial.println(" g");
//  debugSerial.print(az);
//  debugSerial.println(" g");
//  debugSerial.println("*************");
  
  if (motion) {
    payload.reset();
    debugSerial.println("Motion detected!");
    payload.set("motion", motion);
    bool res = modem.send(payload);
    delay(10000);  
  }
  delay(1000);
}
