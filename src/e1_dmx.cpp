/*
    e1_dmx.cpp

*/
#include "e1_dmx.h"

// ------------------------------------------------------------------------------------- LedInit
// set ports for PWM output
// ports can be changed on WEB page Page_DMX.h
void Led_Init()
{
    pLED_RED = LED_PIN[e1c.e1config.LEDconfig][0];
    pLED_GREEN = LED_PIN[e1c.e1config.LEDconfig][1];
    pLED_BLUE = LED_PIN[e1c.e1config.LEDconfig][2];
    pLED_RED2 = LED_PIN[e1c.e1config.LEDconfig][3];

    if (e1c.bStartup == true)
    {
        pinMode(LED_STATUS, OUTPUT);
        pinMode(pLED_BLUE, OUTPUT);
        pinMode(pLED_GREEN, OUTPUT);
        pinMode(pLED_RED, OUTPUT);
    }

    DEBUGGING_L("> PIN mode set - config: ");
    DEBUGGING(e1c.e1config.LEDconfig);

} // LedInit()

// ------------------------------------------------------------------------------------- Led_Test
// test all LEDs by fading in and out
void Led_TestLoop()
{

    if (iLED == LEDcnt) // last LED in array exceeded
        iLED = 0;

    digitalWrite(LEDs[iLED], iBrightness);
    // iLED ++;
    iBrightness += incr;

    if (iBrightness > 200 && incr > 0) // decrease brightness when max brightness is reached
    {
        incr = -5;
        iBrightness += incr;
    }
    if (iBrightness < 0 && incr < 0) // decrease brightness when max brightness is reached
    {
        incr = 5;
        iBrightness += incr;
        iLED++;
    }

    // DEBUGGING(iBrightness);
    delay(40);

} // Led_Test()

void Led_Blink_Status(int brightness, int duration)
{
    Led_Blink(LED_STATUS, brightness, duration);
} // Led_Blink()

void Led_Blink_pRED(int brightness, int duration)
{
    Led_Blink(pLED_RED, brightness, duration);
} // Led_Blink()

// ------------------------------------------------------------------------------------- Led_Blink
// swtich LED on and off
// used when powering up the module to visualize it is operating
void Led_Blink(int iLED, int brightness, int duration)
{

    digitalWrite(iLED, brightness);
    delay(duration);
    digitalWrite(iLED, 0);

} // Led_Blink()

// ------------------------------------------------------------------------------------- Led_Off
// switch all LEDs off
void Led_Off()
{
    analogWrite(pLED_RED, 0);
    analogWrite(pLED_GREEN, 0);
    analogWrite(pLED_BLUE, 0);
    analogWrite(pLED_RED2, 0);

} // LedInit()

// ------------------------------------------------------------------------------------- Led_Set
// set LED to a specific value
void Led_Set(int iRed, int iGreen, int iBlue)
{
    analogWrite(pLED_RED, iRed);
    analogWrite(pLED_GREEN, iGreen);
    analogWrite(pLED_BLUE, iBlue);

    iLedRed = iRed;
    iLedGreen = iGreen;
    iLedBlue = iBlue;

} // Led_Set()

// ------------------------------------------------------------------------------------- LedInit
// switch the LED on/off, one after the other RED,GREEN,BLUE
void Led_Test()
{

    int iDelay = 400;
    DEBUGGING("> blink ...");
    e1c.esp_status = "LED test";
    e1c.esp_level = 75;
    NodeServer_text_Multicast("LED test");

    digitalWrite(LED_STATUS, LOW);
    Led_Off();
    delay(iDelay);

    e1c.esp_status = "LED TEST RED";
    digitalWrite(LED_STATUS, HIGH);
    Led_Set(255, 0, 0);
    delay(iDelay);

    e1c.esp_status = "LED TEST GREEN";
    digitalWrite(LED_STATUS, LOW);
    Led_Set(0, 255, 0);
    delay(iDelay);

    e1c.esp_status = "LED TEST BLUE";
    digitalWrite(LED_STATUS, HIGH);
    Led_Set(0, 0, 255);
    delay(iDelay);

    e1c.esp_status = "LED TEST OFF";
    digitalWrite(LED_STATUS, LOW);
    Led_Set(0, 0, 0);
    delay(iDelay);
}

// ------------------------------------------------------------------------------------- Led_TestStartup
// switch the LED on/off, one after the other RED,GREEN,BLUE
void Led_TestStartup()
{
    int iDelay = 400;
    DEBUGGING("> blink ...");

    digitalWrite(LED_STATUS, LOW);
    Led_Off();

    digitalWrite(LED_STATUS, HIGH);
    Led_Set(1, 0, 0);
    delay(iDelay);

    digitalWrite(LED_STATUS, LOW);
    Led_Set(0, 0, 0);
}

// ------------------------------------------------------------------------------------- Battery_Init
// initialize the paramaters for battery measurement
// a little paranoid but I want to protect a division by 0
void Battery_Init()
{

    pinMode(ANALOG_PIN, INPUT);
    if (e1c.e1config.LEDconfig == 0) // NodeMCU LUA module
    {
        e1c.e1config.batAnalogRef = ANALOG_REF_mV;
        e1c.e1config.batDividerRatio = DIVIDER_RATIO;
    }
    else // ESP12E module w/o board
    {
        e1c.e1config.batAnalogRef = ANALOG_REF_mV_E;
        e1c.e1config.batDividerRatio = DIVIDER_RATIO_E;
    }
    DEBUGGING("> battery - init ");

} // Battery_Init()

// ------------------------------------------------------------------------------------- battery_getval
// LiPo batteries are sensitive if voltage is dropping a certain value
// in addition the LED stripe will need a certain voltage - espacially when showing blue
// read voltage from analog in and convert to Volt and %
void battery_getval()
{

    if (millis() - batteryTimer > 100)
    {
        uiVoltAnalog = analogRead(ANALOG_PIN);
        fVoltLipo = uiVoltAnalog * e1c.e1config.batDividerRatio * e1c.e1config.batAnalogRef / 1000 / 1024;
        // Lipo: 1S full 4,2; empty 3,3V
        // Lipo: 2S full 8,34; empty 6,6V
        /*
		iVoltLevel		= (int) (( (fVoltLipo * 1000) - LIPO_mV_EMPTY) / (LIPO_mV_FULL - LIPO_mV_EMPTY) *100 );
		if( iVoltLevel < 0) // check if level is below limit
			uiVoltLevel = 0;
		else
			uiVoltLevel = iVoltLevel;
		*/

        // LED Stripe level:  RED		0-7,5; 		// YELLOW	7,5 - 7,9V 		// GREEN	7,9V
        battery_critical = false; // used to disable wiFi
        if (fVoltLipo < 3.0)      // no measurement attached to A0
            uiVoltLevel = 99;
        else if (fVoltLipo < 7.0)
        {
            uiVoltLevel = 0;
            battery_critical = true;
        }
        else if (fVoltLipo < 7.5)
            uiVoltLevel = 0;
        else if (fVoltLipo < 7.9)
            uiVoltLevel = 50;
        else
            uiVoltLevel = 100;

        /*
		uiVoltLevel =0;		// RED - BLUE LED too low
		if( fVoltLipo > 7.5) // YELLOW - blue LED still ok
			uiVoltLevel = 50;
		if( fVoltLipo > 7.9) // GREEN - all LED bright
			uiVoltLevel = 100;
		*/

        // Spot 1+2 are using a power from DMX device, no battery connected
        if (e1c.e1config.deviceNum > 1 && e1c.e1config.deviceNum < 4)
            uiVoltLevel = 100;

        batteryTimer = millis();
    }
}

// ------------------------------------------------------------------------------------- reconnect_e131
void reconnect_e131()
{
    // DEBUGGING_L("\ne131> reconnect ");
    // e131.stopUdp();
    // e131.connectMulticast(e1c.universe);
}

// ------------------------------------------------------------------------------------- dmx_serial_init
// initialize DMX output if required
void dmx_serial_init()
{
    if (e1c.e1config.DMXoutputOn == true)
    {
        dmx.init(32); // use 32 channels i/o 512
        for (int thisChannel = DMX_RED; thisChannel <= (DMX_RED + 4); thisChannel++)
            dmx.write(thisChannel + 1, 0);
        dmx.update();
    }
} // dmx_serial_init()

// ------------------------------------------------------------------------------------- handle_udp
// read udp package and write DMX value to LED output (analog,P9813.DMX)
void handle_e131()
{
    /* Parse a packet and update pixels */

    uint16_t num_channels = e131.parsePacket();
    if (num_channels)
    {
        /*
        Serial.printf("Universe %u / %u Channels | Packet#: %u / Errors: %u / CH1: %u\n",
                      e131.universe,            // The Universe for this packet
                      num_channels,             // Number of channels in this packet
                      e131.stats.num_packets,   // Packet counter
                      e131.stats.packet_errors, // Packet error counter
                      e131.data[0]);            // Dimmer data for Channel 1
*/
        udpActive = true;
        testmode = false;
        if (e1c.SoftAPup == true)
            WifiStopAP();
        e1c.esp_level = 70;

        udpPacketCount++;
        powerOffTimer = tempTimer = lastTimeUdp = millis();
        if (e1c.e1config.DMXoutputOn == false)
            digitalWrite(LED_STATUS, HIGH); // set blue status LED

        // if (e131.universe == universe && e131.ddStartCode == 0)
        if (e131.universe == e1c.e1config.universe)
        { // use only e131 packages from the right universe
            e1c.esp_status = "E1.31";
            e1c.esp_level = 100;

            // read DMX values into temp variable
            uiDMXRed = e131.data[DMX_RED];
            uiDMXGreen = e131.data[DMX_GREEN];
            uiDMXBlue = e131.data[DMX_BLUE];
            uiDMXRed2 = e131.data[DMX_RED2];

            // when device is started, the blue status LED will flash
            // if 1st DMX channel is used, indicate e131 submission has started
            // than the flashing of the blue status LED will stop
            if ((e131.data[0]) != 0)
                e131Active = true;

            // apply gamma factor on DMX values
            // every modules can reduce the brightness -> Page_DMX.h
            if (dmxGammaPerc < 100)
            {
                uiDMXRed = uiDMXRed * dmxGammaPerc / 100; // 255 * 100 / 100 = 255; 25 * 200 / 100 = 50
                uiDMXGreen = uiDMXGreen * dmxGammaPerc / 100;
                uiDMXBlue = uiDMXBlue * dmxGammaPerc / 100;
                uiDMXRed2 = uiDMXRed2 * dmxGammaPerc / 100;
            }

            if (uiVoltLevel < 50) // battery too low
            {
                Led_Off();
            }
            else
            {
                // set output for analog and P9813
                analogWrite(pLED_RED, uiDMXRed);
                analogWrite(pLED_GREEN, uiDMXGreen);
                analogWrite(pLED_BLUE, uiDMXBlue);
                analogWrite(pLED_RED2, uiDMXRed2);

                // send data to DMX output if required
                if (e1c.e1config.DMXoutputOn == true)
                {
                    // int i=0;
                    for (int thisChannel = DMX_RED, i = 0; thisChannel <= (DMX_RED + 4); thisChannel++, i++)
                    {
                        dmx.write(thisChannel + 1, e131.data[thisChannel]);
                        dmx.write(i + 1, e131.data[thisChannel]);
                    }
                    dmx.update();
                    delay(10);
                } // DMX output
            }     // battery too low

            yield(); // grant time to background process like wifi handler

        } // if (e131.universe == UNIVERSE)

        // every 2 seconds the modules will send a status to a node server
        if (tempTimer - aliveTimer > 2000)
        {
            battery_getval();
            WifiSignal();
            yield();
            NodeServer_ping_Multicast();
            aliveTimer = tempTimer;
            yield();
        }

        // blink blue LED to indicate idle mode with active RF connection
        if (tempTimer - aliveE131Timer > 2000)
        {
            if (e131Active == false) // do not blink if submission has started
            {
                digitalWrite(LED_STATUS, LOW);
                delay(50);
                aliveE131Timer = tempTimer;
            }
        }

    } // if(e131.parsePacket()) {
    // when no udp packages are received for a given time, switch off all LEDs
    // during a show people might not notice a device which is not switched on
    // but it would be very obvious if the LED are on and all other are off
    else
    {
        if (tempTimer - powerOffTimer > uiDxmPowerOffTime && uiDxmPowerOffTime > 0)
        {
            // DEBUGGING("udp> timeout exceeded -> auto power off");
            if (udpActive == true) // status True = 1
            {
                udpActive = false;
                e131Active = false;
                Led_Off();
                battery_getval();
                WifiSignal();
                yield(); // grant time to background process like wifi handler

                MsgString = " no udp packet since 2 sec";
                e1c.esp_status = "timeout";
                NodeServer_text_Multicast(MsgString);
                tempTimer = powerOffTimer = flashTimer = millis(); // reset timer
                yield();                                           // grant time to background process like wifi handler
            }                                                      // if (udpStatus == 1)
        }                                                          // if (tempTimer - powerOffTimer > 2000)

        if (millis() - lastTimeUdp > 2000)
        {
            udpActive = false;
            e131Active = false;
            Led_Off();
            battery_getval();
            WifiSignal();
            MsgString = " idle - reconnect E1.31";
            e1c.esp_status = "CONNECT";
            // esp_level		= 30;
            NodeServer_text_Multicast(MsgString);
            reconnect_e131();
            delay(100);
            lastTimeUdp = millis();
            yield();
        }
    } // no packet received
}

// ------------------------------------------------------------------------------------- DMXInit
// set paramaters to read correct channels later on
void DMX_LED_off()
{

    /*
	if (config.DMXoutputOn == true)
	{
		for (int thisChannel = DMX_RED; thisChannel <= (DMX_RED+4); thisChannel++)
			dmx.write(thisChannel+1, 0);
		dmx.update();
	}
	*/

    if (e1c.e1config.DMXoutputOn == true)

    {
        dmx.write(1, 0);
        dmx.write(2, 0);
        dmx.write(3, 0);
        dmx.update();
    }
}

// ------------------------------------------------------------------------------------- DMXInit
// set paramaters to read correct channels later on
void DMX_LED_set(int iRed, int iGreen, int iBlue)
{

    if (e1c.e1config.DMXoutputOn == true)

    {
        dmx.write(1, iRed);
        dmx.write(2, iGreen);
        dmx.write(3, iBlue);
        dmx.update();
    }
}
// ------------------------------------------------------------------------------------- DMXInit
// set paramaters to read correct channels later on
void DMX_Init()
{

    dmxCh = e1c.e1config.deviceNum * DMX_SET + e1c.e1config.moduleNum * 3;
    dmxStartChannel = 1 + dmxCh;
    e1c.e1config.DMXstartChannel = dmxStartChannel;
    DMX_RED = 0 + dmxCh;
    DMX_GREEN = 1 + dmxCh;
    DMX_BLUE = 2 + dmxCh;
    DMX_RED2 = 3 + dmxCh;

    uiDxmPowerOffTime = e1c.e1config.DxmPowerOffTime * 1000;

    if (e1c.e1config.DMXoutputOn == true)
    {
        DEBUGGING_L("DMX> device        ");
        DEBUGGING(e1c.e1config.deviceNum);
        DEBUGGING_L("DMX> module        ");
        DEBUGGING(e1c.e1config.moduleNum);

        DEBUGGING_L("DMX> channel       ");
        DEBUGGING(dmxCh);
        DEBUGGING_L("DMX> Start channel ");
        DEBUGGING(dmxStartChannel);
    }

} // DMXInit()

// ------------------------------------------------------------------------------------- NodeServer_init_Multicast
// establish udp in order to send messages to a node server
void NodeServer_init_Multicast()
{
    // IPAddress ipMulti = IPAddress(192,168,178,255);
    e1c.nodeIpMulti = WiFi.localIP(); // get my IP address
    e1c.nodeIpMulti[3] = 255;         // set IP to broadcast address

    Serial.printf("udp>> Begin Multicast local IP %s -> IP %s, UDP port %d \n", WiFi.localIP().toString().c_str(), e1c.nodeIpMulti.toString().c_str(), e1c.nodePortMulti);

    // beginMulticast` is analogous to `begin` — it start listening on a UDP port, plus it also joins the multicast group.
    // So you don't need this one to send packets.
    //                 udp.beginMulticast(WiFi.localIP(), multicast_ip_addr, port)
    int retval = UdpMaster.beginMulticast(WiFi.localIP(), e1c.nodeIpMulti, e1c.nodePortMulti);
    if (retval == 0)
    {
        DEBUGGING("udp>> ERROR beginMulticast - no sockets available to use");
    }
    else
        DEBUGGING("udp ok: beginMulticast");
}

String IpAddress2String(const IPAddress &ipAddress)
{
    return String(ipAddress[0]) + String(".") +
           String(ipAddress[1]) + String(".") +
           String(ipAddress[2]) + String(".") +
           String(ipAddress[3]);
}

void NodeServer_text_Multicast_tmp(String sMessage)
{
    DEBUGGING(sMessage);
    // Udp.beginPacket(ipMulti, localUdpPort);
    Udp.beginPacketMulticast(e1c.nodeIpMulti, e1c.nodePortMulti, WiFi.localIP());
    Udp.print(sMessage);
    // Udp.print("UDP DMX TEST");
    // Udp.write("UDP DMX TEST");
    Udp.endPacket();

    delay(2000);
}

// ------------------------------------------------------------------------------------- NodeServer_ping_Multicast
// module will send x seconds teh RGB values to the node server
void NodeServer_text_Multicast(String sMessage)
{

    /*
	 * {"name":"Ucl-No7-Wheel","systemInfo":{"FWver":"v3.12","state":"BOOT","bat":{"volt":0.00,"perc":0},"rssi":{"db":0,"perc":0
	 * }},"dmx":{"set":11,"mod":0,"uni":2,"out":"off","color":{"r":0,"g":0,"b":0}},"msg":"ALIVE"}
	 */
    // https://jsonformatter.curiousconcept.com/

    String udpString;
    int iLedRed = 0;
    int iLedGreen = 0;
    int iLedBlue = 0;

    if (udpActive == true)
    {
        iLedRed = e131.data[DMX_RED];
        iLedGreen = e131.data[DMX_GREEN];
        iLedBlue = e131.data[DMX_BLUE];
    }

    udpString = "{\"name\":\"";
    udpString += e1c.e1config.DeviceName;
    udpString += "\",\"systemInfo\":{\"FWver\":\"";
    udpString += e1c.fwVers;
    udpString += "\",\"state\":\"";
    udpString += e1c.esp_status;
    udpString += "\",\"perc\":\"";
    udpString += e1c.esp_level;
    udpString += "\",\"ip\":\"";
    udpString += e1c.esp_ip_str;
    udpString += "\",\"bat\":{\"volt\":";
    udpString += fVoltLipo;
    udpString += ",\"perc\":";
    udpString += uiVoltLevel;
    udpString += "},\"rssi\":{\"db\":";
    udpString += rssi_value;
    udpString += ",\"perc\":";
    udpString += rssi_percent;
    udpString += "}},\"dmx\":{\"set\":";
    udpString += e1c.e1config.deviceNum;
    udpString += ",\"mod\":";
    udpString += e1c.e1config.moduleNum;
    udpString += ",\"uni\":";
    udpString += e1c.e1config.universe;
    udpString += ",\"serial\":\"";
    if (e1c.e1config.DMXoutputOn == true)
        udpString += "on";
    else
        udpString += "off";
    udpString += "\",\"data\":\"";
    if (udpActive == true)
        udpString += "on";
    else
        udpString += "off";

    udpString += "\",\"color\":{\"r\":";

    udpString += iLedRed;
    udpString += ",\"g\":";
    udpString += iLedGreen;
    udpString += ",\"b\":";
    udpString += iLedBlue;
    udpString += "}},\"msg\":\"";
    udpString += sMessage;

    udpString += "\"}";

    /*
    DEBUGGING_L("udp>> -------------------------------------------------- ");
    DEBUGGING(sMessage);
    // DEBUGGING(udpString);
    // udpString = sMessage; // debug only

    DEBUGGING_L("udp>> Begin Packet IP ");
    DEBUGGING_L(IpAddress2String(e1c.nodeIpMulti));
    DEBUGGING_L(" UDP port ");
    DEBUGGING(e1c.nodePortMulti);
*/
    //                 udp.beginPacketMulticast(addr, port, WiFi.localIP()
    int retval = UdpMaster.beginPacketMulticast(e1c.nodeIpMulti, e1c.nodePortMulti, WiFi.localIP());
    if (retval == 0)
    {
        DEBUGGING_L("udp ERROR: beginPacketMulticast - problem with the supplied IP address or port\n");
        DEBUGGING_L(" ");
        DEBUGGING_L(e1c.nodeIpMulti);
        DEBUGGING_L(" ");
        DEBUGGING_L(e1c.nodePortMulti);
        DEBUGGING_L(" ");
        DEBUGGING(WiFi.localIP());
    }
    else
        DEBUGGING_L("udp>> beginPacketMulticast - ");

    retval = UdpMaster.println(udpString);
    if (retval == 0)
        DEBUGGING(" println - ERROR");
    else
        DEBUGGING_L(" println - ");

    retval = UdpMaster.endPacket();
    if (retval == 0)
        DEBUGGING(" endPacket - ERROR");
    else
    {
        DEBUGGING_L(" endPacket - SEND: ");
        DEBUGGING(sMessage);
    }
    // UdpMaster.stop();

} // end of NodeServer_text_Multicast_json

// ------------------------------------------------------------------------------------- NodeServer_ping_Multicast
// module will send x seconds teh RGB values to the node server
void NodeServer_ping_Multicast()
{
    iLedRed = e131.data[DMX_RED];
    iLedGreen = e131.data[DMX_GREEN];
    iLedBlue = e131.data[DMX_BLUE];
    iLedRed2 = e131.data[DMX_RED2];

    MsgString = "E131 | R : ";
    MsgString += (String)iLedRed;
    MsgString += " | G : ";
    MsgString += (String)iLedGreen;
    MsgString += " | B : ";
    MsgString += (String)iLedBlue;

    NodeServer_text_Multicast(MsgString);
}

// ------------------------------------------------------------------------------------- flash_get_config
// write default parameters to flash
void flash_set_default_config()
{

    // DEFAULT CONFIG
    e1c.e1config.ssid = WIFI_SSID;
    e1c.e1config.password = WIFI_PASS;
    e1c.e1config.dhcp = true;
    e1c.e1config.IP[0] = 192;
    e1c.e1config.IP[1] = 168;
    e1c.e1config.IP[2] = 179;
    e1c.e1config.IP[3] = 100;
    e1c.e1config.Netmask[0] = 255;
    e1c.e1config.Netmask[1] = 255;
    e1c.e1config.Netmask[2] = 255;
    e1c.e1config.Netmask[3] = 0;
    e1c.e1config.Gateway[0] = 192;
    e1c.e1config.Gateway[1] = 168;
    e1c.e1config.Gateway[2] = 1;
    e1c.e1config.Gateway[3] = 1;
    e1c.e1config.DeviceName = DEVICE_NAME;
    e1c.e1config.deviceNum = E131_NODE_DEVICE_NUM;
    e1c.e1config.moduleNum = E131_NODE_MODULE_NUM;
    e1c.e1config.DMXoutputOn = E131_DMX_OUTPUT;
    e1c.e1config.universe = E131_UNIVERSE;
    e1c.e1config.DxmPowerOffTime = 2;
    e1c.e1config.LEDconfig = E131_LED_CONFIG;
    e1c.e1config.batAnalogRef = ANALOG_REF_mV;
    e1c.e1config.batDividerRatio = DIVIDER_RATIO;
    e1c.e1config.OLEDoutputOn = false;
    e1c.e1config.Firmware = FIRMWARE;

    // WriteConfig();

} //flash_get_config()
