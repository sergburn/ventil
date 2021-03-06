// Copyright (c) 2018 Sergey Burnevsky (sergey.burnevsky @ gmail.com)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include <ESP8266WebServer.h>

namespace ventil {

class Web
{
public:
    Web(int port = 80);

    void begin();
    void handleClient();

private:
    bool isAuthenticated();

    void onGetSchedules();
    void onPostSchedule();

private:
    ESP8266WebServer mWebServer;
};

} // namespace ventil
