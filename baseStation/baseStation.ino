#include <Ethernet.h>
#include <PubSubClient.h>
#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile

// Update these with values suitable for your network.
byte mac[]    = { 0x16, 0x07, 0x84, 0x01, 0x02, 0x87 };
EthernetClient ethClient;

// MQTT Client Settings
byte server[] = { 192, 168, 1, 3 };
byte ip[]     = { 192, 168, 1, 81 };
int port      = 1883;
char* topic   = "HomeAutomation/RelayControl";

// the MQTT client
PubSubClient client(server, port, callback, ethClient);

// ASK settings
const uint16_t dataRate = 2000;
const uint8_t rxPin     = 3;
const uint8_t txPin     = 4;
const uint8_t pttPin    = 5;
const bool pttInverted  = false;

// the ASK driver
RH_ASK rh(dataRate, rxPin, txPin, pttPin, pttInverted);

void setup()
{
  // Serial.begin(9600);
  
  if (Ethernet.begin(mac) == 0) {
    // Serial.println("Failed to configure Ethernet using DHCP.");
    Ethernet.begin(mac, ip);
  }
  
  // Serial.println("Connecting...");
  
  if (client.connect("arClient")) {
    client.subscribe(topic);
    // Serial.print("Client connected, subscribing to topic: ");
    // Serial.println(topic);
  } else {
    // Serial.println("Client failed to connect!");
  }
  
  if (!rh.init()) {
     // Serial.println("init failed");
  }
  
}

void loop()
{
  client.loop();
}

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  // Serial.print("MQTT Message received: ");
  char messageToTransmit[length];
  for (int i = 0; i < length; i++) {
    messageToTransmit[i] = char(payload[i]);
  }
  messageToTransmit[length] = '\0';
  String str = String(messageToTransmit);
  // Serial.println("");
  
  if (sizeof(messageToTransmit) > 0) {
    transmitMessage(messageToTransmit);
  }
}

void transmitMessage(char* message) {
  // Serial.print("Transmitting message via RF: ");
  char messageToTransmit[strlen(message) - 2];
  for (int i = 0; i < strlen(message) - 2; i++) {
    messageToTransmit[i] = char(message[i]);
  }
  messageToTransmit[strlen(message) - 3] = char('\0');
  // Serial.println(messageToTransmit);
  rh.send((uint8_t *) messageToTransmit, strlen(messageToTransmit));
  rh.waitPacketSent();
}
