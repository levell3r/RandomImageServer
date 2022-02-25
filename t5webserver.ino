#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include "epd_driver.h"
#include "arduino_secrets.h"

// Lilygo T5 resolution is 960 x 540
//
//SET Api's - you will need to hardcode your Ip addresses
const char* api_url = "http://192.168.1.195:12345/image";
const char* load_url = "http://192.168.1.195:54321/load";
const char* shutdown_url = "http://192.168.1.195:54321/shutdown";

//No stealing my stuff  -  change this to yours obvs.
const char* ssid     = SECRET_SSID;
const char* password = SECRET_PASS;



// how long we'll sleep (minutes)
long SleepDuration   = 30;
// timer to check how long we've been awake
long StartTime       = 0;
// save wifi signal strenght
int wifi_signal;

// declare framebuffer and (network) image buffer
// these'll be initialized into SPI-RAM in our setup function
uint8_t *framebuffer;
uint8_t *imagebuffer;
int image_width, image_height;

const char * headerKeys[] = {"Image-Width", "Image-Height"};
const size_t numberOfHeaders = 2;

uint8_t StartWiFi() {
  Serial.println("Connecting to: " + String(ssid));
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("STA: Failed!\n");
    WiFi.disconnect(false);
    delay(500);
    WiFi.begin(ssid, password);
  }
  if (WiFi.status() == WL_CONNECTED) {
    wifi_signal = WiFi.RSSI(); // Get Wifi Signal strength now, because the WiFi will be turned off to save power!
    Serial.println("WiFi connected at: " + WiFi.localIP().toString());
  }
  else Serial.println("WiFi connection *** FAILED ***");
  return WiFi.status();
}

void StopWiFi() {
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  Serial.println("WiFi switched Off");
}

void boot_imageserver() {
    HTTPClient http;
    http.begin(load_url);
    int httpCode = http.GET();
}

void shutdown_imageserver() {
    HTTPClient http;
    http.begin(shutdown_url); 
    int httpCode = http.GET();
}

void draw_image()
{
    epd_poweron();
    epd_clear();
    epd_draw_grayscale_image(epd_full_screen(), framebuffer); 
    epd_poweroff();
}

//Sleep Function - to be continued...
void BeginSleep() {
  epd_poweroff_all();
  long SleepTimer = SleepDuration * 60; 
  esp_sleep_enable_timer_wakeup(SleepTimer * 1000000LL); 
  Serial.println("Awake for : " + String((millis() - StartTime) / 1000.0, 3) + "-secs");
  Serial.println("Entering " + String(SleepTimer) + " (secs) of sleep time");
  Serial.println("Starting deep-sleep period...");
  esp_deep_sleep_start(); 
}



void QueryServer()
{
  HTTPClient http;
  Serial.print("[HTTP] begin...\n");
  // configure server and url
  http.begin(api_url); //HTTP
  http.collectHeaders(headerKeys, numberOfHeaders);
  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();
  // httpCode will be negative on error
  if(httpCode > 0)
  {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      // file found at server
      if(httpCode == HTTP_CODE_OK) {
          // get lenght of document (is -1 when Server sends no Content-Length header)
          int len = http.getSize();
          Serial.printf("[HTTP] Server says it'll send a response with %i bytes (excluding headers)\n", len);
          // we've made some custom HTTP Response headers which tell us what size the image was
          image_width = http.header("Image-Width").toInt();
          image_height = http.header("Image-Height").toInt();
          // get a handle to the tcp stream
          WiFiClient * stream = http.getStreamPtr();
          // keep track of how many bytes we've read thus far
          size_t read = 0;
          // read all data from server
          while(http.connected() && (len > 0 || len == -1))
          {
              // get available data size
              size_t size = stream->available();
              // if size > 0, read the bytes into 
              if(size) {
                  // read http bytes progressively into our imagebuffer
                  int c = stream->readBytes(imagebuffer + read, 2048);

                  if(len > 0) {
                      len -= c;
                  }
                  read += c;
              }
          }
      }
  } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}


void setup()
{
    // start
    Serial.begin(115200);
    Serial.println("Wakey wakey!");
    StartTime = millis();

    // initialize framebuffer space in SPIRAM
    framebuffer = (uint8_t *)heap_caps_malloc(EPD_WIDTH * EPD_HEIGHT / 2, MALLOC_CAP_SPIRAM);
    memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);
    imagebuffer = (uint8_t *)heap_caps_malloc(EPD_WIDTH * EPD_HEIGHT / 2, MALLOC_CAP_SPIRAM);
    memset(imagebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);

    // initialize the screen
    epd_init();
    
    // start WIFI
    if (StartWiFi() == WL_CONNECTED)
    {
      //call the bootup API and wait for it to load
      boot_imageserver();
      Serial.println("Boot Image API");
      delay(20000);
      QueryServer();
      // image data resides in imagebuffer now, copy it over to the framebuffer
      Rect_t area = {
          .x = (EPD_WIDTH - image_width) / 2,
          .y = (EPD_HEIGHT - image_height) / 2,
          .width = image_width,
          .height =  image_height
      };
      epd_copy_to_framebuffer(area, imagebuffer, framebuffer);
      // do a fullscreen redraw with the framebuffer
      draw_image();
    }
    // we don't need the buffers anymore, free them
    free(framebuffer);
    free(imagebuffer);
    
    //Shutdown the imageserver so it will randomly pick new image on next iteration
    shutdown_imageserver();

    //sleep function for battery saving - to be continued..
    BeginSleep();
}


void loop()
{
    delay(3000);
}
