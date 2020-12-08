#include <littleFS.h>
#include <ESP8266mDNS.h>

#include "WiFiManager.h"
#include "HttpServer.h"
#include "Logger.h"
#include "WS281xDriver.h"

/********************************************************/
/******************** Public Method *********************/
/********************************************************/

HttpServer::HttpServer()
    : _webServer(80)
{
}

HttpServer::~HttpServer()
{
}

void HttpServer::setup(void)
{
  _webServer.on("/restart", [&]() {
    _webServer.send(200, "text/plain", "ESP restart now !");
    delay(200);
    ESP.restart();
  });

  _webServer.on("/wifimanager", [&]() {
    _webServer.send(200, "text/plain", "Reset WifiManager configuration, restart now in AP mode...");
    delay(200);
    WiFiManager wifiManager;
    wifiManager.resetSettings();
    delay(200);
    ESP.restart();
  });

  _webServer.on("/cmd", HttpServer::handleCmd);

  _webServer.onNotFound(HttpServer::handleNotFound);

  _webServer.begin();
}

void HttpServer::handle(void)
{
  _webServer.handleClient();
}

String HttpServer::getContentType(String filename)
{
  if (_webServer.hasArg("download"))
    return "application/octet-stream";
  else if (filename.endsWith(".htm"))
    return "text/html";
  else if (filename.endsWith(".html"))
    return "text/html";
  else if (filename.endsWith(".css"))
    return "text/css";
  else if (filename.endsWith(".js"))
    return "application/javascript";
  else if (filename.endsWith(".png"))
    return "image/png";
  else if (filename.endsWith(".gif"))
    return "image/gif";
  else if (filename.endsWith(".jpg"))
    return "image/jpeg";
  else if (filename.endsWith(".ico"))
    return "image/x-icon";
  else if (filename.endsWith(".xml"))
    return "text/xml";
  else if (filename.endsWith(".pdf"))
    return "application/x-pdf";
  else if (filename.endsWith(".zip"))
    return "application/x-zip";
  else if (filename.endsWith(".gz"))
    return "application/x-gzip";
  return "text/plain";
}

// send the right file to the client (if it exists)
bool HttpServer::handleFileRead(String path)
{
  Log.println("handleFileRead: " + path);
  if (path.endsWith("/"))
  {
    path += "index.html"; // If a folder is requested, send the index file
  }
  String contentType = HTTPServer.getContentType(path); // Get the MIME type
  String pathWithGz = path + ".gz";
  if (LittleFS.exists(pathWithGz) || LittleFS.exists(path))
  {                                                       // If the file exists, either as a compressed archive, or normal
    if (LittleFS.exists(pathWithGz))                      // If there's a compressed version available
      path += ".gz";                                      // Use the compressed verion
    File file = LittleFS.open(path, "r");                 // Open the file
    HTTPServer.webServer().streamFile(file, contentType); // Send it to the client
    file.close();                                         // Close the file again
    Log.println(String("\tSent file: ") + path);
    return true;
  }
  Log.println(String("\tFile Not Found: ") + path); // If the file doesn't exist, return false
  return false;
}

void HttpServer::handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += HTTPServer.webServer().uri();
  message += "\nMethod: ";
  message += (HTTPServer.webServer().method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += HTTPServer.webServer().args();
  message += "\n";
  for (uint8_t i = 0; i < HTTPServer.webServer().args(); i++)
  {
    message += " " + HTTPServer.webServer().argName(i) + ": " + HTTPServer.webServer().arg(i) + "\n";
  }
  HTTPServer.webServer().send(404, "text/plain", message);
}

void HttpServer::handleCmd()
{
  String message;
  for (uint8_t i = 0; i < HTTPServer.webServer().args(); ++i)
  {
    String name = HTTPServer.webServer().argName(i);
    String value = HTTPServer.webServer().arg(i).c_str();

    LedDriver.sendCommand(name, value);
  }

  HTTPServer.webServer().send(200, "text/plain", message);
}

ESP8266WebServer &HttpServer::webServer()
{
  return _webServer;
}

/********************************************************/
/******************** Private Method ********************/
/********************************************************/

void HttpServer::sendJson(const uint16 code, JsonDocument &doc)
{
  WiFiClient client = HTTPServer.webServer().client();

  // Write Header
  client.print(F("HTTP/1.0 "));
  client.print(code);
  client.println(F(" OK"));
  client.println(F("Content-Type: application/json"));
  client.println(F("Access-Control-Allow-Origin: *"));
  client.print(F("Content-Length: "));
  client.println(measureJson(doc));
  client.println(F("Connection: close"));
  client.println();

  // Write JSON document
  serializeJson(doc, client);
}

#if !defined(NO_GLOBAL_INSTANCES)
HttpServer HTTPServer;
#endif