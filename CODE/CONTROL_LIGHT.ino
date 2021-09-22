#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
int led1=12;
int led2=13;
 /************************* WiFi Access Point *********************************/

#define WLAN_SSID       "upstair"
#define WLAN_PASS       "135792468"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "hoangpnh2000"
#define AIO_KEY         "45e8582e8b714fdba4970735060dd5e9"

/************ Global State (you don't need to change this!) ******************/


WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Subscribe light1onoff = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/light1");

Adafruit_MQTT_Subscribe light2onoff = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/light2");
/*************************** Sketch Code ************************************/


void MQTT_connect();

void setup() {
pinMode(led1, OUTPUT);
pinMode(led2,OUTPUT);
  Serial.begin(115200);
  delay(10);

  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&light1onoff);
 mqtt.subscribe(&light2onoff);
}

uint32_t x=0;

void loop() { 
  MQTT_connect();

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) 
  {
   
    if (subscription == &light1onoff) {
      Serial.print(F("light1onoff: "));
      Serial.println((char *)light1onoff.lastread);
      
      if (strcmp((char *)light1onoff.lastread, "ON") == 0) {
        digitalWrite(led1, LOW); 
      }
      if (strcmp((char *)light1onoff.lastread, "OFF") == 0) {
        digitalWrite(led1, HIGH); 
      }
    }

   else if(subscription == &light2onoff){
       //Print the new value to the serial monitor
      Serial.print("light2onoff: ");
      Serial.println((char *) light2onoff.lastread);
   
      //If the new value is  "ON", turn the light on.
      //Otherwise, turn it off.
      if (!strcmp((char *) light2onoff.lastread, "ON"))
      {
        //Active low logic
        digitalWrite(led2, LOW);
      }
      else if (!strcmp((char *) light2onoff.lastread, "OFF"))
      {
        digitalWrite(led2, HIGH);
      }
    }
    
  }
}
void MQTT_connect() {
  int8_t ret;

  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
