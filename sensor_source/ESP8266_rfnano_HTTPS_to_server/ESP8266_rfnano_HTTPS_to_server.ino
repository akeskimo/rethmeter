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


const char* ssid = "";
const char* password = "";

//Domain name with URL path or IP address with path
const char* serverName = "https://";
const int httpsPort = 443;

//SHA1 from site certificate through web browser - copy/paste it AS IT IS!!!!!
const char* fingerprint = "";

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
unsigned int errorFlag = 0;

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
    return;  //
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
	
/* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
   would try to act as both a client and an access-point and could cause
   network-issues with your other WiFi-devices on your WiFi-network. */
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
      WiFiRestart(ssid, password);
  }

  // Print ESP8266 Local IP Address
  Serial.println();
  Serial.println("Wifi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //initialize the NTP client
  timeClient.begin();

} //setup()

//function for restarting WiFi------------------------------------
void WiFiRestart(const char* ssid_restart, const char* password_restart) {
  Serial.println("Turning WiFi off...");
  WiFi.mode(WIFI_OFF);
  Serial.println("Sleepping for 10 seconds...");
  delay(10000);
  Serial.println("Trying to reconnect...");
  WiFi.mode(WIFI_STA);
  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to WiFi
  WiFi.begin(ssid_restart, password_restart);
  Serial.println("Connecting to WiFi");

  // Print ESP8266 Local IP Address
  Serial.println();
  Serial.println("Wifi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
} //WifiRestart

//Function for checking if there is stored data on flash and transfering it to server using TransferData function----------------------------
unsigned long CheckStorage(const char* fprint,const char* sName,unsigned long t_time, const char* ssid_storage, const char* password_storage){
	  int Flag = 1; 
    t_time += PERIOD;  //get ready for the next iteration
    // this opens the file "meas.txt" in read-mode
    Serial.println("Opening file");
    File meas = SPIFFS.open("/meas.txt", "r");

    while (meas.available()) {
      //read line by line from the file
      String stored_json = meas.readStringUntil('\n');    //read until end of line

     Flag = TransferData(stored_json,fprint,sName, ssid_storage, password_storage);

    }//while data.available()

    Serial.println("Closing file");
    meas.close(); //close file after data transferred

    if (Flag == 0) {
      //delete local data buffer after succesful transfer
      Serial.println("Removing file");
      SPIFFS.remove("/meas.txt");
    }
  return t_time;
} //CheckStorageAndTransferData

//Function for transfering json data to server------------------------------------------------------
int TransferData(String json_out, const char* fprint2,const char* sName2, const char* ssid_transfer, const char* password_transfer){
	
	// wait for WiFi connection
    if (WiFi.status() == WL_CONNECTED) {
		
		//transfer data to server
		  // Create a WiFiClientSecure object
		  BearSSL::WiFiClientSecure client;
		  client.setFingerprint(fprint2);
		  // Initialize the client library HTTPClient
		  HTTPClient http;
		  http.begin(client, sName2);
		  http.addHeader("Content-Type", "application/json");

		  Serial.print("HTTPS POST data...\n");
		  int httpCode = http.POST(json_out);
		  if (httpCode > 0) {

			String response = http.getString();
			Serial.println(httpCode);
			Serial.println(response);
			http.end();
			return 0; //return no error
		  }
		  else {
			
			Serial.printf("Error occurred while sending HTTPS POST: %s\n", http.errorToString(httpCode).c_str());
			http.end();
			return 1; //return error
		  }
	}//if WiFi.status
	
	else {
      WiFiRestart(ssid_transfer, password_transfer);
	  return 1;
    }
} // TransferData

//Function for generating JSON --------------------------------------------------------------
String GenerateJSON(String str, String fDate){
  String json;
	// Split string for JSON building
    for (int i = 0; i < str.length(); i++) {
      if (str.substring(i, i + 1) == ",") {
        str_id = str.substring(0, i);
        str_location = str.substring(i + 1, i + 3);
        str_humid = str.substring(i + 3, i + 8);
        str_temp = str.substring(i + 8, i + 13);
        str_battery = str.substring(i + 13);
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

	//Generate JSON

    // Generate serialized JSON and send it to the Serial port.
    DynamicJsonDocument doc(1024);

    doc["Sensor"] = str_id;
    doc["Location"] = str_location;
    doc["Timestamp"] = fDate;
    doc["Temperature"] = str_temp.toFloat(); //temperature string to Float
    doc["Humidity"] = str_humid.toFloat(); //humidity string to Float
    doc["Battery"] = str_battery.toFloat(); //Battery voltage to Float

    serializeJson(doc, json); //json to string format
	
	return json;
	
} //GenerateJSON

//Function for saving JSON data to file---------------------------------------------------------
void SaveToSPIFFS(String json_SPIFFS){
	//Save data to SPIFFS memory to be sent when connection works

        if (SPIFFS.exists("/meas.txt")) { // in append mode to append to existing data file
          File meas = SPIFFS.open("/meas.txt", "a");
          if (meas.size() < 3000000) { //maximum file size set to 3MB
            int bytesWritten = meas.println(json_SPIFFS); //write json data to file
            if (bytesWritten > 0) {
      				Serial.print("Data saved to measurement file meas.txt - bytes written: ");
      				Serial.print(bytesWritten);
      				Serial.print(" bytes ");
      				Serial.print("Total size: ");
      				Serial.print(meas.size());
      				Serial.println(" bytes ");
      			}
			      else {
				      Serial.print("Error in writing data");
			      }
			    meas.close();
          }
          else {
            Serial.println("Maximum allowed measurement filesize reached!!!");
            meas.close();
          }
        }//if SPIFFS.exists
		
        else {  //in write mode for creating file
          File meas = SPIFFS.open("/meas.txt", "w");
          if (!meas) {
            Serial.println("file creation failed");
          }
          if (meas.size() < 3000000) { //maximum file size set to 3MB
            meas.println(json_SPIFFS); //write json data to file
            Serial.print("Data saved to measurement file meas.txt - File size: ");
            Serial.print(meas.size());
            Serial.println(" bytes");
            meas.close();
          }
          else {
            Serial.println("Maximum allowed measurement filesize reached!!!");
            meas.close();
          }
        } //else
      } //SaveToSPIFFS

//Main program loop------------------------------------------------------------
void loop() {
	
  // Check if there is data to be transfer in SPIFFS memory (4MB max) and WiFi is connected
  if (SPIFFS.exists("/meas.txt") && WiFi.status() == WL_CONNECTED && millis() - target_time >= PERIOD) {
		target_time = CheckStorage(fingerprint,serverName, target_time, ssid, password);  //Check if there is already stored data that needs to be transferred and delete if transferred successfully
	}//if SPIFFS
	
	//Read data from serial connection to Nano
	if (nano.available()) {
		//Read data from serial
		str_out = nano.readString();
		Serial.println("in nano loop");
		
		//Get time from NTP server ------------------------------------------
   timeClient.update();
//		while (!timeClient.update()) {
//		  Serial.println("Updating time");
//		  timeClient.forceUpdate(); //sometimes the NTP Client retrieves 1970. To ensure that doesn’t happen we need to force the update.
//		}
		// The formattedDate comes with the following format:
		// 2018-05-28T16:00:13Z
		formattedDate = timeClient.getFormattedDate(); //if this fails --> timecheck in serverside python code?
		
		//Generate JSON document from received data
		String json_i = GenerateJSON(str_out, formattedDate);
		
		//Send JSON document to server
		 int errorFlag = TransferData(json_i,fingerprint,serverName, ssid, password);
		
		if (errorFlag == 1) {
			SaveToSPIFFS(json_i);
		}
	} //if nano.available
}// loop()
