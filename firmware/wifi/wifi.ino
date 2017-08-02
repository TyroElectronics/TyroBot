/*TyroBoard WiFi Module Code
  copyright (c) 2017 Tyro Electronics
  written by Tyler Spadgenske
  Licensed under GPL 3.0
*/

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

ESP8266WiFiMulti WiFiMulti;

int data = -1; //the magic number

int ind1;
String ssidStr;
String passStr;
String keyStr;
String eventStr;

//IFTT
String value1, value2, value3 = "";
bool dataAvailable;
String postData;
WiFiClient client;

void enterBootloader() {
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  delay(10);
  pinMode(4, INPUT);
  delay(10);
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  delay(10);
  pinMode(4, INPUT);
}

void setup() {
  pinMode(0, OUTPUT);
  digitalWrite(0, 1);
  Serial.begin(9600);
}

void loop() {
  if (Serial.available()) {
      delay(100); //allows all serial sent to be received together
      char data = Serial.read();
      //Serial.println(data);
      if (data == 0) {
        enterBootloader();
        data = -1;
      }
      //get SSID from host
      if (data == 1) {
        delay(100);
        data = -1;
        String credentials = Serial.readString();
        ind1 = credentials.indexOf(',');  
        ssidStr = credentials.substring(0, ind1);   
        passStr = credentials.substring(ind1 + 1);
        char ssid[50];
        char pass[50];
        ssidStr.toCharArray(ssid, 50);
        passStr.toCharArray(pass, 50);
        WiFiMulti.addAP(ssid, pass); // connect to network
        //check for connection
        if((WiFiMulti.run() == WL_CONNECTED)) {
          long rssiInt = WiFi.RSSI();
          String rssiStr = String(rssiInt);
          char rssi[4];
          rssiStr.toCharArray(rssi, 4);
          
          Serial.write(rssi);
          Serial.write(",");
          IPAddress ipStr = WiFi.localIP();
          String ipString = String(ipStr[0]);
          for (byte octet = 1; octet < 4; ++octet) {
            ipString += '.' + String(ipStr[octet]);
          }
          char ip[20];
          ipString.toCharArray(ip, 20);
          Serial.write(ip);
        }
        else {
          Serial.write("Not connected.");
          digitalWrite(0, 0);
        }
      }
      //IFTT mode
     if (data == 2) {
      delay(100);
      data = -1;
      String IFTTdata = Serial.readString();
      ind1 = IFTTdata.indexOf(',');  
      keyStr = IFTTdata.substring(0, ind1);   
      eventStr = IFTTdata.substring(ind1 + 1);
      Serial.println(keyStr);
      Serial.println(eventStr);
      if (connectIFTT()) {
        post(keyStr,eventStr);
      }
     }
  }

if((WiFiMulti.run() == WL_CONNECTED)) {
     
     } else {
        //Serial.println("Not connected");
        delay(1000);
     }
}

bool connectIFTT()
{
  if (client.connect("maker.ifttt.com", 80))
    return true;
  else return false;
}

void post(String privateKey, String event)
{
  compileData();
  client.print("POST /trigger/");
  client.print(event);
  client.print("/with/key/");
  client.print(privateKey);
  client.println(" HTTP/1.1");

  client.println("Host: maker.ifttt.com");
  client.println("User-Agent: Arduino/1.0");
  client.println("Connection: close");
  if (dataAvailable)
  { // append json values if available
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(postData.length());
    client.println();
    client.println(postData);
  }
  else
    client.println();
}

bool setValue(int valueToSet, String value)
{
  switch (valueToSet)
  {
    case 1:
      value1 = value;
      break;
    case 2:
      value2 = value;
      break;
    case 3:
      value3 = value;
      break;
    default:
      return false;
      break;
  }
  return true;
}

void compileData()
{
  if (value1 != "" || value2 != "" || value3 != "")
  {
    dataAvailable = true;
    bool valueEntered = false;
    postData = "{";
    if (value1 != "")
    {
      postData.concat("\"value1\":\"");
      postData.concat(value1);
      valueEntered = true;
    }
    if (value2 != "")
    {
      if (valueEntered)postData.concat("\",");
      postData.concat("\"value2\":\"");
      postData.concat(value2);
      valueEntered = true;
    }
    if (value3 != "")
    {
      if (valueEntered)postData.concat("\",");
      postData.concat("\"value3\":\"");
      postData.concat(value3);
    }
    postData.concat("\"}");
  }
  else dataAvailable = false;
}
