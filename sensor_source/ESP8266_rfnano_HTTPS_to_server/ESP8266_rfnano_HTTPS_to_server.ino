// Library for serial communication
#include <SoftwareSerial.h>
//library for Json
#include <ArduinoJson.h>
//Library for WiFi
#include <ESP8266WiFi.h>
//Library for secure wifi connection
#include <WiFiClientSecure.h>
//Library for UDP packets
#include <WiFiUdp.h>
//Library for HTTPS POST
#include <ESP8266HTTPClient.h>
//Library for date and time
#include <NTPClient.h>
//Library for using SPIFFS to save data buffer
#include "FS.h"

//Network setup------------------------------

//network credentials for wifi
const char* ssid = ""; 
const char* password = "";

//Domain name with URL path or IP address with path (postgresql in port 5432)
const char* serverName = "https://192.168.0.100";
const int httpsPort = 443;

//SHA1 from site certificate through web browser - copy/paste it AS IT IS!!!!!
const char* fingerprint = "FC:CC:DE:3C:60:A6:5C:50:10:F6:00:41:A8:BB:31:50:95:95:E7:72";

//Pin definitions on nodeMCU -----------------------------

//#define LED D1 // Led in NodeMCU at pin GPIO16 (D0).
#define D0 16
#define D1 5 // I2C Bus SCL (clock)
#define D2 4 // I2C Bus SDA (data)
#define D3 0
#define D4 2 // Same as "LED_BUILTIN", but inverted logic
#define D5 14 // SPI Bus SCK (clock)
#define D6 12 // SPI Bus MISO 
#define D7 13 // SPI Bus MOSI
#define D8 15 // SPI Bus SS (CS)
#define D9 3 // RX0 (Serial console)
#define D10 1 // TX0 (Serial console)

//Setup for serial communication --------------------------
// cpu frequency
#define FREQUENCY    160
#define PERIOD (60*1000L)  //how often nodemcu tries to save internally stored data to rethmeter server - once in a minute

SoftwareSerial nano(D1, D2); // (Rx, Tx)

//Variable definitions -----------------------------
String str_id;
String str_location;
String str_humid;
String str_temp;
String str_battery;
String str_out;
String formattedDate;

unsigned long target_time = 0L; //for keeping https send loop for stored data constant in respect of internal clock

const long utcOffsetInSeconds = 7200; //ntpclient
// Set offset time in seconds to adjust for your timezone, for example:
// GMT +1 = 3600
// GMT +8 = 28800
// GMT -1 = -3600
// GMT 0 = 0


// Define NTP Client to get time ----------------------------
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);


void setup()
{

  // Setup Serial Monitor----------------------------------
  Serial.begin(115200);
  while (!Serial) continue;  // wait for serial port to connect. Needed for native USB port only

  //SPIFFS setup------------------------------
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;  //<----onko fiksua
  }

  //  //SPIFFS format COMMENT THESE LINES has to be done ONLY ONCE!!!
  //  Serial.println("Please wait 30 secs for SPIFFS to be formatted");
  //  SPIFFS.format();
  //  Serial.println("Spiffs formatted");


  //Setup RX for receiving serial data from nano board ----------------------------
  nano.begin(115200);
  //  // Setup serial read


  //setup WiFi ------------------------------------------
  delay(10);
  WiFi.mode(WIFI_STA);

  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to WiFi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  unsigned short count = 0;


  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
    count++;

    if (count >= 30)
      WiFiRestart();
  }

  // Print ESP8266 Local IP Address
  Serial.println();
  Serial.println("Wifi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //initialize the NTP client
  timeClient.begin();

  //---------------------for development!!!!!!!!!!!!!!!!!!!!!!!!!!
  if (SPIFFS.exists("/meas.txt")) {
    //File meas = SPIFFS.open("/meas.txt", "r");
    SPIFFS.remove("/meas.txt");
  }

} //void setup()
//------------------------------------
void WiFiRestart() {
  Serial.println("Turning WiFi off...");
  WiFi.mode(WIFI_OFF);
  Serial.println("Sleepping for 10 seconds...");
  delay(10000);
  Serial.println("Trying to reconnect...");
  WiFi.mode(WIFI_STA);
}


void loop() {
  unsigned int errorFlag = 0;
  //Check if there is already stored data that needs to be transferred and delete if transferred successfully--------------------------
  // Check if there is data to be transfer in SPIFFS memory (4MB max) and WiFi is connected
  if (SPIFFS.exists("/meas.txt") && WiFi.status() == WL_CONNECTED && millis() - target_time >= PERIOD) {
    target_time += PERIOD;  //get ready for the next iteration
    // this opens the file "meas.txt" in read-mode
    File meas = SPIFFS.open("/meas.txt", "r");

    while (meas.available()) {
      //read line by line from the file
      String stored_json = meas.readStringUntil('\n');    //read until end of line

      //transfer data to server
      // Create a WiFiClientSecure object
      BearSSL::WiFiClientSecure client;
      client.setFingerprint(fingerprint);
      // Initialize the client library HTTPClient
      HTTPClient http;
      http.begin(client, serverName);
      http.addHeader("Content-Type", "application/json");

      Serial.print("HTTPS POST saved data...\n");
      int httpCode = http.POST(stored_json);
      if (httpCode > 0) {

        String response = http.getString();
        Serial.println(httpCode);
        Serial.println(response);

      }
      else {
        errorFlag = 1;
        Serial.printf("Error occurred while sending HTTPS POST: %s\n", http.errorToString(httpCode).c_str());
        http.end();
        break;
      }

    }//while data.available()

    Serial.println("Closing file");
    meas.close(); //close file after data transferred

    if (errorFlag == 0) {
      //delete local data buffer after succesful transfer
      Serial.println("Removing file");
      SPIFFS.remove("/meas.txt");
    }

  }//if SPIFFS

  //Read data from serial port  -----------------------------------------
  if (nano.available()) {
    //Read data from serial
    str_out = nano.readString();
    Serial.println("in nano loop");
    // Split string for debugging and development purposes
    for (int i = 0; i < str_out.length(); i++) {
      if (str_out.substring(i, i + 1) == ",") {
        str_id = str_out.substring(0, i);
        str_location = str_out.substring(i + 1, i + 3);
        str_humid = str_out.substring(i + 3, i + 8);
        str_temp = str_out.substring(i + 8, i + 13);
        str_battery = str_out.substring(i + 13);
        break;
      }
    }

    // Print values to Serial Monitor
    Serial.print("ID: ");
    Serial.print(str_id);
    Serial.print("  - Location_ID: " );
    Serial.print(str_location);
    Serial.print("  - Humidity: ");
    Serial.print(str_humid);
    Serial.print("  - Temperature: ");
    Serial.print(str_temp);
    Serial.print("  - battery: ");
    Serial.println(str_battery);

    //Get time from NTP server ------------------------------------------
    while (!timeClient.update()) {
      Serial.print("getting time");
      timeClient.forceUpdate(); //sometimes the NTP Client retrieves 1970. To ensure that doesn’t happen we need to force the update.
    }
    // The formattedDate comes with the following format:
    // 2018-05-28T16:00:13Z
    formattedDate = timeClient.getFormattedDate(); //if this fails --> timecheck in serverside python code?

    //Serial.println(formattedDate);

    //Generate JSON ---------------------------------------------------

    // Generate serialized JSON and send it to the Serial port.
    DynamicJsonDocument doc(1024);

    doc["Sensor"] = str_id;
    doc["Location"] = str_location;
    doc["Timestamp"] = formattedDate;
    doc["Temperature"] = str_temp.toFloat(); //temperature string to Float
    doc["Humidity"] = str_humid.toFloat(); //humidity string to Float
    doc["Battery"] = str_battery.toFloat(); //Battery voltage to Float

    serializeJson(doc, Serial); //print output for debugging purposes


    //HTTPS POST to send data to rethmeter webserver ------------------------------------------

    // Start a new line
    Serial.println();
    Serial.println(fingerprint); //fingerprint

    // wait for WiFi connection
    if (WiFi.status() == WL_CONNECTED) {
      // Create a WiFiClientSecure object
      BearSSL::WiFiClientSecure client;

      //Allow self signed certificates
      //client.allowSelfSignedCerts();
      // Set the fingerprint/CA cert to connect the server.
      //client.setInsecure();
      client.setFingerprint(fingerprint);

      // Initialize the client library HTTPClient
      HTTPClient http;
      http.writeToStream(&Serial);
      Serial.println("HTTPS begin");
      http.begin(client, serverName);
      http.addHeader("Content-Type", "application/json");

      //Serialize document
      String json;
      serializeJson(doc, json);
      Serial.print("HTTPS POST...\n");
      int httpCode = http.POST(json);
      if (httpCode > 0) {

        String response = http.getString();
        Serial.println(httpCode);
        Serial.println(response);
        return;

      }
      else {

        Serial.printf("Error occurred while sending HTTPS POST: %s\n", http.errorToString(httpCode).c_str());

        //Save data to SPIFFS memory to be sent when connection works

        if (SPIFFS.exists("/meas.txt")) { // in append mode to append to existing data file
          File meas = SPIFFS.open("/meas.txt", "FILE_APPEND");
          if (meas.size() < 3000000) { //maximum file size set to 3MB
            meas.println(json); //write json data to file
            Serial.print("Data saved to measurement file meas.txt - File size: ");
            Serial.print(meas.size());
            Serial.println(" bytes");
            meas.close();
          }
          else {
            Serial.println("Maximum allowed measurement filesize reached!!!");
            meas.close();
          }
        }
        else {  //in write mode for creating file
          File meas = SPIFFS.open("/meas.txt", "w");
          if (!meas) {
            Serial.println("file creation failed");
          }
          if (meas.size() < 3000000) { //maximum file size set to 3MB
            meas.println(json); //write json data to file
            Serial.print("Data saved to measurement file meas.txt - File size: ");
            Serial.print(meas.size());
            Serial.println(" bytes");
            meas.close();
          }
          else {
            Serial.println("Maximum allowed measurement filesize reached!!!");
            meas.close();
          }
        }
      }

      http.end();
      return; //Back to monitoring serial for more data

    }
    else {
      WiFiRestart();
    }
  } // if (nano.available())
}// void loop()
