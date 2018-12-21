// Copyright (c) 2018 Sergey Burnevsky (sergey.burnevsky @ gmail.com)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <time.h>

#include "ota.h"
#include "web.h"
#include "cred.h"

WiFiUDP udp;
NTPClient ntpClient(udp, "europe.pool.ntp.org", 7200, 600000);
ventil::Web webServer(80);

static const char* MONTHS[] = { "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };
static const char* DAYS[] = { "SUN", "MON", "TUE", "WED", "THR", "FRI", "SAT", "SUN" };
static const uint32_t K_DEBUG_INTERVAL_MS = 20000;

static uint8_t macAddr[WL_MAC_ADDR_LENGTH];
static bool isTimeUpdateNeeded = true;
static uint32_t gLastDebugTime = 0;

using namespace ventil;

void setup()
{
    Serial.begin(115200);
    Serial.println("Booting");

    // WIFI
    WiFi.mode(WIFI_STA);
    WiFi.begin(Cred::WLAN_SSID, Cred::WLAN_PASSWD);
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
    ntpClient.begin();

    webServer.begin();

    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    Serial.printf("ChipId %x, FlashId %x, FSize %u, FRsize %u, Freq %d, VCC: %hu\n",
                  ESP.getChipId(),
                  ESP.getFlashChipId(),
                  ESP.getFlashChipSize(),
                  ESP.getFlashChipRealSize(),
                  ESP.getCpuFreqMHz(),
                  ESP.getVcc());

    Serial.println("Ready");
}

void loop()
{
    if (isTimeUpdateNeeded)
    {
        if (ntpClient.update())
        {
            isTimeUpdateNeeded = false;
            Serial.printf("NTP Time: %d\n", ntpClient.getEpochTime());
        }
        else
        {
            Serial.println("NTP failed!");
        }
    }

    otaHandle();
    if (otaIsUpdating())
    {
        return;
    }

    webServer.handleClient();

    unsigned long now_ms = millis();
    if (now_ms - gLastDebugTime > K_DEBUG_INTERVAL_MS)
    {
        gLastDebugTime = now_ms;

        time_t now_s = ntpClient.getEpochTime(); //time(NULL);
        tm tm_now = { 0 };
        localtime_r(&now_s, &tm_now);

        Serial.printf("%04u-%s-%02u %02u:%02u:%02u, %s, day #%d\n",
            tm_now.tm_year + 1900, MONTHS[tm_now.tm_mon], tm_now.tm_mday,
            tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec,
            DAYS[tm_now.tm_wday], tm_now.tm_yday + 1);
    }
}
