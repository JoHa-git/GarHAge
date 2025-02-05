/*   
 * GarHAge
 * a Home-Automation-friendly ESP8266-based MQTT Garage Door Controller
 * Licensed under the MIT License, Copyright (c) 2017 marthoc
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "config.h"

// Mapping NodeMCU Ports to Arduino GPIO Pins
// Allows use of NodeMCU Port nomenclature in config.h
#define D0 16
#define D1 5
#define D2 4
#define D3 0
#define D4 2
#define D5 14
#define D6 12 
#define D7 13
#define D8 15

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

const boolean static_ip = STATIC_IP;
IPAddress ip(IP);
IPAddress gateway(GATEWAY);
IPAddress subnet(SUBNET);

const char* mqtt_broker = MQTT_BROKER;
const char* mqtt_clientId = MQTT_CLIENTID;
const char* mqtt_username = MQTT_USERNAME;
const char* mqtt_password = MQTT_PASSWORD;

const boolean activeHighRelay = ACTIVE_HIGH_RELAY;

const char* door1_alias = DOOR1_ALIAS;
const char* mqtt_door1_action_topic = MQTT_DOOR1_ACTION_TOPIC;
const char* mqtt_door1_status_topic = MQTT_DOOR1_STATUS_TOPIC;
const int door1_openPin = DOOR1_OPEN_PIN;
const int door1_closePin = DOOR1_CLOSE_PIN;
const int door1_statusPin = DOOR1_STATUS_PIN;
const char* door1_statusSwitchLogic = DOOR1_STATUS_SWITCH_LOGIC;

const char* door2_alias = DOOR2_ALIAS;
const char* mqtt_door2_action_topic = MQTT_DOOR2_ACTION_TOPIC;
const char* mqtt_door2_status_topic = MQTT_DOOR2_STATUS_TOPIC;
const int door2_statusPin = DOOR2_STATUS_PIN;
const char* door2_statusSwitchLogic = DOOR2_STATUS_SWITCH_LOGIC;

const char* door3_alias = DOOR3_ALIAS;
const char* mqtt_door3_action_topic = MQTT_DOOR3_ACTION_TOPIC;
const char* mqtt_door3_status_topic = MQTT_DOOR3_STATUS_TOPIC;
const int door3_statusPin = DOOR3_STATUS_PIN;
const char* door3_statusSwitchLogic = DOOR3_STATUS_SWITCH_LOGIC;

const char* door4_alias = DOOR4_ALIAS;
const char* mqtt_door4_action_topic = MQTT_DOOR4_ACTION_TOPIC;
const char* mqtt_door4_status_topic = MQTT_DOOR4_STATUS_TOPIC;
const int door4_statusPin = DOOR4_STATUS_PIN;
const char* door4_statusSwitchLogic = DOOR4_STATUS_SWITCH_LOGIC;

const int relayActiveTime = 500;
int door1_lastStatusValue = 2;
int door2_lastStatusValue = 2;
int door3_lastStatusValue = 2;
int door4_lastStatusValue = 2;
unsigned long door1_lastSwitchTime = 0;
unsigned long door2_lastSwitchTime = 0;
unsigned long door3_lastSwitchTime = 0;
unsigned long door4_lastSwitchTime = 0;
int debounceTime = 2000;

String availabilityBase = MQTT_CLIENTID;
String availabilitySuffix = "/availability";
String availabilityTopicStr = availabilityBase + availabilitySuffix;
const char* availabilityTopic = availabilityTopicStr.c_str();
const char* birthMessage = "online";
const char* lwtMessage = "offline";

WiFiClient espClient;
PubSubClient client(espClient);

// Wifi setup function

void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  if (static_ip) {
    WiFi.config(ip, gateway, subnet);
  }

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print(" WiFi connected - IP address: ");
  Serial.println(WiFi.localIP());
}

// Callback when MQTT message is received; calls triggerDoorAction(), passing topic and payload as parameters

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  
  Serial.println();

  String topicToProcess = topic;
  payload[length] = '\0';
  String payloadToProcess = (char*)payload;
  triggerDoorAction(topicToProcess, payloadToProcess);
}

// Functions that check door status and publish an update when called

void publish_door1_status() {
  if (digitalRead(door1_statusPin) == LOW) {
    if (door1_statusSwitchLogic == "NO") {
      Serial.print(door1_alias);
      Serial.print(" closed! Publishing to ");
      Serial.print(mqtt_door1_status_topic);
      Serial.println("...");
      client.publish(mqtt_door1_status_topic, "closed", true);
    }
    else if (door1_statusSwitchLogic == "NC") {
      Serial.print(door1_alias);
      Serial.print(" open! Publishing to ");
      Serial.print(mqtt_door1_status_topic);
      Serial.println("...");
      client.publish(mqtt_door1_status_topic, "open", true);      
    }
    else {
      Serial.println("Error! Specify only either NO or NC for DOOR1_STATUS_SWITCH_LOGIC in config.h! Not publishing...");
    }
  }
  else {
    if (door1_statusSwitchLogic == "NO") {
      Serial.print(door1_alias);
      Serial.print(" open! Publishing to ");
      Serial.print(mqtt_door1_status_topic);
      Serial.println("...");
      client.publish(mqtt_door1_status_topic, "open", true);
    }
    else if (door1_statusSwitchLogic == "NC") {
      Serial.print(door1_alias);
      Serial.print(" closed! Publishing to ");
      Serial.print(mqtt_door1_status_topic);
      Serial.println("...");
      client.publish(mqtt_door1_status_topic, "closed", true);      
    }
    else {
      Serial.println("Error! Specify only either NO or NC for DOOR1_STATUS_SWITCH_LOGIC in config.h! Not publishing...");
    }
  }
}

void publish_door2_status() {
  if (digitalRead(door2_statusPin) == LOW) {
    if (door2_statusSwitchLogic == "NO") {
      Serial.print(door2_alias);
      Serial.print(" closed! Publishing to ");
      Serial.print(mqtt_door2_status_topic);
      Serial.println("...");
      client.publish(mqtt_door2_status_topic, "closed", true);
    }
    else if (door2_statusSwitchLogic == "NC") {
      Serial.print(door2_alias);
      Serial.print(" open! Publishing to ");
      Serial.print(mqtt_door2_status_topic);
      Serial.println("...");
      client.publish(mqtt_door2_status_topic, "open", true);      
    }
    else {
      Serial.println("Error! Specify only either NO or NC for DOOR2_STATUS_SWITCH_LOGIC in config.h! Not publishing...");
    }
  }
  else {
    if (door2_statusSwitchLogic == "NO") {
      Serial.print(door2_alias);
      Serial.print(" open! Publishing to ");
      Serial.print(mqtt_door2_status_topic);
      Serial.println("...");
      client.publish(mqtt_door2_status_topic, "open", true);
    }
    else if (door2_statusSwitchLogic == "NC") {
      Serial.print(door2_alias);
      Serial.print(" closed! Publishing to ");
      Serial.print(mqtt_door2_status_topic);
      Serial.println("...");
      client.publish(mqtt_door2_status_topic, "closed", true);      
    }
    else {
      Serial.println("Error! Specify only either NO or NC for DOOR2_STATUS_SWITCH_LOGIC in config.h! Not publishing...");
    }
  }
}

void publish_door3_status() {
  if (digitalRead(door3_statusPin) == LOW) {
    if (door3_statusSwitchLogic == "NO") {
      Serial.print(door3_alias);
      Serial.print(" closed! Publishing to ");
      Serial.print(mqtt_door3_status_topic);
      Serial.println("...");
      client.publish(mqtt_door3_status_topic, "closed", true);
    }
    else if (door3_statusSwitchLogic == "NC") {
      Serial.print(door3_alias);
      Serial.print(" open! Publishing to ");
      Serial.print(mqtt_door3_status_topic);
      Serial.println("...");
      client.publish(mqtt_door3_status_topic, "open", true);      
    }
    else {
      Serial.println("Error! Specify only either NO or NC for door3_STATUS_SWITCH_LOGIC in config.h! Not publishing...");
    }
  }
  else {
    if (door3_statusSwitchLogic == "NO") {
      Serial.print(door3_alias);
      Serial.print(" open! Publishing to ");
      Serial.print(mqtt_door3_status_topic);
      Serial.println("...");
      client.publish(mqtt_door3_status_topic, "open", true);
    }
    else if (door3_statusSwitchLogic == "NC") {
      Serial.print(door3_alias);
      Serial.print(" closed! Publishing to ");
      Serial.print(mqtt_door3_status_topic);
      Serial.println("...");
      client.publish(mqtt_door3_status_topic, "closed", true);      
    }
    else {
      Serial.println("Error! Specify only either NO or NC for door3_STATUS_SWITCH_LOGIC in config.h! Not publishing...");
    }
  }
}

void publish_door4_status() {
  if (digitalRead(door4_statusPin) == LOW) {
    if (door4_statusSwitchLogic == "NO") {
      Serial.print(door4_alias);
      Serial.print(" closed! Publishing to ");
      Serial.print(mqtt_door4_status_topic);
      Serial.println("...");
      client.publish(mqtt_door4_status_topic, "closed", true);
    }
    else if (door4_statusSwitchLogic == "NC") {
      Serial.print(door4_alias);
      Serial.print(" open! Publishing to ");
      Serial.print(mqtt_door4_status_topic);
      Serial.println("...");
      client.publish(mqtt_door4_status_topic, "open", true);      
    }
    else {
      Serial.println("Error! Specify only either NO or NC for door4_STATUS_SWITCH_LOGIC in config.h! Not publishing...");
    }
  }
  else {
    if (door4_statusSwitchLogic == "NO") {
      Serial.print(door4_alias);
      Serial.print(" open! Publishing to ");
      Serial.print(mqtt_door4_status_topic);
      Serial.println("...");
      client.publish(mqtt_door4_status_topic, "open", true);
    }
    else if (door4_statusSwitchLogic == "NC") {
      Serial.print(door4_alias);
      Serial.print(" closed! Publishing to ");
      Serial.print(mqtt_door4_status_topic);
      Serial.println("...");
      client.publish(mqtt_door4_status_topic, "closed", true);      
    }
    else {
      Serial.println("Error! Specify only either NO or NC for door4_STATUS_SWITCH_LOGIC in config.h! Not publishing...");
    }
  }
}

// Functions that run in loop() to check each loop if door status (open/closed) has changed and call publish_doorX_status() to publish any change if so

void check_door1_status() {
  int currentStatusValue = digitalRead(door1_statusPin);
  if (currentStatusValue != door1_lastStatusValue) {
    unsigned int currentTime = millis();
    if (currentTime - door1_lastSwitchTime >= debounceTime) {
      publish_door1_status();
      door1_lastStatusValue = currentStatusValue;
      door1_lastSwitchTime = currentTime;
    }
  }
}

void check_door2_status() {
  int currentStatusValue = digitalRead(door2_statusPin);
  if (currentStatusValue != door2_lastStatusValue) {
    unsigned int currentTime = millis();
    if (currentTime - door2_lastSwitchTime >= debounceTime) {
      publish_door2_status();
      door2_lastStatusValue = currentStatusValue;
      door2_lastSwitchTime = currentTime;
    }
  }
}

void check_door3_status() {
  int currentStatusValue = digitalRead(door3_statusPin);
  if (currentStatusValue != door3_lastStatusValue) {
    unsigned int currentTime = millis();
    if (currentTime - door3_lastSwitchTime >= debounceTime) {
      publish_door3_status();
      door3_lastStatusValue = currentStatusValue;
      door3_lastSwitchTime = currentTime;
    }
  }
}

void check_door4_status() {
  int currentStatusValue = digitalRead(door4_statusPin);
  if (currentStatusValue != door4_lastStatusValue) {
    unsigned int currentTime = millis();
    if (currentTime - door4_lastSwitchTime >= debounceTime) {
      publish_door4_status();
      door4_lastStatusValue = currentStatusValue;
      door4_lastSwitchTime = currentTime;
    }
  }
}

// Function that publishes birthMessage

void publish_birth_message() {
  // Publish the birthMessage
  Serial.print("Publishing birth message \"");
  Serial.print(birthMessage);
  Serial.print("\" to ");
  Serial.print(availabilityTopic);
  Serial.println("...");
  client.publish(availabilityTopic, birthMessage, true);
}

// Function that toggles the relevant relay-connected output pin

void toggleRelay(int pin) {
  if (activeHighRelay) {
    digitalWrite(pin, HIGH);
    delay(relayActiveTime);
    digitalWrite(pin, LOW);
  }
  else {
    digitalWrite(pin, LOW);
    delay(relayActiveTime);
    digitalWrite(pin, HIGH);
  }
}

// Function called by callback() when a message is received 
// Passes the message topic as the "requestedDoor" parameter and the message payload as the "requestedAction" parameter

void triggerDoorAction(String requestedDoor, String requestedAction) {
  if (requestedDoor == mqtt_door1_action_topic && requestedAction == "OPEN") {
    Serial.print("Triggering ");
    Serial.print(door1_alias);
    Serial.println(" OPEN relay!");
    toggleRelay(door1_openPin);
  }
  else if (requestedDoor == mqtt_door1_action_topic && requestedAction == "CLOSE") {
    Serial.print("Triggering ");
    Serial.print(door1_alias);
    Serial.println(" CLOSE relay!");
    toggleRelay(door1_closePin);
  }
  else if (requestedDoor == mqtt_door1_action_topic && requestedAction == "STATE") {
    Serial.print("Publishing on-demand status update for ");
    Serial.print(door1_alias);
    Serial.println("!");
    publish_birth_message();
    publish_door1_status();
  }
  else if (requestedDoor == mqtt_door2_action_topic && requestedAction == "STATE") {
    Serial.print("Publishing on-demand status update for ");
    Serial.print(door2_alias);
    Serial.println("!");
    publish_birth_message();
    publish_door2_status();
  }  
  else if (requestedDoor == mqtt_door3_action_topic && requestedAction == "STATE") {
    Serial.print("Publishing on-demand status update for ");
    Serial.print(door3_alias);
    Serial.println("!");
    publish_birth_message();
    publish_door3_status();
  } 
  else if (requestedDoor == mqtt_door4_action_topic && requestedAction == "STATE") {
    Serial.print("Publishing on-demand status update for ");
    Serial.print(door4_alias);
    Serial.println("!");
    publish_birth_message();
    publish_door4_status();
  } 
  else { Serial.println("Unrecognized action payload... taking no action!");
  }
}

// Function that runs in loop() to connect/reconnect to the MQTT broker, and publish the current door statuses on connect

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqtt_clientId, mqtt_username, mqtt_password, availabilityTopic, 0, true, lwtMessage)) {
      Serial.println("Connected!");

      // Publish the birth message on connect/reconnect
      publish_birth_message();

      // Subscribe to the action topics to listen for action messages
      Serial.print("Subscribing to ");
      Serial.print(mqtt_door1_action_topic);
      Serial.println("...");
      client.subscribe(mqtt_door1_action_topic);
      

      // Publish the current door status on connect/reconnect to ensure status is synced with whatever happened while disconnected
      publish_door1_status();
      publish_door2_status();
	  publish_door3_status();
      publish_door4_status();
      

    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  // Setup the output and input pins used in the sketch
  // Set the lastStatusValue variables to the state of the status pins at setup time

  // Setup Door 1 pins
  pinMode(door1_openPin, OUTPUT);
  pinMode(door1_closePin, OUTPUT);
  // Set output pins LOW with an active-high relay
  if (activeHighRelay) {
    digitalWrite(door1_openPin, LOW);
    digitalWrite(door1_closePin, LOW);
  }
  // Set output pins HIGH with an active-low relay
  else {
    digitalWrite(door1_openPin, HIGH);
    digitalWrite(door1_closePin, HIGH);
  }
  // Set input pin to use internal pullup resistor
  pinMode(door1_statusPin, INPUT_PULLUP);
  // Update variable with current door state
  door1_lastStatusValue = digitalRead(door1_statusPin);

  // Setup Door 2 pins
  // Set input pin to use internal pullup resistor
  pinMode(door2_statusPin, INPUT_PULLUP);
  // Update variable with current door state
  door2_lastStatusValue = digitalRead(door2_statusPin);
  
  // Setup Door 3 pins
  // Set input pin to use internal pullup resistor
  pinMode(door3_statusPin, INPUT_PULLUP);
  // Update variable with current door state
  door3_lastStatusValue = digitalRead(door3_statusPin);
  
  // Setup Door 4 pins
  // Set input pin to use internal pullup resistor
  pinMode(door4_statusPin, INPUT_PULLUP);
  // Update variable with current door state
  door4_lastStatusValue = digitalRead(door4_statusPin);
  


  // Setup serial output, connect to wifi, connect to MQTT broker, set MQTT message callback
  Serial.begin(115200);

  Serial.println("Starting GarHAge...");

  if (activeHighRelay) {
    Serial.println("Relay mode: Active-High");
  }
  else {
    Serial.println("Relay mode: Active-Low");
  }

  setup_wifi();
  client.setServer(mqtt_broker, 1883);
  client.setCallback(callback);
}

void loop() {
  // Connect/reconnect to the MQTT broker and listen for messages
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  // Check door open/closed status each loop and publish changes
  check_door1_status();
  check_door2_status(); 
  check_door3_status(); 
  check_door4_status(); 
}
