#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <HX711_ADC.h>
#include <Firebase_ESP_Client.h>
// Set the LCD address and dimensions
#define LCD_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"


#define WIFI_SSID "Meralco"
#define WIFI_PASSWORD "Amistacompany"


// Insert Firebase project API Key
#define API_KEY "AIzaSyB46bALbbnAAY5pgtuk8BQAvc-_Lnhy3qg"
#define DATABASE_URL "https://auth-7dd0a-default-rtdb.firebaseio.com/"

FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;


unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

// Initialize the LCD
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

// Pins for MQ2
const int gasSensorPin = 12;

// Pins for HX711
const int HX711_dout = 15;
const int HX711_sck = 5;
HX711_ADC LoadCell(HX711_dout, HX711_sck);

const int calVal_calVal_eepromAdress = 0;
unsigned long t = 0;

void setup() {
  Serial.begin(57600);
  delay(10);
  Serial.println();
  Serial.println("Starting...");

  // Initialize the LCD
  lcd.init();
  lcd.backlight();

  // Set up the gas sensor pin
  pinMode(gasSensorPin, INPUT);

  // HX711 setup
  float calibrationValue = 9.58;  // Change this value if needed9.57
  LoadCell.begin();
  unsigned long stabilizingtime = 5000;
  boolean _tare = true;
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
  } else {
    LoadCell.setCalFactor(calibrationValue);
    Serial.println("Startup is complete");
  }
  while (!LoadCell.update())
    ;

  Serial.print("Calibration value: ");
  Serial.println(LoadCell.getCalFactor());
  Serial.print("HX711 measured conversion time ms: ");
  Serial.println(LoadCell.getConversionTime());
  Serial.print("HX711 measured sampling rate HZ: ");
  Serial.println(LoadCell.getSPS());
  Serial.print("HX711 measured settlingtime ms: ");
  Serial.println(LoadCell.getSettlingTime());
  Serial.println("Note that the settling time may increase significantly if you use delay() in your sketch!");
  if (LoadCell.getSPS() < 7) {
    Serial.println("!!Sampling rate is lower than specification, check MCU>HX711 wiring and pin designations");
  } else if (LoadCell.getSPS() > 100) {
    Serial.println("!!Sampling rate is higher than specification, check MCU>HX711 wiring and pin designations");
  }


  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback;  //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  static boolean newDataReady = false;
  const int serialPrintInterval = 2000;


  // Read the sensor value
  int sensorValue = digitalRead(gasSensorPin);
  float voltage = sensorValue == HIGH ? 5.0 : 0.0;  // Assuming the gas sensor outputs HIGH when gas is detected

  // Check for new data and start next conversion for HX711
  if (LoadCell.update()) {
    newDataReady = true;
  }

  // Get smoothed value from the dataset and print it
  if (newDataReady) {
    if (millis() > t + serialPrintInterval) {
      float weight = LoadCell.getData() / 1000.0;
      char weightStr[8];
      dtostrf(weight, 6, 2, weightStr);
      Serial.print("Load cell output value: ");
      Serial.println(weight);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Weight: ");
      lcd.print(weight);
      lcd.print("kg");

      // Send weight to Firebase
      if (Firebase.RTDB.set(&fbdo, "tanksInfo/danicab@gmail,com/Weight", weightStr)) {
        Serial.println("Weight sent to Firebase");
      }

      if (Firebase.RTDB.set(&fbdo, "tanksInfo/mark@gmail,com/Weight", weightStr)) {
        Serial.println("Weight sent to Firebase");
      }

      // Check gas leak status and print on LCD
      if (sensorValue == HIGH) {
        lcd.setCursor(0, 1);
        lcd.print("No Leak detected");
        // Send leak status to Firebase
        if (Firebase.RTDB.setString(&fbdo, "tanksInfo/danicab@gmail,com/Status", "No Leak")) {
          Serial.println("No Leak status sent to Firebase");
        }
        if (Firebase.RTDB.setString(&fbdo, "tanksInfo/mark@gmail,com/Status", "No Leak")) {
          Serial.println("No Leak status sent to Firebase");
        }
      } else {
        lcd.setCursor(0, 1);
        lcd.print("leak detected");
        // Send no leak status to Firebase
        if (Firebase.RTDB.setString(&fbdo, "tanksInfo/danicab@gmail,com/Status", "Leak")) {
          Serial.println("leak status sent to Firebase");
        }
        if (Firebase.RTDB.setString(&fbdo, "tanksInfo/mark@gmail,com/Status", "Leak")) {
          Serial.println(" Leak status sent to Firebase");
        }
      }

      // Reset flag and update time
      newDataReady = false;
      t = millis();
    }
  }

  // Receive command from serial terminal, send 't' to initiate tare operation
  if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') LoadCell.tareNoDelay();
  }

  // Check if the last tare operation is complete
  if (LoadCell.getTareStatus()) {
    Serial.println("Tare complete");
  }
}