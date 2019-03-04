#ifndef E1_GLOBAL_H
#define E1_GLOBAL_H

#include <Arduino.h>
#include <ESP8266WiFi.h>

#ifdef PASSWORD_THOMAS
#include "e1_password_thomas.h"
#else
#include "e1_password.h"
#endif

#define FIRMWARE "v3.26 2018-02-02"
#define DEVICE_NAME "ucl-01-spot" // default name for factory reset
#define NODE_PORT 45131
#define E131_UNIVERSE 2
#define E131_NODE_DEVICE_NUM 0
#define E131_NODE_MODULE_NUM 0
#define E131_DMX_GAMMA 100
#define E131_DMX_OUTPUT false
#define E131_LED_CONFIG 0

/*
Item		default		Description
Universe	2	 		this will define multicast address
Node name	"ucl-01-xx"	Name of module
DeviceNum 	0..9		Defining offset 1
						0 Front , 1 Back, 2 Spot1 , 3 Spot2, 4 Laser, 5 Actor1,6 Actor 2…
Module Num	0			Defining offset 2
						0 Wheel, 1 Post, 2 Body
dmxGammaPerc	1		Gamma correction (brightness)
dmxOutput	0			On off DMX serial output
LEDconfig	0			Asssign LED RGBR to GPIO Pin1 - NodeMCU 14-13-12-152 - Mini 1  13-12-14-153 - Mini 2  12-13-14-15
						4 - Mini 3  13-14-12-15
*/

#include "e1_config.h"

#define Button1 0   // D3  = GPIO  0
#define Button2 D10 // D10 = GPIO  xx
#define Button3 D9  // D9  = GPIO  xx

/* Debugging */
#define DEBUGGING(...) Serial.println(__VA_ARGS__)
#define DEBUGGING_L(...) Serial.print(__VA_ARGS__)

void DMX_Init(void);
void Led_Init(void);
void Led_Test(void);
void Led_Off(void);
void Led_Blink(void);
void Led_Blink(int iLED, int brightness, int duration);
void Led_Set(int iRed, int iGreen, int iBlue);
void Led_Blink_Status(int brightness, int duration);
void Led_Blink_pRED(int brightness, int duration);
void Battery_Init(void);
void NodeServer_ping_Multicast(void);
void NodeServer_text_Multicast(String sMessage);
void NodeServer_init_Multicast(void);
void esp_reset(void);
bool ReadConfig(void);
void flash_set_default_config();
void Led_TestStartup();
void handle_e131();
void battery_getval(void);

uint8_t getConfigLED(void);
void WifiStopAP();
void WifiSignal(void);

#endif
