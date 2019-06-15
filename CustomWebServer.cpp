#include <CustomWebServer.h>

CustomWebServer::CustomWebServer(WebSite* setupSite, WebSite* normalSite)
{
    EEPROM.begin(512);
    this->setupSite = setupSite;
    this->normalSite = normalSite;

    String ssid = "";
    String pw = "";
    getWLANCredentialsFromEEPROM(ssid, pw);

    isDebugSerial = (bool)Serial;

    hostingWebServer = NULL;

    launchServerInNetwork(ssid, pw, isUsingAP());
}

CustomWebServer::~CustomWebServer()
{
  EEPROM.commit();
  EEPROM.end();
}

void CustomWebServer::handleClient()
{
  hostingWebServer->handleClient();
}

void CustomWebServer::beginHtmlDoc(String& html, String caption)
{
  float font_size = (float)(100.f/(float)caption.length())*1.1f;
  if (font_size <= 0.f)
  {
    font_size = 1.f;
  }
  
  html = "<html>\n<header>\n";
  //CSS
  html += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\">\n";
  html += "<title>"+caption+"</title>\n<meta charset=\"utf-8\">\n";
  html += "</header>\n<body>\
  <div class=\"container\" style=\"align-content: center; width: 90vw;\">\n\
  <div class=\"row\">\
  <div class=\"col-xs-12 col-sm-12 col-md-12 col-lg-12\">\
  <h1 style=\"font-size:"+String(font_size)+"vw; align-content: center; align-text: center; width: 100%;\">"+caption+"</h1>\
  </div>\n</div>\
  <div class=\"well\" style=\"margin: auto; width: 80%\">\n";
}

void CustomWebServer::endHtmlDoc(String& html)
{
  html += "</div></div>\n\
  <a class=\"btn btn-primary\" style=\"position: fixed; bottom: 5vh; right: 5vw;\" href=\"/cleanUp\">Reset chip</a>\
  <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.4.0/jquery.min.js\"></script>\n\
  <script src=\"https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.7/umd/popper.min.js\"></script>\n\
  <script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js\"></script>\n\
  </body>\n</html>";
}

void CustomWebServer::showPage(String html, String caption)
{
  if(isDebugSerial) Serial.println("Show Page: "+caption);
  String preHtml = "";
  CustomWebServer::beginHtmlDoc(preHtml, caption);
  html = preHtml + html;
  CustomWebServer::endHtmlDoc(html);
  hostingWebServer->send(200, "text/html", html);
}

bool CustomWebServer::hasValidCredentials()
{
  return EEPROM.read(0) == 1;
}

bool CustomWebServer::isUsingAP()
{
  return EEPROM.read(0) == 2;
}

void CustomWebServer::writeUseAPToEEPROM(bool val)
{
  if (val)
  {
    EEPROM.write(0, 2);
  }
  else
  {
    EEPROM.write(0, 0);
  }
  
  EEPROM.commit();
}

bool CustomWebServer::getWLANCredentialsFromEEPROM(String& ssid, String& pw)
{
  if (!hasValidCredentials())
    return false;

  if (isUsingAP())
    return true;

  ssid = "";
  int lastReadPos = 0;
  for(int i = 1; i < EEPROM.length(); i++)
  {
    char c = EEPROM.read(i);
    if (c == 0)
    {
      lastReadPos = i;
      break;
    }
    ssid += String(c);
  }

  if (lastReadPos == 0 || lastReadPos == EEPROM.length())
    return false;

  pw = "";
  for(int i = lastReadPos + 1; i < EEPROM.length(); i++)
  {
    char c = EEPROM.read(i);
    if (c == 0)
    {
      break;
    }
    pw += String(c);  
  }
  
  return true;
}

void CustomWebServer::writeWLANCredentialsToEEPROM(String& ssid, String& pw)
{
  if (2 + ssid.length() + pw.length() > EEPROM.length())
  {
    return;
  }
  
  // flag that wlan credentials are written
  EEPROM.write(0, 1);
  for (int i = 0; i < ssid.length(); i++)
    EEPROM.write(1+i, (byte)ssid[i]);

  // terminate string
  EEPROM.write(ssid.length()+1, 0);


  int offset = ssid.length() + 2;
  for (int i = 0; i < pw.length(); i++)
    EEPROM.write(offset+i, (byte)pw[i]);

  if (offset + pw.length() < EEPROM.length())
    // Terminate only if needed
    EEPROM.write(offset + pw.length(), 0);

  if (!EEPROM.commit())
    if(isDebugSerial) Serial.println("Can't write to EEPROM!");
}

void CustomWebServer::cleanUpEEPROM()
{
  for (int i = 0; i < EEPROM.length(); i++)
    EEPROM.write(i, 0);

  EEPROM.commit();
}

void CustomWebServer::invalidateWLANCredentials()
{
  // flag that wlan credentials as obsolete
  EEPROM.write(0, 0);
  EEPROM.commit();
}

void CustomWebServer::doRedirect(String location)
{
  hostingWebServer->sendHeader("Location", location, true);
  hostingWebServer->send(303, "text/plain", "");
}

void CustomWebServer::launchServerInNetwork(String ssid, String pw, bool useAP)
{
  if(hostingWebServer != NULL)
  {
    hostingWebServer->stop();
    delete hostingWebServer;
  }

  WiFi.disconnect(true);
  WiFi.softAPdisconnect(true);

  isOpen = false;

  hostingWebServer = new ESP8266WebServer(80);

  if ((ssid == String("") && useAP) || !hasValidCredentials())
  {
    if(isDebugSerial) Serial.println("launch Server in AP mode!");
    WiFi.softAP(AP_SSID); //init wifi mode access point
    IP = WiFi.softAPIP();
    if (useAP)
    {
      this->normalSite->configureWebserver(hostingWebServer);
      if(isDebugSerial) Serial.println("launch LED Server");
    }
    else 
    {
      this->setupSite->configureWebserver(hostingWebServer);
      if(isDebugSerial) Serial.println("launch Settings Server");
    }
    isOpen = true;
  }
  else
  {
    WiFi.mode(WIFI_STA);
    isOpen = (WiFi.begin(ssid.c_str(), pw.c_str()) == WL_CONNECTED);
    IP = WiFi.localIP();
    this->normalSite->configureWebserver(hostingWebServer);
    if(isDebugSerial) Serial.println("Connected as integrated Server! (In: "+ssid+")");
  }

  if (isUsingAP() != useAP)
    writeUseAPToEEPROM(useAP);

  String oldSSID = "";
  String oldPW = "";
  getWLANCredentialsFromEEPROM(oldSSID, oldPW);
  if (oldSSID != ssid)
    writeWLANCredentialsToEEPROM(ssid, pw);
  
  hostingWebServer->begin();
}

bool CustomWebServer::isServerOpen()
{
  return isOpen;
}