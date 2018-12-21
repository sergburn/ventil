// Copyright (c) 2018 Sergey Burnevsky (sergey.burnevsky @ gmail.com)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "web.h"
#include "cred.h"

const char* www_realm = "Ventil";
const char* www_authFailResponse = "Authentication Failed";

namespace ventil {

Web::Web(int port) :
    mWebServer(port)
{
}

void Web::begin()
{
    mWebServer.on("/schedules", [this]() { onGetSchedules(); });
    mWebServer.on("/schedule", HTTP_POST, [this]() { Web::onPostSchedule(); });
    mWebServer.begin();
}

void Web::handleClient()
{
    mWebServer.handleClient();
}

bool Web::isAuthenticated()
{
    if (!mWebServer.authenticate(Cred::WEB_USER, Cred::WEB_PASSWD))
    {
        // Basic
        // return server.requestAuthentication();
        // Digest Auth Method with Custom realm and Failure Response
        mWebServer.requestAuthentication(DIGEST_AUTH, www_realm, www_authFailResponse);
        return false;
    }
    return true;
}

void Web::onGetSchedules()
{
    if (isAuthenticated())
    {
        mWebServer.send(200, "text/plain", "Login OK");
    }
}

void Web::onPostSchedule()
{
    if (isAuthenticated())
    {
        mWebServer.send(200, "text/plain", "Post OK");
    }
}

} // namespace ventil
