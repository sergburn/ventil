#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <time.h>

#include "ota.h"
#include "credentials.h"

WiFiUDP udp;
NTPClient ntpClient(udp);

static uint8_t macAddr[WL_MAC_ADDR_LENGTH];

void setup()
{
    Serial.begin(115200);
    Serial.println("Booting");

    // WIFI
    WiFi.mode(WIFI_STA);
    WiFi.begin(WLAN_SSID, WLAN_PASSWD);
    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }

    WiFi.macAddress(macAddr);
    Serial.print("MAC: ");
    Serial.println(WiFi.macAddress());

    otaSetup();
    Serial.println("Ready");
}

void loop()
{
    otaHandle();
    if (!otaIsUpdating())
    {
    }
}
