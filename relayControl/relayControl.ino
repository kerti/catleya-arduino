#include <RH_ASK.h>
#include <SPI.h>

// address of this device
String devAddress = "RELAY.01";

// command constants
String cmdHead        = "[C]";
String cmdSet         = "SET";
String cmdSetAll      = "SETALL";
String cmdParamOn     = "ON";
String cmdParamOff    = "OFF";

// relay pin assignments
const uint8_t relays[8] = { 2, 3, 4, 5, 6, 7, 8, 9 };

// bit masking integers
const int mask[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };

// indicator pin assignment
const uint8_t indicatorPin = 13;

// ACK blink settings
const uint8_t ackBlinkRate  = 2;
const uint8_t ackBlinkTimes = 2;

// ASK settings
const uint16_t dataRate = 2000;
const uint8_t rxPin     = 12;
const uint8_t txPin     = 11;
const uint8_t pttPin    = 10;
const bool pttInverted  = false;

// the received message and its parsed components
String message;
String command;
String commandParams[16];

// the ASK driver
RH_ASK rh(dataRate, rxPin, txPin, pttPin, pttInverted);

void setup() {
  initializeSerial();
  initializeOutputPins();
  initializeRadiohead();
}

void loop() {
  receiveTransmission();
}

void initializeSerial() {
  // initialize serial port
  Serial.begin(9600);
  Serial.println("Setting up debugging...");
}

void initializeOutputPins() {
  // initialize all output pins
  for (uint8_t i = 0; i < sizeof(relays); i++) {
    digitalWrite(relays[i], HIGH);
    pinMode(relays[i], OUTPUT);
  }
  pinMode(indicatorPin, OUTPUT);
}

void initializeRadiohead() {
  rh.init();
  Serial.println("Setting up comms...");
  if (!rh.init()) {
    Serial.println("Radiohead initialization failed!");
  } 
  else {
    Serial.println("Comms ready to receive...");
  }
}

void receiveTransmission() {
  uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
  uint8_t buflen = sizeof(buf);

  if (rh.recv(buf, &buflen)) // Non-blocking
  {
    storeMessage(buf, buflen);
    printMessage(message);
    parseMessage();
    executeCommand();
    ackBlink();
  }
}

void storeMessage(const uint8_t* buf, uint8_t len) {
  char messageArr[len + 1];
  for (uint8_t i = 0; i < len; i++) {
    messageArr[i] = char(buf[i]);
  }
  messageArr[len] = '\0';
  message = messageArr;
}

void parseMessage() {
  if (message.length() > 0) {
    Serial.println("Parsing message...");

    uint8_t i = 0;
    char* pch;
    char charBuf[message.length() + 1];
    String tempComponent;
    
    message.toCharArray(charBuf, message.length() + 1);
    pch = strtok(charBuf, "|");

    while(pch != NULL) {
      tempComponent = pch;
      Serial.println(tempComponent);
      pch = strtok(NULL, "|");
      if (0 == i) {
        // this is the command part
        if (cmdHead != tempComponent) {
          Serial.println("Command header not found, breaking.");
          break;
        }
      } else if (1 == i) {
        if (devAddress != tempComponent) {
          Serial.println("Address mismatch, breaking.");
          break;
        }
      } else if (2 == i) {
        command = tempComponent;
      } else {
        commandParams[i - 3] = tempComponent;
      }
      i++;
    }
    
    Serial.println("Message parsing complete.");
  } 
  else {
    Serial.println("No message to parse, ignoring...");
  }
}

void executeCommand() {
  if (command.length() == 0) {
    Serial.println("Command not found, breaking.");
    return;
  } else {
    Serial.print("Executing command: ");
    Serial.println(command);
  }
  
  if (cmdSet == command) {
    executeSet();
    return;
  } else if (cmdSetAll == command) {
    executeSetAll();
    return;
  } else {
    Serial.println("Command not supported, breaking.");
    return;
  }
}

void executeSet() {
  uint8_t relayToSet = commandParams[0].toInt();
  if (relayToSet < -1 || relayToSet > 8) {
    Serial.println("Relay assignment not supported, breaking.");
    return;
  }
  
  if (cmdParamOn == commandParams[1] || cmdParamOff == commandParams[1]) {
    Serial.print("Setting relay # ");
    Serial.print(commandParams[0] + 1);
    Serial.print(" to ");
    Serial.print(commandParams[1]);
    Serial.println(".");
    setRelayState(relayToSet, cmdParamOn == commandParams[1]);
    ackBlink();
  } else {
    Serial.println("Command parameter not supported, breaking.");
    return;
  }
}

void executeSetAll() {
  Serial.println("Executing SETALL command...");
  int relayMask = commandParams[0].toInt();
  Serial.print("Relay activation mask: ");
  Serial.println(relayMask);
  int masked;
  boolean isActive;
  for (uint8_t i = 0; i < 8; i++) {
    masked = mask[i] & relayMask;
    isActive = masked == mask[i];
    Serial.print("Setting relay # ");
    Serial.print(i);
    Serial.print(" to ");
    Serial.print(isActive);
    Serial.println(".");
    setRelayState(i, isActive);
  }
  ackBlink();
}

void setRelayState(uint8_t relayIndex, boolean active) {
  if (active) {
    digitalWrite(relays[relayIndex], LOW);
  } else {
    digitalWrite(relays[relayIndex], HIGH);
  }
}


void printMessage(const char* prompt, const uint8_t* buf, uint8_t len) {
  #ifdef RH_HAVE_SERIAL
  uint8_t i;
  // Serial.print("Message length: ");
  // Serial.println(len);
  // Serial.print(prompt);
  for (i = 0; i < len; i++) {
    // Serial.print(char(buf[i]));
  }
  // Serial.println("");
  #endif
}

void printMessage(String message) {
  #ifdef RH_HVAVE_SERIAL
  // Serial.print("Message received: ");
  // Serial.println(message);
  #endif
}


void ackBlink() {
  for (uint8_t i = 0; i < ackBlinkTimes; i++) {
    digitalWrite(indicatorPin, HIGH);
    delay(500 / ackBlinkRate);
    digitalWrite(indicatorPin, LOW);
    delay(500 / ackBlinkRate);
  }
  // Serial.println("Transmission processing completed.");
  // Serial.println("---===---");
  // Serial.println("");
}

