#ifndef _CUSTOMWEBSERVER_
#define _CUSTOMWEBSERVER_

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <WebSite.h>

#define AP_SSID "LED Server Init"

class CustomWebServer
{
 protected:
    ESP8266WebServer* hostingWebServer = NULL;
    WebSite* setupSite = NULL;
    WebSite* normalSite = NULL;
    IPAddress IP;
    bool isOpen = false;

 private:
   bool isDebugSerial = false;

 public:
   CustomWebServer(WebSite* setupSite, WebSite* normalSite);
   ~CustomWebServer();

   //custom html stuff
   static void beginHtmlDoc(String& html, String caption);
   static void endHtmlDoc(String& html);

   //web server stuff
   void handleClient();
   void showPage(String html, String caption);
   void doRedirect(String location);

   bool isServerOpen();

   ESP8266WebServer* getWebServer() { return hostingWebServer; };


   //EEPROM stuff
   bool hasValidCredentials();
   bool isUsingAP();
   void writeUseAPToEEPROM(bool val);
   bool getWLANCredentialsFromEEPROM(String& ssid, String& pw);
   void cleanUpEEPROM();
   void invalidateWLANCredentials();
   void writeWLANCredentialsToEEPROM(String& ssid, String& pw);

   //special behaviour
   void launchServerInNetwork(String ssid, String pw, bool useAP);

   operator String()
   {
       return String("Custom webserver: \nIP: ") + IP.toString() + String("\n");
   };
};
#endif