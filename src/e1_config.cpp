/*
    e1_config.cpp
    
*/
#include <e1_config.h>

/* Constructor */
E1CONFIG::E1CONFIG()
{
    esp_level = 0;
    esp_status = "really unknown";
    bStartup = true;
    SoftAPup = false;
    fwVers = "v9.99";
    nodePortMulti = NODE_PORT;
    e1config.universe = E131_UNIVERSE;
    e1config.deviceNum = E131_NODE_DEVICE_NUM;
    e1config.moduleNum = E131_NODE_MODULE_NUM;
    e1config.DMXoutputOn = E131_DMX_OUTPUT;
    e1config.LEDconfig = E131_LED_CONFIG;
}
