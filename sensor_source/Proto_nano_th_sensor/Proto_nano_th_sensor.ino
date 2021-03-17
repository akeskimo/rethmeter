#include <DHT.h>
//DeepSleep
#include "LowPower.h"
// Include RadioHead Amplitude Shift Keying Library
#include <RH_ASK.h>
// Include dependant SPI Library
#include <SPI.h>

#define DHTPIN 3     // Digital pin D3 connected to the DHT sensor

// Sensor type:
#define DHTTYPE    DHT22     // DHT 22 (AM2302)

DHT dht(DHTPIN, DHTTYPE);

// Create Amplitude Shift Keying Object
RH_ASK rf_driver;

//sensor id
String id = "01";
//sensor location
String location = "01";
// current temperature & humidity, updated in loop()
float h = 0.0;
float t = 0.0;
String str_humid;
String str_temp;
String str_battery;
String str_out;

int battPin = A1; //analog pin for reading battery voltage

void setup() {
  //powersaving features: disable a/d conversion
  //  power_adc_disable();

  Serial.begin(9600);
  pinMode(battPin, INPUT); //pin for analog voltage input
  dht.begin();

  // Initialize ASK Object
  if (!rf_driver.init())
  {
    Serial.println("Init failed");
  }
    analogReference(INTERNAL); //Use internal 1.1v voltage as reference voltage for analog pins to be compared with battery voltage
}

void loop() {
  //read battery voltage
  int voltReading = analogRead(battPin);
  float volts = (voltReading / 204.6); //float voltage= sensorValue * (5.0 / 1023.0);
  
  //read values from DHT22
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Indication if read data fails
  if (isnan(h) || isnan(t))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  //Print temp and humidity values to serial monitor
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %, Temp: ");
  Serial.print(t);
  Serial.print(" Celsius");
  Serial.print(" , Battery: ");
  Serial.println(volts);

  // Convert Humidity to string
  str_humid = String(h);

  // Convert Temperature to string
  str_temp = String(t);

 //convert voltage to string
 str_battery = String(volts);
  // Combine Humidity and Temperature
  str_out = id + "," + location + str_humid + str_temp + str_battery;

  // Compose output character
  static char *msg = str_out.c_str();
  rf_driver.send((uint8_t *)msg, strlen(msg));
  rf_driver.waitPacketSent();

  Serial.println(str_out);

  //low power wait by changing the clockspeed
  //  CLKPR = 0x80;
  //
  //  CLKPR = 0x01;
  //sleep for 104 seconds - reality can vary depending on what happens after every 8s
  for (int i = 0; i < 13; i++) {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }
}
