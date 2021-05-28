#include <Arduino.h>

#include <SPI.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
#include <DHT.h>
#define BLYNK_PRINT Serial


char auth[] ="----------------";               //Authentication code sent by Blynk
char ssid[] = "-------";                       //WiFi SSID
char pass[] = "-------";                       //WiFi Password
int Soil_Moisture_State = 0;
int lastState = 0;

#define Soil_Moisture_Pin D2 
#define DHTPIN D4   
#define Relay_Pin D0 
#define Pump_Pin D1
#define DHTTYPE DHT11     
DHT dht(DHTPIN, DHTTYPE);
SimpleTimer timer;

void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  Blynk.virtualWrite(V5, h);  //V5 is for Humidity
  Blynk.virtualWrite(V6, t);  //V6 is for Temperature
}

BLYNK_WRITE(V1)
{
  Serial.print("Got a value: ");
  Serial.println(param.asStr());
  digitalWrite(Pump_Pin,param.asInt());
}

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  pinMode(Soil_Moisture_Pin, INPUT);
  dht.begin();

  timer.setInterval(1000L, sendSensor);
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
}
int sensor=0;
// void sendTemps()
// {
//   Serial.println(temp);
//   Serial.println(sensor);
//   Blynk.virtualWrite(V1, temp);
//   Blynk.virtualWrite(V2,sensor);
//   delay(1000);
// }
void loop()
{
  Blynk.run(); 
  timer.run(); 
  sendSensor();
  Soil_Moisture_State = digitalRead(Soil_Moisture_Pin);
  Serial.println(Soil_Moisture_State);

if (Soil_Moisture_State == 1 && lastState == 0) {
  Serial.println("needs water, send notification");
  Blynk.notify("Water your plants");
  lastState = !lastState;
  delay(1000);
//send notification
    
  } 
  else if (Soil_Moisture_State == 1 && lastState == 1) {
    //do nothing, has not been watered yet
  Serial.println("has not been watered yet");
  Blynk.notify("Water your plants");
  delay(1000);
  }
  else {
    //st
    Serial.println("does not need water");
    Blynk.notify("does not need water");
    lastState = !lastState;
    delay(1000);
  }
  
  delay(100);
}