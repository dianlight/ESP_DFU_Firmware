#include <Arduino.h>
/**
 * @brief This is a minimal DFU firmware for ESP8266
 * 
 * Features:
 * - WiFiManager support
 * - OLED display information
 * - OTA support
 * 
 * @see README.md to check valid configurations
 */
#include "Config.h"
#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#ifdef USE_WIFIMANAGER
  #include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
  #include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
  #include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#endif

#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
// Include the correct display library
// For a connection via I2C using Wire include
//#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#if defined(USE_DISPLAY) && USE_DISPLAY == 0
  #include "SSD1306Wire.h" 
  SSD1306Wire  display(I2C_OLED_ADDRESS, SDA, SCL);
#elif defined(USE_DISPLAY) && USE_DISPLAY == 1
  #include "SH1106Wire.h"
  SH1106Wire display(I2C_OLED_ADDRESS, SDA, SCL);
#elif defined(USE_DISPLAY) && USE_DISPLAY == 2
  #include "SSD1306Spi.h"
  SSD1306Spi  display(RES, DC, CS);
#elif defined(USE_DISPLAY) && USE_DISPLAY == 3
  #include "SH1106SPi.h"
  SH1106Spi   display(RES, DC);
#elif defined(USE_DISPLAY)
    #error Invalid value for USE_DISPLAY param. See Readme.md      
#endif

#ifdef SERIAL_DEBUG
  #define DEBUG Serial.printf
#else
  #define DEBUG //
#endif


void setup() {
  // put your setup code here, to run once:
  #ifdef SERIAL_DEBUG
    Serial.begin(9600);
    delay(2000);
  #endif
  DEBUG("DFU Firmware ready\n");

  #ifdef USE_WIFIMANAGER
    DEBUG("Use WiFiManager");
    WiFiManager wifiManager;
    wifiManager.autoConnect();
  #else 
    DEBUG("Use Manual Connection");
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    WiFi.begin();
    delay(500);
    int8_t status = WiFi.waitForConnectResult();
    if(status != WL_CONNECTED){
      DEBUG("No connection start AP mode");
      IPAddress local_IP(192,168,4,1);
      IPAddress gateway(192,168,4,1);
      IPAddress subnet(255,255,255,0);

      WiFi.mode(WIFI_AP); // drop station mode if LAN/WiFi is down
      WiFi.softAPConfig(local_IP,gateway,subnet);
      WiFi.softAP(MDNS_NAME_STR "_AP");
      WiFi.softAPIP();
    }
  #endif

  #ifdef USE_DISPLAY
    DEBUG("Use Display " USE_DISPLAY_STR "\n");
    display.init();
    display.flipScreenVertically();
    display.setContrast(255);
  #endif

  ArduinoOTA.setHostname(MDNS_NAME_STR);
  ArduinoOTA.begin(true);
  DEBUG("OTA Satrted\n");
  ArduinoOTA.onStart([]() {
    #ifdef USE_DISPLAY
      display.clear();
      display.setFont(ArialMT_Plain_10);
      display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
      display.drawString(display.getWidth()/2, display.getHeight()/2 - 10, "OTA Update");
      display.display();
    #endif
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    #ifdef USE_DISPLAY
      display.drawProgressBar(4, 32, 120, 8, progress / (total / 100) );
      display.display();
    #endif
  });

  ArduinoOTA.onEnd([]() {
    #ifdef USE_DISPLAY
      display.clear();
      display.setFont(ArialMT_Plain_10);
      display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
      display.drawString(display.getWidth()/2, display.getHeight()/2, "Restart");
      display.display();
    #endif
  });

  #ifdef USE_DISPLAY
    // Align text vertical/horizontal center
    display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    display.setFont(ArialMT_Plain_10);
    if(WiFi.getMode() == WIFI_AP){
      display.drawString(display.getWidth()/2, display.getHeight()/2, "AP " MESSAGE_STR "\n" + WiFi.softAPSSID() + "\n" + WiFi.softAPIP().toString()+ "\n" + MDNS_NAME_STR + ".local");
    } else {
      display.drawString(display.getWidth()/2, display.getHeight()/2, MESSAGE_STR "\n" + WiFi.localIP().toString()+ "\n" + MDNS_NAME_STR + ".local");
    }
    display.display();
  #endif
    if(WiFi.getMode() == WIFI_AP){
      DEBUG("AP Ready for OTA:\n%s %s %s.local\n",WiFi.softAPSSID().c_str(),WiFi.softAPIP().toString().c_str(),MDNS_NAME_STR);
    } else {
      DEBUG("Ready for OTA:\n%s %s.local\n",WiFi.localIP().toString().c_str(), MDNS_NAME_STR);
    }
}

void loop() {
  ArduinoOTA.handle();
}