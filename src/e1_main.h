#ifndef E1_MAIN_H
#define E1_MAIN_H

#include <e1_global.h>

#include <E131.h>
E131 e131;

E1CONFIG e1c;

#include "e1_password.h"

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
// #include <ESP8266HTTPUpdateServer.h>
// #include <ESP8266httpUpdate.h>

#include <WiFiClient.h>
#include <Ticker.h>
#include <EEPROM.h>

String MessageString;
// Im Gegensatz zu Domainnamen d�rfen Hostnames nur aus den ASCII-Zeichen a�z bzw. A�Z (zwischen Gro�- und Kleinbuchstaben wird nicht unterschieden),
// den Ziffern 0�9 und
// dem Bindestrich-Minus - bestehen

IPAddress ip9(192, 168, 179, 200);
IPAddress esp_ip(0, 0, 0, 0);
IPAddress dns(192, 168, 179, 1);
IPAddress gateway(192, 168, 179, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress ip0(0, 0, 0, 0);
IPAddress dns0(0, 0, 0, 0);
IPAddress gateway0(0, 0, 0, 0);
String esp_ip_str = "noipyet";

IPAddress ipMulti = IPAddress(192, 168, 179, 255);

// GPIO15 = GND (always)
// GPIO00 = GND when flashing
// D0 = GPIO16 will cause conflict - Belongs to the RTC module, not the general GPIO module,

// DMX Sender
uint8_t DMXsequence = 1;

int DMXGammaPerc = 100; // correct all channels with this factor
uint8_t DMXMin = 0;
uint8_t DMXMax = 255;

String DMX_Actor[] = {
	"Front",   // 0
	"Back",	// 1
	"Spot1",   // 2
	"Spot2",   // 3
	"Laser",   // 4
	"Kim",	 // 5
	"Lale",	// 6
	"Swantje", // 7
	"Liz",	 // 8
	"No5",	 // 9
	"No6",	 // 10
	"No7",	 // 11
	"No8"	  // 12
};
String DMX_Module[] = {
	"Wheel",
	"Post",
	"Body",
	"Legs"};

extern boolean udpActive;

String rssi_qual = "really unknown";
int rssi_bars = 0;
int32_t rssi_value = 0;
int rssi_percent = 0;

unsigned long lastTimeHost = 0, lastTimeMaster = 0;
int retval;

ESP8266WebServer server(80); // The Webserver
// ESP8266HTTPUpdateServer httpUpdater;

boolean firstStart = true;	// On firststart = true, NTP will try to get a valid time
int AdminTimeOutCounter = 0;  // Counter for Disabling the AdminMode
boolean AdminEnabled = false; // Enable Admin Mode for a given Time
unsigned int WiFiConnectCnt = 0;
byte Minute_Old = 100; // Helpvariable for checking, when a new Minute comes up (for Auto Turn On / Off)

#define update_server "192.168.179.4"
#define firmware_version FIRMWARE
// firmware_version room"_messwerte_0.X.Y"
#define update_php "/ucls/updateFW.php"
#define update_bin "/ucls/ESP12E_E131_DMX.bin"
#define update_file "http://192.168.179.4/ucls/update/ESP12E_E131_DMX.bin"

void WriteConfig(void);
void EraseConfig(void);
void ConfigureWifi(void);
void dmx_serial_init(void);

#endif
