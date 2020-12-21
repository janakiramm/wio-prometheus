#include <ArduinoHttpClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

#include "arduino_secrets.h"

#include"Free_Fonts.h"
#include"TFT_eSPI.h"
TFT_eSPI tft;
TFT_eSprite spr = TFT_eSprite(&tft);

//set the ssid and password in the arduino_secrets.h
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

//set the address and port of Kubernetes NodePort or LoadBalancer service
char serverAddress[] = "10.0.0.243";  
int port = 80;

WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);
int status = WL_IDLE_STATUS;

StaticJsonDocument<200> doc;

const char* nodes; 
const char* cpu; 
const char* mem;
const char* pods;
const char* containers;
const char* ver;


void setup() {
    Serial.begin(115200);
    delay(100);

    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    WiFi.begin(ssid, pass);
    
    tft.begin();
    tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);
    
    tft.setTextSize(2);
    tft.setCursor((320 - tft.textWidth("Connecting to Wi-Fi.."))/2, 120);
    tft.print("Connecting to Wi-Fi...");
 
    // attempt to connect to Wifi network:
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        // wait 1 second for re-trying
        delay(1000);
    }
 
    Serial.print("Connected to ");
    Serial.println(ssid);
 
    tft.fillScreen(TFT_BLACK);
    tft.setCursor((320 - tft.textWidth("WiFi Connected!"))/2, 120);
    tft.print("WiFi Connected!");

    //verify connectivity 
    Connect();        

}

void Connect() {
    tft.fillScreen(TFT_BLACK);
    tft.setCursor((320 - tft.textWidth("Connecting to Server.."))/2, 120);
    tft.print("Connecting to Server..");
    Serial.println("Connecting to Server...");
    client.get("/metrics");
  
    int statusCode = client.responseStatusCode();
    String response = client.responseBody();
    Serial.println(statusCode);
    
    if (statusCode==200) {
    
      tft.fillScreen(TFT_BLACK);
      tft.setCursor((320 - tft.textWidth("Connected..."))/2, 120);
      tft.print("Connected...");      
    }
    else {

      tft.fillScreen(TFT_BLACK);
      tft.setCursor((320 - tft.textWidth("Problem Connecting..."))/2, 120);
      tft.print("Problem Connecting...");      

    }
}

void loop() {
  //invoke the endpoint every 30s
  getData();
  delay(30000);

}

void getData() {

    client.get("/metrics");
  
    int statusCode = client.responseStatusCode();
    String response = client.responseBody();
    Serial.println(statusCode);

    if (statusCode!=200) {
      tft.fillScreen(TFT_BLACK);
      tft.setCursor((320 - tft.textWidth("Problem Connecting..."))/2, 120);
      tft.print("Problem Connecting...");      
    }

    //parse the JSON response
    DeserializationError error = deserializeJson(doc, response);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
  
    nodes = doc["nodes"];
    cpu = doc["cpu"];
    mem = doc["memory"];
    pods = doc["pods"];
    containers = doc["containers"];
    ver = doc["version"];
  
    Serial.println(nodes);
    Serial.println(cpu);
    Serial.println(mem);
    Serial.println(pods);
    Serial.println(containers);
    Serial.println(ver);

    tft.fillScreen(tft.color565(0,0,255)); 
    tft.setTextColor(TFT_CYAN);    
    tft.setCursor((320 - tft.textWidth("Kubernetes Dashboard"))/2, 25);
    tft.print("Kubernetes Dashboard");      
    
    tft.drawFastHLine(0,55,320,TFT_WHITE); 
    tft.setTextColor(TFT_YELLOW);
    tft.setCursor((320 - tft.textWidth(ver))/2, 62);
    tft.print(ver);      
    tft.drawFastVLine(160,90,200,TFT_WHITE); 
    tft.drawFastHLine(0,85,320,TFT_WHITE); 
    
    tft.setTextColor(TFT_WHITE); 
    tft.drawString("Nodes",65,95);
    tft.setTextColor(TFT_YELLOW); 
    tft.drawString(nodes,180,95); 
    tft.drawFastHLine(0,125,320,TFT_WHITE); 
    tft.setTextColor(TFT_WHITE);
    
    tft.drawString("CPU",15,135);
    tft.setTextColor(TFT_YELLOW);
    tft.drawString(cpu,77,135); 
    tft.setTextColor(TFT_WHITE);
    tft.drawString("MEM",180,135);
    tft.setTextColor(TFT_YELLOW);
    tft.drawString(mem,250,135); 
    tft.drawFastHLine(0,160,320,TFT_WHITE); 
  
    tft.setTextColor(TFT_WHITE); 
    tft.drawString("Pods",77,175);
    tft.setTextColor(TFT_YELLOW); 
    tft.drawString(pods,180,175); 
    tft.drawFastHLine(0,200,320,TFT_WHITE); 
    tft.setTextColor(TFT_WHITE);
  
    tft.setTextColor(TFT_WHITE); 
    tft.drawString("Containers",13,210);
    tft.setTextColor(TFT_YELLOW); 
    tft.drawString(containers,180,210); 
    tft.setTextColor(TFT_WHITE);    
  
}
