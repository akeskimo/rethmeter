// Include RadioHead Amplitude Shift Keying Library
#include <RH_ASK.h>
// Include dependant SPI Library
#include <SPI.h>

#include <SoftwareSerial.h>

#include <DHT.h>

#define DHTPIN 2     // Digital pin D2 connected to the DHT sensor
// Sensor type:
#define DHTTYPE    DHT22     // DHT 22 (AM2302)

#define PERIOD (104*1000L)  //how often internal DHT22 sensor is read - 104seconds is the time that rf sensors sleep 13*8seconds

DHT dht(DHTPIN, DHTTYPE);

// Create Amplitude Shift Keying Object
RH_ASK rf_driver(2000, 12);

SoftwareSerial espSerial(5, 6); // RX(receive data), TX (transmit data)

// Define output strings for readings from rf sensors
String str_id;
String str_location;
String str_humid;
String str_temp;
String str_out;
String str_battery;

unsigned long target_time = 0L; //for keeping internal DHT22 read loop constant in respect of internal clock

//sensor id for connected sensor
String id_int = "01";
//sensor location
String location_int = "01";
// current temperature & humidity, updated in loop()
float h = 0.0;
float t = 0.0;
String str_int_humid;
String str_int_temp;
String str_int_out;
String str_int_battery = "5.00";

void setup() {

  // Initialize ASK Object
  if (!rf_driver.init())
    Serial.println("Init failed");

  // Setup Serial Monitor
  Serial.begin(115200);

  //setup DHT
  dht.begin();
  
  //Serial.begin(115200);
  espSerial.begin(115200);

}

void loop() {

  // Set buffer to size of expected message
  uint8_t buf[19];
  uint8_t buflen = sizeof(buf);
  // Check if received packet is correct size
  if (rf_driver.recv(buf, &buflen))
  {
    //int i;
    // Message received with valid checksum
    // Get values from string

    // Convert received data into string
    str_out = String((char*)buf);
    // Split string into two values
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

    //send received string to nodeMCU as it is
    espSerial.print(str_out);

    // Print values to Serial Monitor for development purposes and debugging
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

  }

  if (millis() - target_time >= PERIOD) //read data from internal DHT22 sensor every 104seconds

    //read values from DHT22
  {
    target_time += PERIOD;  //get ready for the next iteration

    float h = dht.readHumidity();
    float t = dht.readTemperature();
    Serial.print(t);
    Serial.print(" ");
    Serial.println(h);
    
    // Indication if read data fails
    if (isnan(h) || isnan(t))
    {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    // Convert Humidity to string
    str_int_humid = String(h);

    // Convert Temperature to string
    str_int_temp = String(t);

    // Combine ID, location, Humidity and Temperature
    str_int_out = id_int + "," + location_int + str_int_humid + str_int_temp + str_int_battery;

    //send received string to nodeMCU
    espSerial.print(str_int_out);

    Serial.println(str_int_out);
  }
}
