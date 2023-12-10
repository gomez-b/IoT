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

//Function Declartions:
//void time();
void database();

//Wifi Setup
const char* ssid     = "Telefono";
const char* password = "purpleturtle";

//This is for getting the time
//String TimeURL = "https://timeapi.io/api/Time/current/zone?timeZone=America/Los_Angeles";
DynamicJsonDocument doc(1024);

//This is the URL parameters that will be used to push sensor data to website
// String myURL     = "https://gomez-jimenez-brian.000webhostapp.com/PHPCRUD/data.php"; //this is the first part of the full URL to be sent
//String emailURL = "https://maker.ifttt.com/trigger/button_pressed/json/with/key/c6p3kXDiJ6xWzaP8tXOZkx";
//String myURL = "https://gomez-jimenez-brian.000webhostapp.com/results.txt"; 
/* String dataNodetime   = "?node_name=node_2&time_received="; // specifies that we refering to node_1 and sending temp (temp value not here currently)
//u_int8_t degrees = 0; //sets up tempValue as an unsigned 8 bit int
String dataHumi = "&humidity="; //specifies the next part of the URL that will refers to the humidity
u_int8_t flameValue = 0; //sets up humiValue as an unsigned 8 bit int
String dataTemp  = "&temperature=";//2022-10-25T20:44:11.4055468"; //this is the final segment of the string // this will be swapped out with the actual time */
String myURL     = "https://gomezb.online/data.php"; //this is the first part of the full URL to be sent
//Define button and sensors variable names
//float temp;
int Button = D0;
const int tempSensor = A0;
int flameSensor = D1;
u_int8_t tempValue = 0;
String mytime;

void setup() {
  Serial.begin(9600); //set baud rate to use serial monitor
  delay(10); // just a short delay
  Serial.println("");
  pinMode(BUILTIN_LED,OUTPUT);
  // (A) Start connecting to the WiFI
  Serial.println("Connecting to WiFi"); 
  WiFi.begin(ssid, password); // Connection to WiFi Starts until status()=WL_CONNECTED
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print("."); // waiting to get connected
  }

  // Details of the connection
  connectionDetails();

  //Setup for pins of sensors and button
  pinMode(Button, INPUT_PULLUP); //Sets pushButton(D0) as an input pin that will be used for switch
/*   pinMode(tempSensor, INPUT); //Set tempSensor(A0) as an input pin for analog temp sensor
  pinMode(flameSensor, INPUT); //Set flameSensor(D1) as an input pin for digital flame sensor */
}

void loop() {

  //Read button and sensor values
  int buttonState = digitalRead(Button);
/*   flameValue = digitalRead(flameSensor);
  int sensorVal = analogRead(tempSensor); */

/*   float mv = (sensorVal/1024.0)*3300;
  float cel = mv/100;
  float temperature = (cel*1.8)+40;
  tempValue = temperature; */
  //Serial.println(degrees);
  if(buttonState){
     Serial.println("Button is pressed!");//for testing 

/*     Serial.print("Temp Value: "); Serial.println(tempValue);
    Serial.print("Flame Value: "); Serial.println(flameValue);
    time(); //This is the function is to GET time from timeapi.io */
    database(); //This is the function to push the data with timestamp to my database

    delay(25000); //25 second delay between REQUEST to servers
    //Serial.println("DATA WAS POSTED!");
    Serial.println("Updated Status of LED!");
  }
}

/* void time(){
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
/*         deserializeJson(doc,https.getString()); // deserialize the JSON format into keys and values
        String temp = doc["dateTime"]; // get the value associated with the dataTime key         //i added the second equal sign when forcing the string declaration to global
        mytime = temp;
        Serial.println(temp); // soomething like 2022-10-25T21:03:44.1790514
      }

      https.end(); // end of request
    
    } else {
        Serial.printf("[HTTPS] Unable to connect\n");
    } */

void database(){
  if (WiFi.status() == WL_CONNECTED){
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient https;

    //String fullUrl = myURL + dataNodetime + mytime+ dataTemp+ tempValue+ dataHumi + flameValue  ; // preparing the full URL
    //String myemail = emailURL;
   // Serial.println("Requesting: --> " + myURL);
    Serial.println("Requesting: -->"+ myURL);
    if (https.begin(client, myURL)) { // start the connection 1=started / 0=failed
      int httpCode = https.GET(); //GET METHOD

      
      Serial.println("Response code <--: " + String(httpCode)); // print response code: e.g.,:200
      if (httpCode > 0) {
        Serial.println(https.getString()); // this is the content of the get request received
        deserializeJson(doc,https.getString()); // deserialize the JSON file
        //Serial.println(https.getString()); // this is the content of the get request received
        if (https.getString()=="on"){
          digitalWrite(BUILTIN_LED,LOW);//on
          Serial.println("LED is ON!");
        }
        else if (https.getString()=="off")
        {
          digitalWrite(BUILTIN_LED,HIGH);//off
          Serial.println("LED is OFF!");
        }
        else {
          Serial.println("Do Nothing");
        } 

      }
    https.end(); // end of request
    }else{
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }
}
