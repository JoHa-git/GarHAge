/*   
 * GarHAge
 * a Home-Automation-friendly ESP8266-based MQTT Garage Door Controller
 * Licensed under the MIT License, Copyright (c) 2017 marthoc
 * 
 * User-configurable Parameters 
*/

// Wifi Parameters

#define WIFI_SSID ""
#define WIFI_PASSWORD ""

// Static IP Parameters

#define STATIC_IP false
#define IP 192,168,1,100
#define GATEWAY 192,168,1,1
#define SUBNET 255,255,255,0

// MQTT Parameters

#define MQTT_BROKER "192.168.1.50"
#define MQTT_CLIENTID ""
#define MQTT_USERNAME ""
#define MQTT_PASSWORD ""

// Relay Parameters

#define ACTIVE_HIGH_RELAY true

// Door 1 Parameters

#define DOOR1_ALIAS "Iso ovi"
#define MQTT_DOOR1_ACTION_TOPIC "garage/door/1/action"
#define MQTT_DOOR1_STATUS_TOPIC "garage/door/1/status"
#define DOOR1_OPEN_PIN D1
#define DOOR1_CLOSE_PIN D1
#define DOOR1_STATUS_PIN D5
#define DOOR1_STATUS_SWITCH_LOGIC "NO"

// Door 2-4 Parameters

#define DOOR2_ALIAS "Sivuovi"
#define MQTT_DOOR2_ACTION_TOPIC "garage/door/2/action"
#define MQTT_DOOR2_STATUS_TOPIC "garage/door/2/status"
#define DOOR2_STATUS_PIN D2
#define DOOR2_STATUS_SWITCH_LOGIC "NO"

#define DOOR3_ALIAS "Kylmavarasto"
#define MQTT_DOOR3_ACTION_TOPIC "garage/door/3/action"
#define MQTT_DOOR3_STATUS_TOPIC "garage/door/3/status"
#define DOOR3_STATUS_PIN D3
#define DOOR3_STATUS_SWITCH_LOGIC "NO"

#define DOOR4_ALIAS "Puutarhavarasto"
#define MQTT_DOOR4_ACTION_TOPIC "garage/door/4/action"
#define MQTT_DOOR4_STATUS_TOPIC "garage/door/4/status"
#define DOOR4_STATUS_PIN D4
#define DOOR4_STATUS_SWITCH_LOGIC "NO"