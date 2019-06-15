#ifndef _WIFIUTILS_
#define _WIFIUTILS_

#include <ESP8266WebServer.h>

#define MAX_NETS_LIST 10

class WiFiUtils
{
public:
    static void getWiFiNetworks(String* nets, int& count)
    {
        int netCount = WiFi.scanNetworks();
        if (netCount < count)
            count = netCount;

        for (int i = 0; i < count; i++)
            nets[i] = String(WiFi.SSID(i));
    }

    static void appendNamedSelection(String& html, String* optArray, int count, String name, bool isRequired)
    {
        html+="\n<select name=\""+name+"\"";
        if (isRequired)
            html += " required ";
        html += ">\n";

        for (int i = 0; i < count; i++)
            html += "<option value=\""+optArray[i]+"\">"+optArray[i]+"</option>\n";

        html += "</select>\n";
    }
};

#endif