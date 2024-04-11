# TANK GAS WEIGHING SCALE AND LEAK MONITORING USING ESP32 WITH FIREBASE
## DESCRIPTION
With an ESP32 microcontroller and expansion board, coupled with an HX711 load cell amplifier, MQ2 gas sensor, four load cells, and an I2C LCD, this project aims to create a comprehensive gas tank monitoring system. Utilizing the ESP32's capabilities, the system continuously reads data from the load cells to accurately measure the tank's weight distribution. Concurrently, the MQ2 sensor detects gas levels and potential leaks, ensuring safety and security. The I2C LCD provides real-time visual feedback on weight and gas status for local monitoring, while Firebase integration enables remote access to data for logging and analysis. This setup offers a versatile solution suitable for various applications, from industrial to household gas monitoring needs.
# WIRE DIAGRAM
![Alt text](https://github.com/messiderata/Scale/blob/main/Wire%20Diagram.png)

## LOAD CELL WIRING
![Alt text](https://github.com/firebitlab/arduino/blob/master/HX711/hx711.jpg)

# INSTRUCTION
Include Libraries and Define Constants:

## 1. Include necessary libraries such as WiFi, Wire, LiquidCrystal_I2C, HX711_ADC, and Firebase_ESP_Client.
Define constants for the LCD address, dimensions, WiFi credentials, Firebase API key, and database URL.

## 2. Initialize Global Variables and Objects:
Declare global variables for Firebase authentication, LCD display, sensor pins, Firebase data object, and other necessary variables.

## 3. Setup Function:
In the setup() function:
Begin serial communication for debugging purposes.
Initialize the LCD display and backlight.
Set up the gas sensor pin mode.
Configure and calibrate the HX711 load cell amplifier.
Connect to the WiFi network.

Sign up for Firebase authentication and configure Firebase connection.

## 4. Main Loop Function:
In the loop() function:
Continuously read data from the gas sensor and load cells.
Process sensor data and calculate weight and gas status.
Display weight and gas status on the LCD display.
Send weight and gas status data to Firebase Realtime Database for remote monitoring.

## 5. Gas Sensor Logic:
Read the output of the gas sensor pin.
Determine gas presence based on sensor output.
Display corresponding messages on the LCD and send gas status to Firebase.

## 6. Load Cell Logic:
Read data from the HX711 load cell amplifier.
Calculate weight from the sensor data.
Display weight on the LCD and send it to Firebase.

## 7.Tare Operation (Optional):
Listen for commands from the serial terminal.
Perform a tare operation on the load cell if 't' is received.
Print tare completion status.

## 8.Firebase Integration:
Configure Firebase API key and database URL.
Sign up for Firebase authentication.
Send sensor data (weight and gas status) to Firebase Realtime Database.
This step-by-step breakdown provides a comprehensive guide for understanding and setting up the code to monitor gas levels and weight data using an ESP32 microcontroller, load cells, gas sensor, and Firebase integration.
