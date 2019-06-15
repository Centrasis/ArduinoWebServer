#ifndef _WEBSITE_
#define _WEBSITE_

#include <ESP8266WebServer.h>
#include <vector>
#include <functional>

class WebSite
{
  protected:
    std::vector<std::pair<std::string, ESP8266WebServer::THandlerFunction>> subSites;
    ESP8266WebServer* attachedWebServer = NULL;
    void onNotFound();

  public:
    WebSite();
    void addSubSite(const char* siteLocation, ESP8266WebServer::THandlerFunction action);
    void configureWebserver(ESP8266WebServer* webserver);
    void doRedirect(String location);
};
#endif