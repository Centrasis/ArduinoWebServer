#include "WebSite.h"

WebSite::WebSite()
{

}

void WebSite::addSubSite(const char* siteLocation, ESP8266WebServer::THandlerFunction action)
{
  subSites.push_back(std::pair<std::string, ESP8266WebServer::THandlerFunction>(std::string(siteLocation), action));
}

void WebSite::configureWebserver(ESP8266WebServer* webserver)
{
  attachedWebServer = webserver;
  for(auto site: subSites)
  {
    webserver->on(site.first.c_str(), site.second);
  }
  std::function<void()> _onNotFound = std::bind(&WebSite::onNotFound, this);
  webserver->onNotFound((ESP8266WebServer::THandlerFunction)_onNotFound);
}

void WebSite::onNotFound()
{
  attachedWebServer->send(200, "text/html", "<a href=\"/\" style=\"font-size:10vw;\">Zur Startseite</a>");
  // redirect see other
  doRedirect(String("/"));
}

void WebSite::doRedirect(String location)
{
  attachedWebServer->sendHeader("Location", location, true);
  attachedWebServer->send(303, "text/plain", "");
}
