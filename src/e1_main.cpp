/*
    e1_main.cpp
    
*/
#include "e1_main.h"

// ------------------------------------------------------------------------------------- HTTPUpdateConnect
// start update server protected with password defined in global.h
/*
void HTTPUpdateConnect()
{
  httpUpdater.setup(&server, UPDATE_USER_NAME, UPDATE_USER_PASSWORD);
  server.begin();
  DEBUGGING_L("> HTTPUpdateServer ready! Open http://");
  DEBUGGING_L(e1c.e1config.DeviceName);
  DEBUGGING(".local/update in your browser\n");
} // HTTPUpdateConnect()
*/

// ------------------------------------------------------------------------------------- server_init
// attach the WEB pages to the server
void esp_reset()
{
  String MessageString = "RESET...";
  e1c.esp_status = "RESET";
  e1c.esp_level = 0;
  NodeServer_text_Multicast(MessageString);
  delay(50);
  ESP.restart();
}

// ------------------------------------------------------------------------------------- server_reply
void server_reply(String message)
{

  String msg = "<!DOCTYPE HTML><html><head><title>";
  msg += e1c.e1config.DeviceName;
  msg += "</title></head><body>";
  msg += "<h1>";
  msg += e1c.e1config.DeviceName;
  msg += "</h1><h3>";
  msg += message;
  msg += "</h3></body></html>";

  NodeServer_text_Multicast(message);
  server.send(200, "text/html", msg);
}

// ------------------------------------------------------------------------------------- server_init
// attach the WEB pages to the server
void esp_info()
{
  char buf[12]; // "-2147483648\0"

  String strEspInfo = "Info ";
  strEspInfo += "Status     :" + e1c.esp_status + "<br><br>";
  strEspInfo += "Firmware   :" + e1c.fwVers + "<br>";

  strEspInfo += "universe   :";
  itoa(e1c.e1config.universe, buf, 10);
  strEspInfo += buf;
  strEspInfo += "<br>";

  strEspInfo += "deviceNum   :";
  itoa(e1c.e1config.deviceNum, buf, 10);
  strEspInfo += buf;
  strEspInfo += "<br>";

  strEspInfo += "moduleNum   :";
  itoa(e1c.e1config.moduleNum, buf, 10);
  strEspInfo += buf;
  strEspInfo += "<br>";

  strEspInfo += "LEDconfig   :";
  itoa(e1c.e1config.LEDconfig, buf, 10);
  strEspInfo += buf;
  strEspInfo += "<br>";

  server_reply(strEspInfo);
}

// ------------------------------------------------------------------------------------- server_init
void server_init()
{
  server.on("/", []() { esp_info(); });

  // server.on("/update-pull", firwmareUpdateFixed);

  server.on("/test", []() { 	server_reply("LED test"					); Led_Test		(); });
  server.on("/reset", []() { 	server_reply("RESET"                 	); esp_reset	(); });
  server.on("/off", []() { 	server_reply("switch all LEDs off"   	); Led_Off		(); });
  server.on("/on", []() { 	server_reply("set LED to 1"			   	); Led_Set		(1,0,0	); });
  server.on("/on100", []() { 	server_reply("set LED to 100"		   	); Led_Set		(100,0,0); });

  server.onNotFound([]() { Serial.println("Uuups .... Page Not Found"); server.send ( 400, "text/html", "Uuups .... Page not Found" ); });
  server.begin();
  Serial.println("> HTTP server started");

} // server_init()

// ------------------------------------------------------------------------------------- WifiStopAP
// stop access point
void WifiStopAP()
{

  DEBUGGING("WiFi AP> disconnect Access Point");
  WiFi.softAPdisconnect();
  WiFi.mode(WIFI_STA);
  delay(100);
  e1c.SoftAPup = false;
}

// ------------------------------------------------------------------------------------- module_init
// usually part of setup but required during operation if WiFI connection got lost
void module_init()
{

  int connect_count = 0;

  e1c.esp_level = (10);

  Led_Off();

  if (e1c.bStartup == true) // blink only during initial startup to avoid flashing during show
  {
    Led_Blink_Status(25, 150);
    delay(150);
    Led_Blink_Status(25, 150);
    WiFi.setAutoConnect(false); // do not connect on power on to the last used access point
  }
  else
    WiFi.setAutoConnect(true); // connect to the last used access point

  if (connect_count > 10) //
  {
    e1c.bStartup = true;
    connect_count = 0;
  }

  // connect to WiFi and set multicast address based on universe
  // unsigned long universe = e1c.universe;

  DEBUGGING_L("\nmodule_init> connect using ip ");
  DEBUGGING(esp_ip);

  //       e131.beginMulticast(ssid, passphrase, 2); /* via Multicast for Universe 2 */
  retval = e131.beginMulticast(e1c.e1config.ssid.c_str(), e1c.e1config.password.c_str(), e1c.e1config.universe);
  if (retval == WL_CONNECTED)
  {
    // WifiStopAP(); 		// AP is only needed if normal WiFi is missing
    // WiFi.printDiag(Serial);

    const char *modes[] = {"NULL", "STA", "AP", "STA+AP"};
    Serial.print("WiFi> Mode    : ");
    Serial.println(modes[WiFi.getMode()]);

    const char *phymodes[] = {"", "B", "G", "N"};
    Serial.print("WiFi> PHY mode: ");
    Serial.println(phymodes[(int)WiFi.getPhyMode()]);

    Serial.print("WiFi> Channel : ");
    Serial.println(WiFi.channel());

    Serial.print("WiFi> E1.31 universe : ");
    Serial.println(e1c.e1config.universe);

    e1c.esp_level = (50);
    NodeServer_init_Multicast(); // udp begin, connect to master panel
    NodeServer_text_Multicast("Wifi connected");
    e1c.esp_status = "ALIVE";

    server_init(); // attach html pages
    // HTTPUpdateConnect(); // OTA update server

    if (e1c.bStartup == true)
    {
      NodeServer_text_Multicast("Wifi connected STARTUP");
      Led_Blink_Status(25, 300);

      // get ip of ESP8266 module
      esp_ip = WiFi.localIP();
      gateway = WiFi.gatewayIP();
      dns = gateway;
      Serial.printf("init >> esp_ip    %s\n", esp_ip.toString().c_str());
      Serial.printf("init >> dns       %s\n", dns.toString().c_str());
      Serial.printf("init >> gateway   %s\n", gateway.toString().c_str());
      Serial.printf("init >> subnet    %s\n", subnet.toString().c_str());
      NodeServer_text_Multicast("Wifi dns");
      // WiFi.config(local_ip, gateway, subnet, dns1, dns2)
      // WiFi.config(esp_ip, gateway, subnet);
      NodeServer_text_Multicast("Wifi config");
      esp_ip_str = WiFi.localIP().toString();
      NodeServer_text_Multicast("Wifi esp_ip_str");
    }
    e1c.bStartup = false;
  }
  else // WLAN AP not found
  {
    if (e1c.bStartup == true) // injecting pause during startup mode to save power and avoid modules gets to hot
    {
      Led_Blink_Status(25, 150);

      // wifi_set_sleep_type(LIGHT_SLEEP_T); // switch modem off
      WiFi.mode(WIFI_OFF);
      DEBUGGING_L(" zzZZZ");
      WiFi.forceSleepBegin();
      delay(4000);
      // wifi_set_sleep_type(NONE_SLEEP_T); // switch back to normal state, no sleep
      WiFi.forceSleepWake();
      DEBUGGING_L(" AWAKE  ");
    }
  }
  connect_count++;
  if (connect_count > 5) // enable DHCP again
    WiFi.config(ip0, gateway0, subnet, dns0);
}

// ------------------------------------------------------------------------------------- WiFiConnect
// start Access Point
void WifiConnectAP()
{

  DEBUGGING("--------------------------------------------------- WifiConnectAP ");
  DEBUGGING_L(">> SSID: ");
  DEBUGGING(ACCESS_POINT_NAME);

  // AP mode
  // WiFi.mode(WIFI_AP); // Access point (AP) mode, where it creates its own network that others can join
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ACCESS_POINT_NAME, ACCESS_POINT_PASSWORD);
  IPAddress myIP = WiFi.softAPIP();

  DEBUGGING_L(">> WiFi - connected to Access Point with IP ");
  DEBUGGING(myIP);
  Led_Blink_pRED(100, 300);
  Led_Blink_pRED(100, 300);
  e1c.SoftAPup = true;
}

// ------------------------------------------------------------------------------------- setup
// starting the device
void setup()
{
  Led_Init(); // set output ports for LED
              // udp_send_init();

  e1c.esp_status = "BOOT";
  e1c.esp_level = (0);

  Serial.begin(115200); // start serial
  delay(100);

  DEBUGGING("\n\n");
  DEBUGGING("--------------------------------------------------- ESP8266 E131 client ");
  DEBUGGING_L("         Firmware ");
  DEBUGGING(FIRMWARE);

  flash_set_default_config();
  Serial.println("> General config applied");
  DEBUGGING_L("         universe ");
  DEBUGGING(e1c.e1config.universe);
  // config.OLEDoutputOn = true; // DEBUG ONLY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  e1c.e1config.Firmware = FIRMWARE;
  e1c.fwVers = e1c.e1config.Firmware.substring(0, 5);
  Led_Init();     // set output ports for LED
  Battery_Init(); // initialize battery monitor on A0

  if (AdminEnabled)
    WifiConnectAP();
  else
    WiFi.mode(WIFI_STA);

  WiFi.hostname(String(e1c.e1config.DeviceName));

  Led_TestStartup(); // blink RGB stripe

  // set to 0 to enable DHCP for startup
  esp_ip = ip0;
  gateway = gateway0;
  dns = gateway0;

  e1c.bStartup = true;
  module_init();

  e1c.bStartup = false;

  /*
	wifi_set_sleep_type(LIGHT_SLEEP_T); // switch modem off
	wifi_set_sleep_type(NONE_SLEEP_T); // switch back to normal state, no sleep
	*/
}

// ------------------------------------------------------------------------------------- // ------------------------------------------------------------------------------------- loop

void WifiSignal()
{

  /*
	High quality	90% ~= -55db
	Medium quality	50% ~= -75db
	Low quality	 30% ~= -85db
	Unusable quality	   8% ~= -96db
	*/

  rssi_value = WiFi.RSSI();
  rssi_percent = 2 * (rssi_value + 100);

  if (rssi_value > -55)
  {
    rssi_percent = 100;
    rssi_qual = "high";
    rssi_bars = 4;
  }
  else if (rssi_value >= -75)
  {
    rssi_qual = "medium";
    rssi_bars = 3;
  }
  else if (rssi_value >= -85)
  {
    rssi_qual = "low";
    rssi_bars = 2;
  }
  else
  {
    rssi_percent = 0;
    rssi_qual = "very poor";
    rssi_bars = 1;
  }

  /*
	Serial.print  ("RSSI: ");
	Serial.print  (rssi);
	Serial.print  (" ");
	Serial.print  (rssi_qual);
	Serial.print  (" ");
	Serial.println(rssi_bars);
    */
}

// ------------------------------------------------------------------------------------- loop
void loop()
{
  if (WiFi.status() != WL_CONNECTED) // check if WiFi is conencted
  {
    DEBUGGING("========== WIFI Reconnect ============= ");
    module_init(); // connect to Wifi + start http server
  }
  else
  {
    if (e1c.battery_critical == true)
    {
      e1c.esp_level = (0);
      e1c.esp_status = "BATTERY CRITICAL - FORCE SLEEP";
      NodeServer_text_Multicast(e1c.esp_status); // wifi_set_sleep_type(LIGHT_SLEEP_T); // switch modem off
      yield();
      WiFi.mode(WIFI_OFF);
      DEBUGGING_L(" zzZZZ");
      WiFi.forceSleepBegin();
      delay(4000);
      // wifi_set_sleep_type(NONE_SLEEP_T); // switch back to normal state, no sleep
      WiFi.forceSleepWake();
      DEBUGGING_L(" AWAKE  ");
    }
    else // battery not critical
    {
      // handle_udp();
      handle_e131();      // read udp ACN/E1.31 packages
      WiFiConnectCnt = 0; // reset Connect counter

      if (udpActive == false) // give udp priority and delay http requests until no udp packets are received
      {
        e1c.esp_level = 50;

        if (millis() - lastTimeHost > 10)
        {
          server.handleClient();
          lastTimeHost = millis();
          yield();
        }
        // send status every second if udp is not active
        if (millis() - lastTimeMaster > 1000)
        {
          battery_getval();
          WifiSignal();
          Led_Off();
          yield();

          if (e1c.uiVoltLevel < 50) // battery too low
          {
            MessageString = " zzzZZ BATTERY TOO LOW";
            e1c.esp_status = "BattLow";
            e1c.esp_level = (20);
            Led_Set(50, 0, 0);
            delay(50);
            Led_Off();
            delay(50);
            Led_Set(50, 0, 0);
            delay(50);
            Led_Off();
            delay(500);
          }
          else
          {
            MessageString = " no udp - zzzZZZZ";
            e1c.esp_status = "zzZZ";
          }

          NodeServer_text_Multicast(MessageString);
          lastTimeMaster = millis();
          yield();
        }

      } // if (udpActive == false)

    } // battery critical

  } // if (WiFi.status() != WL_CONNECTED)
}
