#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Define I2C address and LCD size
String I2C_ADDR = "0x27";
const int LCD_COLS = 16;
const int LCD_ROWS = 2;

LiquidCrystal_I2C lcd(0x27, 16, 2);
ESP8266WiFiMulti WiFiMulti;

// Define DHT22 pin and type
const int DHTPIN = 2;// D4
String DHTTYPE = "DHT22";
DHT dht(2, DHT22);

// Define MQ135 pin
const int MQ135PIN = 17;// A0

// String pushingBoxAPI = "http://api.pushingbox.com/pushingbox?devid=v192809FEEA0DEA7&";

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);

  // Initialize the LCD
  lcd.init();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 0); // first row
  lcd.print("AIR QUALITY");
  lcd.setCursor(5, 1); // second row
  lcd.print("MONITOR");
  delay(2000);

  // Initialize DHT sensor
  dht.begin();

  // Connecting to Access Point
  lcd.clear();
  lcd.setCursor(5, 0); // first row
  lcd.print("WIFI");
  lcd.setCursor(0, 1); // second row
  lcd.print("CONNECTING...");
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("Verynice Galaxy 31099", "Lahempo12");
  while((WiFiMulti.run() != WL_CONNECTED)){
    delay(1000);
  }
  lcd.clear();
  lcd.setCursor(5, 0); // first row
  lcd.print("WIFI");
  lcd.setCursor(2, 1); // second row
  lcd.print("CONNECTED");
  delay(2000);
}

void loop() {
  // Read temperature and humidity from DHT22
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Check if any reads failed and exit early (to try again)
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Read gas level from MQ135
  int gasLevel = analogRead(MQ135PIN);

  // Setting air quality
  String gasStatus;

  if(gasLevel >= 0 && gasLevel <= 50)
   {
    gasStatus = "GOOD";
   }
    else if(gasLevel >= 51 && gasLevel <= 100)
   {
    gasStatus = "MODERATE";
   }
   else if(gasLevel >= 101 && gasLevel <= 150)
   {
    gasStatus = "POOR";
   }
   else if(gasLevel >= 151 && gasLevel <= 200)
   {
    gasStatus = "UNHEALTHY";
   }
   else if(gasLevel >= 201 && gasLevel <= 300)
   {
    gasStatus = "MORBID";
   }
   else if(gasLevel >= 301 && gasLevel <= 500)
   {
    gasStatus = "HAZARDOUS";
   }
   else
   {
    gasStatus = "TOXIC";
   }


  String tempStatus;
  if(temperature >= 22 && temperature <= 27)
  {
    tempStatus = "Comfortable";
  }
  else
  {
    tempStatus = "Uncomfortable";
  }

  String humidStatus;
  if(humidity >= 40 && humidity <= 60)
  {
    humidStatus = "Comfortable";
  }
  else
  {
    humidStatus = "Uncomfortable";
  }

  // Print data to Serial for debugging
  Serial.println("-------------------------");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" *C ");
  Serial.println(tempStatus);
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print("% ");
  Serial.println(humidStatus);
  Serial.print("Air Quality: ");
  Serial.print(gasLevel);
  Serial.print("ppm  ");
  Serial.println(gasStatus);
  Serial.println("-------------------------");

  lcd.clear(); // Clear the LCD for next display

  // Display temperature data on LCD
  lcd.setCursor(0, 0); // first row
  lcd.print("Tempera: ");
  lcd.print(temperature);
  lcd.print(" C ");
  lcd.setCursor(0, 1); // second row
  lcd.print(tempStatus);

  // sendDataToGoogleSheets(temperature, tempStatus, humidity, humidStatus, gasLevel, gasStatus);
  sendDataToServer(temperature, tempStatus, humidity, humidStatus, gasLevel, gasStatus);
  
  delay(3000); // Wait a few seconds between measurements

  lcd.clear(); // Clear the LCD for next display

  // Display humidity data on LCD
  lcd.setCursor(0, 0); // first row
  lcd.print("Humid:");
  lcd.print(humidity);
  lcd.print(" % ");
  lcd.setCursor(0, 1); // second row
  lcd.print(humidStatus);

  delay(3000); // Wait a few seconds between measurements

  lcd.clear(); // Clear the LCD for next display

  lcd.setCursor(0, 0); // first row
  lcd.print("AIR QUALITY: ");
  lcd.print(gasLevel);
  lcd.setCursor(0, 1); // second rowt
  lcd.print(gasStatus);

  delay(3000); // Wait a few seconds before repeating
}

// void sendDataToGoogleSheets(float temperature, String temperatureStatus, float humidity, String humidityStatus, int gasLevel, String gasLevelStatus){
//   // wait for WiFi connection
//   if ((WiFiMulti.run() == WL_CONNECTED)) {
//     WiFiClient client;
//     HTTPClient http;
//     String url = pushingBoxAPI + "temperature=" + (String)temperature + "&temperatureStatus=" + (String)temperatureStatus + "&humidity=" + (String)humidity + "&humidityStatus=" + (String)humidityStatus + "&gasLevel=" + (String)gasLevel + "&gasLevelStatus=" + (String)gasLevelStatus;

//     Serial.print("[HTTP] begin...\n");
//     if (http.begin(client, url)) {  // HTTP


//       Serial.println("[HTTP] GET..." + (String)url);
//       // start connection and send HTTP header
//       int httpCode = http.GET();

//       // httpCode will be negative on error
//       if (httpCode > 0) {
//         // HTTP header has been send and Server response header has been handled
//         Serial.printf("[HTTP] GET... code: %d\n", httpCode);
        
//         // file found at server
//         if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
//           String payload = http.getString();
//           Serial.println(payload);
//         }
//       } else {
//         Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
//       }

//       http.end();
//     } else {
//       Serial.println("[HTTP] Unable to connect");
//     }
//   }
//   delay(1000);
// }

void sendDataToServer(float temperature, String temperatureStatus, float humidity, String humidityStatus, int gasLevel, String gasLevelStatus) {
  if (WiFiMulti.run() == WL_CONNECTED) {
    WiFiClient client;
    client.setInsecure(); // disable SSL certificate
    
    HTTPClient http;
    String url = "https://air-quality-monitor-two.vercel.app/update";

    String payload = "{";
    payload += "\"temperature\":" + String(temperature) + ",";
    payload += "\"temperatureStatus\":\"" + temperatureStatus + "\",";
    payload += "\"humidity\":" + String(humidity) + ",";
    payload += "\"humidityStatus\":\"" + humidityStatus + "\",";
    payload += "\"gasLevel\":" + String(gasLevel) + ",";
    payload += "\"gasLevelStatus\":\"" + gasLevelStatus + "\"";
    payload += "}";

    if (http.begin(client, url)) {
      http.addHeader("Content-Type", "application/json");
      int httpCode = http.POST(payload);

      if (httpCode > 0) {
        Serial.printf("[HTTP] POST... code: %d\n", httpCode);
      } else {
        Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.println("[HTTP] Unable to connect");
    }
  }
  delay(1000);
}

