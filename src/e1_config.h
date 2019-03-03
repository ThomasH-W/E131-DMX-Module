#ifndef E1_CONFIG_H
#define E1_CONFIG_H

#include <e1_global.h>

typedef struct strConfig
{
  String ssid;
  String password;
  byte IP[4];
  byte Netmask[4];
  byte Gateway[4];
  boolean dhcp;
  byte LED_R;
  byte LED_G;
  byte LED_B;
  unsigned long deviceNum;
  unsigned long moduleNum;
  unsigned long universe;
  unsigned long DxmPowerOffTime;
  unsigned long DMXstartChannel;
  boolean DMXoutputOn;
  byte LEDconfig;
  unsigned long batAnalogRef;
  float batDividerRatio;
  boolean OLEDoutputOn;
  String Firmware;
  String DeviceName;
} config;

class E1CONFIG
{
private:
public:
  config e1config; /* config */
  String esp_status;
  uint8_t esp_level;
  boolean bStartup; // On firststart = true, set ports
  boolean battery_critical;
  uint8_t uiVoltLevel;
  String esp_ip_str;
  boolean SoftAPup;
  String fwVers;
  IPAddress nodeIpMulti;
  unsigned int nodePortMulti;

  E1CONFIG();
};

#endif
