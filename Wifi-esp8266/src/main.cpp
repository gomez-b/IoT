/* Purpose: This project uses 8266 wifi features,
this program consists of turning on or off an LED 
using API
  Author: Brian Gomez Jimenez
   */
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>  // must be installed 
#include <WiFiClientSecure.h>
#include "sendRequest.h"
#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN 2  
// Uncomment the type of sensor in use:
#define DHTTYPE    DHT11     // DHT 11
DHT_Unified dht(DHTPIN, DHTTYPE);
//Function Declartions:

void database();
void time();

//Wifi Setup
const char* ssid     = "Telefono";
const char* password = "purpleturtle";

//This is for getting the time
String TimeURL = "https://timeapi.io/api/Time/current/zone?timeZone=America/Los_Angeles";

DynamicJsonDocument doc(1024);

//This is the URL parameters that will be used to push sensor data to website
String myURL     = "https://gomezb.online/data.php"; //this is the first part of the full URL to be sent
 String dataNode   = "?node_name=node_2&time_received="; // specifies that we refering to node_1 and sending temp (temp value not here currently)
//u_int8_t degrees = 0; //sets up tempValue as an unsigned 8 bit int
String dataHumi = "&humidity="; //specifies the next part of the URL that will refers to the humidity
String dataTemp  = "&temperature=";//2022-10-25T20:44:11.4055468"; //this is the final segment of the string // this will be swapped out with the actual time */
 
//Define button and sensors variable names
float temp;
int Button = D0;

//u_int8_t micValue = 0;
String mytime;
u_int8_t tempValue = 0; 
u_int8_t humValue = 0; 

void setup() {
  dht.begin();
  Serial.begin(9600); //set baud rate to use serial monitor
   sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
   dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  delay(10); // just a short delay
  Serial.println("");
 
  //(A) Start connecting to the WiFI
 Serial.println("Connecting to WiFi"); 
 WiFi.begin(ssid, password); // Connection to WiFi Starts until status()=WL_CONNECTED
  
  while (WiFi.status() != WL_CONNECTED) {
   delay(100);
   Serial.print("."); // waiting to get connected
 }

 // Details of the connection
  connectionDetails();
    //Setup for pins of sensors and button
   pinMode(Button, INPUT); //Sets pushButton(D0) as an input pin that will be used for switch

}

void loop() {

  //Read button and sensor values
  int  buttonState = digitalRead(Button);

  if(buttonState){
     sensors_event_t event;
      dht.temperature().getEvent(&event);
      if (isnan(event.temperature)) {
          Serial.println(F("Error reading temperature!"));
        }
      else {
          Serial.print(F("Temperature: "));
          Serial.print(event.temperature);
          Serial.println(F("°C"));
      // Convert Celsius to Fahrenheit
          float tempFahrenheit = (event.temperature * 9.0 / 5.0) + 32.0;
          Serial.print("Temperature in Fahrenheit: ");
          Serial.println(tempFahrenheit);

          tempValue = static_cast<u_int8_t>(tempFahrenheit); 
        }
  // Get humidity event and print its value.
      dht.humidity().getEvent(&event);
      if (isnan(event.relative_humidity)) {
        Serial.println(F("Error reading humidity!"));
      }
      else {
        Serial.print(F("Humidity: "));
        Serial.print(event.relative_humidity);
        //Serial.println(F("%"));
        humValue = static_cast<u_int8_t>(event.relative_humidity);
      }
        Serial.println("Button is pressed!");//for testing 

      time(); //This is the function is to GET time from timeapi.io */
      database(); //This is the function to push the data with timestamp to my database

      delay(500); //25 second delay between REQUEST to servers
    
    Serial.println("Data posted to GRAFANA!");
  }
}

 void time(){
  if (WiFi.status() == WL_CONNECTED){
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient https;

    String fullUrl = TimeURL; // preparing the full URL
    Serial.println("Requesting: --> " + fullUrl);

    if (https.begin(client, fullUrl)) { // start the connection 1=started / 0=failed
      
      int httpCode = https.GET(); // choose GET or POST method
      //int httpCode = https.POST(fullUrl); // need to include URL
     
      Serial.println("Response code <--: " + String(httpCode)); // print response code: e.g.,:200
      
      Serial.println(https.getString());// for testing purposes

      if (httpCode > 0) {
        Serial.println(https.getString()); // this is the content of the get request received
        deserializeJson(doc,https.getString()); // deserialize the JSON file
        /*--- Sample Response ----
        {"year":2022,"month":10,"day":25,"hour":20,"minute":44,"seconds":11,"milliSeconds":405,
        "dateTime":"2022-10-25T20:44:11.4055468","date":"10/25/2022","time":"20:44",
        "timeZone":"America/Los_Angeles","dayOfWeek":"Tuesday","dstActive":true}
        ------------------------ */
        deserializeJson(doc,https.getString()); // deserialize the JSON format into keys and values
        String temp = doc["dateTime"]; // get the value associated with the dataTime key         //i added the second equal sign when forcing the string declaration to global
        mytime = temp;
        Serial.println(temp); // soomething like 2022-10-25T21:03:44.1790514
      }

      https.end(); // end of request
    
    } else {
        Serial.printf("[HTTPS] Unable to connect\n");
    } 
  }
 }

void database(){
  if (WiFi.status() == WL_CONNECTED){
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient https;

    //String fullUrl = myURL + dataNodetime + mytime+ dataTemp+ tempValue+ dataHumi + humValue  ; // preparing the full URL
     String fullUrl = myURL + dataNode + mytime +dataTemp+ tempValue+ dataHumi + humValue  ; // preparing the full URL

    Serial.println(fullUrl);
    //String myemail = emailURL;
   // Serial.println("Requesting: --> " + myURL);
    Serial.println("Requesting: -->"+ fullUrl);
    if (https.begin(client, fullUrl)) { // start the connection 1=started / 0=failed
      int httpCode = https.GET(); //GET METHOD

      
      Serial.println("Response code <--: " + String(httpCode)); // print response code: e.g.,:200
      if (httpCode > 0) {
        deserializeJson(doc,https.getString()); // deserialize the JSON file
        Serial.println(https.getString()); // this is the content of the get request received
      }
    https.end(); // end of request
    }else{
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }
}