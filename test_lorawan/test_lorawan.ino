#include <AllThingsTalk_LoRaWAN.h>
#define debugSerial Serial
#define loraSerial Serial1


//OTAA Credentials
uint8_t DEVEUI[8] = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x04, 0xCC, 0xD4 };
uint8_t APPEUI[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t APPKEY[16] = { 0xAC, 0x22, 0xD8, 0x6F, 0xAD, 0x22, 0x90, 0x2C, 0x60, 0x9A, 0x37, 0xCA, 0x33, 0x54, 0xBF, 0xB7 };

CborPayload payload;

int   buttonPin           = 20;                          // Pin number of the button
bool  prevButtonState     = false; 

OTAACredentials credentials(DEVEUI, APPEUI, APPKEY);

LoRaModem modem(loraSerial, debugSerial, credentials);
 
void setup() {
  debugSerial.begin(57600);
  pinMode(buttonPin, INPUT);                            // Initialize the button pin on the board
  if (!modem.init()) {                                  // Writes the message below if the modem can't be initialized
    debugSerial.println("Could not initialize the modem. Check your keys.");
    exit(0);                                            // And exits
  } else {                                              // Otherwise prints the text below to Serial
    debugSerial.println("Model initialized succesfully!");
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  pushButton();
}

void pushButton() {                                     // Name of our function that we'll call from the loop function below
  bool buttonState = digitalRead(buttonPin);            // Read the status of the button and save it in 'buttonState'
  if (buttonState == true) {                            // If the button is pressed
    if (prevButtonState == false) {                     // If the button previously wasn't pressed. This way the code below won't be run multiple times if you press and keep pressing the button.
      debugSerial.println("Button press detected, now sending...");
      payload.reset();                                  // Resets the payload in case there's anything left
      payload.set("button", buttonState);                    // Sets the payload with our entity name "3" and the value of our button (true/1)
      modem.send(payload);                              // Sends the payload via LoRaWAN
      prevButtonState = true;                           // Sets the previous button state to pressed (so the 'if' statement above fails) 
    }
  } else { 
    prevButtonState = false;                            // If the button is not being pressed, set the value of previous button state to False
  }
}
