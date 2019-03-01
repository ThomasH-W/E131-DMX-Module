/*
 * password.h
 *
 *  Created on: 04.03.2017
 *      Author: Thomas
 */

#ifndef E1_DMX_H_
#define E1_DMX_H_

#include <e1_global.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <E131.h>

String MsgString;

extern E1CONFIG e1c;
extern E131 e131;

extern int32_t rssi_value;
extern int32_t rssi_percent;

WiFiUDP UdpMaster;
WiFiUDP Udp;

// set various timers
unsigned long tempTimer, flashTimer, powerOffTimer, aliveTimer, noWiFiTimer;
unsigned long lastTimeUdp = 0;

int iLED = 0;
int iBrightness = 0;
int incr = 1;

/* DXM */
// uint16_t universe = 2, universeTX = 2;
#define DMX_SET 12
uint8_t dmxCh = 0;
uint8_t dmxStartChannel = 0;
uint16_t dmxGammaPerc = 100; // correct all channels with this factor
uint8_t dmxMin = 0;
uint8_t dmxMax = 255;
unsigned long uiDxmPowerOffTime = 2000;

boolean dmxTxLoop = false;
uint8_t dmxTxRed = 50;
uint8_t dmxTxGreen = 0;
uint8_t dmxTxBlue = 0;
uint8_t dmxTxRed2 = 0;

uint16_t uiDMXRed = 0;   // raw DMX data
uint16_t uiDMXGreen = 0; // raw DMX data
uint16_t uiDMXBlue = 0;  // raw DMX data
uint16_t uiDMXRed2 = 0;  // raw DMX data

uint8_t iLedRed = 0;   // temp data
uint8_t iLedGreen = 0; // temp data
uint8_t iLedBlue = 0;  // temp data
uint8_t iLedRed2 = 0;  // temp data

int DMX_RED = 0;
int DMX_GREEN = 1;
int DMX_BLUE = 2;
int DMX_RED2 = 3;

int iRetVal = false;
boolean udpActive = false;
boolean e131Active = false;
boolean testmode = false;
boolean testDmxRGB = false;
boolean testButton = false;
boolean node_json = true;

// static const uint8_t LED_BUILTIN = 16;
uint8_t pLED_RED = 14;   // D5 = GPIO 14
uint8_t pLED_GREEN = 13; // D7 = GPIO 13
uint8_t pLED_BLUE = 12;  // D6 = GPIO 12
uint8_t pLED_RED2 = 15;  // D8 = GPIO 15 // not for ESP12E !!!

uint8_t LEDs[] = {pLED_RED, pLED_GREEN, pLED_BLUE};
uint8_t LEDcnt = sizeof(LEDs);

// 			Set 	Element
#define LED_PIN_SETS 4
uint8_t LED_PIN[LED_PIN_SETS][4] =
    {
        // R  	G  	B 	R2
        {14, 13, 12, 15}, // NodeMCU with MOSFET
        {13, 12, 14, 15}, // ESP12E mini board
        {12, 13, 14, 15},
        {13, 14, 12, 15} // ESP12E mini board
};

// library file ESPDMX.cpp
// int sendPin = 2;		// GPIO2 = D4 = TxD1 - default on ESP8266
#define LED_STATUS 2 // D4 = GPIO  2
// #define LED_STATUS 	9 	// SDs = GPIO  9
#define DMX_STATUS 10 // D12 = GPIO  10

// ---------------------------------------------------------- battery monitor
// #include <Battery.h>
#define ANALOG_PIN A0        // read, input
#define DIVIDER_RATIO 4.3    // Bat+ --- 3k3 --- A0 --- 1k --- GND
#define ANALOG_REF_mV 3333   // NodeLUA - A0 up tp 3.3V
#define DIVIDER_RATIO_E 11.0 // Bat+ --- 10k --- A0 --- 1k --- GND
#define ANALOG_REF_mV_E 1070 // ESP12E - A0 up tp 1.0V

#define LIPO_mV_FULL 8340  //                           Lipo: 1S full 4,2; empty 3,3V
#define LIPO_mV_EMPTY 7600 // Need 7.6V for blue LED  - Lipo: 2S full 8,34; empty 6,6V
#define SENSE_PIN 0xFF     // activiation Pin / output; set to 0xFF to ignore
float fVoltLipo = 0;
uint16_t uiVoltAnalog = 0;
uint8_t uiVoltLevel = 0;
boolean battery_critical = false; // used to disable WiFi
unsigned long batteryTimer, aliveE131Timer;
unsigned long udpPacketCount = 0, udpPacketErrorCount = 0;

#include <ESPDMX.h>
DMXESPSerial dmx; // output on GPIO2 = D4 = TxD1

#endif /* E1_DMX_H_ */
