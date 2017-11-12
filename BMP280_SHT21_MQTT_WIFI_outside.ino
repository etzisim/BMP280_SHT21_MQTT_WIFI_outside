#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <PubSubClient.h>
#include <SHT21.h>
#include <ESP8266WiFi.h>


//BMP280
#define BMP_SCK D3
#define BMP_MISO D2
#define BMP_MOSI D4 
#define BMP_CS D5

Adafruit_BMP280 bme;
SHT21 SHT21;

//WIFI

const char ssid[] = "wifi";      // your network SSID (name)
const char password[] = "pass"; // your network key


//#####MQTT#######

const char* mqtt_server = "192.168.1.2";

WiFiClient espClient;
PubSubClient client(espClient);
const char ClientID[] = "sensor_outside";  
const char mqtt_user[] = "nodered";  
const char mqtt_pass[] = "passwordfornodered";



void setup() {
  Serial.begin(9600);
  Wire.begin(D2,D3);

  if(bme.begin()){
    Serial.println("BME280 Sensor connected");
  }
  SHT21.begin();
  Serial.println("");
  Serial.println("");
  Serial.println("");
  Serial.println(""); 
}



void loop() {
  if (!WiFi.isConnected()) {
    connectWifi();
  }else{
    Serial.println("WiFi Connected");
  }
  if (!client.connected()){
    connectMQTT();
  }else{
    Serial.println("MQTT Connected");
  }
  
  client.publish("home-assistant/temp/Temperature_outside_BME280", String(bme.readTemperature()).c_str());
  client.publish("home-assistant/temp/Barometric_Pressure_outside_BME280", String(bme.readPressure()/100).c_str());
//  delay(100);
  client.publish("home-assistant/temp/Humidity_outside_sht21", String(SHT21.getHumidity()).c_str());
  client.publish("home-assistant/temp/Temperature_outside_sht21", String(SHT21.getTemperature()).c_str());
  

  Serial.println("BMP280: ");
  Serial.print("Temperatur: ");
  Serial.print(bme.readTemperature());
  Serial.println(" °C");
  Serial.print("Luftdruck: ");
  Serial.print(bme.readPressure()/100);
  Serial.println(" mB");
 
  
  delay(1000);
  Serial.println("SHT21: ");
  Serial.print("Humidity(%RH): ");
  Serial.println(SHT21.getHumidity());
  Serial.print("     Temperature(C): ");
  Serial.println(SHT21.getTemperature());
  ESP.deepSleep(120e6);
//  delay(2000);
}




boolean connectWifi() {
  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  uint8_t i = 0;
  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while ((!WiFi.isConnected()) && (i < 30)) {
    Serial.print(".");
    delay(500);
    ++i;
  }
  if (WiFi.isConnected()) {
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("");
    Serial.println("");
    Serial.println("");
    return false;
  } else {
    Serial.println("WiFi not connected, try again");
    Serial.println("");
    Serial.println("");
    Serial.println("");
    return true;
  }
}

boolean connectMQTT() {
  Serial.println("Connecting MQTT ClientID: ");
  Serial.print("MQTT ClientID: ");
  Serial.println(ClientID);
  if(!client.connected()){
    Serial.println("Connecting...");
    client.setServer(mqtt_server, 1883);
    client.connect(ClientID, mqtt_user, mqtt_pass);
  }
  Serial.println("");
  Serial.println("");
  Serial.println("");
}
