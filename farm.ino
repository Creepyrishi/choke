#include <DHT.h>
#include <Servo.h>

#define DHTPIN 7          // Pin connected to DHT11
#define DHTTYPE DHT11     // DHT 11 type
#define NUTRITION_PIN A0  // Pin connected to nutrition sensor
#define WATER_PUMP_PIN 8  // Pin for water pump relay
#define CONVEYOR_PIN 9    // Pin for conveyor belt motor relay
#define SERVO_PIN 10      // Pin for servo motor

DHT dht(DHTPIN, DHTTYPE);
Servo servoMotor;

float temperature, humidity, nutritionLevel;
String data = "";

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  
  // Initialize DHT sensor and servo motor
  dht.begin();
  servoMotor.attach(SERVO_PIN);
  
  // Set motor pins as output
  pinMode(WATER_PUMP_PIN, OUTPUT);
  pinMode(CONVEYOR_PIN, OUTPUT);
  
  // Start the web server (in ESP32, code for this would go here)
}

void loop() {
  // Read sensor data
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  nutritionLevel = analogRead(NUTRITION_PIN);

  // Check if readings are valid
  if (isnan(temperature) || isnan(humidity) || isnan(nutritionLevel)) {
    Serial.println("Failed to read from DHT sensor or nutrition sensor");
    return;
  }

  // Send sensor data to web server (for ESP32 to handle)
  data = "Temperature: " + String(temperature) + "C, ";
  data += "Humidity: " + String(humidity) + "%, ";
  data += "Nutrition Level: " + String(nutritionLevel);

  Serial.println(data); // Send data to ESP32 via Serial for web update

  // Check for commands from web server
//   if (Serial.available() > 0) {
//     String command = Serial.readString();

//     if (command == "activate_water_pump") {
//       digitalWrite(WATER_PUMP_PIN, HIGH);  // Turn on water pump
//     }
//     else if (command == "deactivate_water_pump") {
//       digitalWrite(WATER_PUMP_PIN, LOW);   // Turn off water pump
//     }
//     else if (command == "start_conveyor") {
//       digitalWrite(CONVEYOR_PIN, HIGH);    // Start conveyor motor
//     }
//     else if (command == "stop_conveyor") {
//       digitalWrite(CONVEYOR_PIN, LOW);     // Stop conveyor motor
//     }
//     else if (command == "mix_slurry") {
//       servoMotor.write(90);                // Move servo to mix slurry
//     }
//     else if (command == "stop_mixing") {
//       servoMotor.write(0);                 // Move servo to initial position
//     }
//   }

//   delay(2000);  // Wait before taking the next set of readings

unsigned long previousMillis = 0;
const long interval = 5000; // 5 seconds

void handleCommand(String command) {
  command.trim(); // Remove any leading or trailing whitespace

  if (command.length() == 5 && command[0] == '[' && command[4] == ']') {
    int waterPumpState = command[1] - '0';
    int conveyorState = command[2] - '0';
    int slurryState = command[3] - '0';

    if (waterPumpState == 1) {
      digitalWrite(WATER_PUMP_PIN, HIGH);  // Turn on water pump
      previousMillis = millis();
      while (millis() - previousMillis < interval) {
        // Do nothing, just wait for the interval to pass
      }
      digitalWrite(WATER_PUMP_PIN, LOW);   // Turn off water pump
    }

    if (conveyorState == 1) {
      digitalWrite(CONVEYOR_PIN, HIGH);    // Start conveyor motor
      previousMillis = millis();
      while (millis() - previousMillis < interval) {
        // Do nothing, just wait for the interval to pass
      }
      digitalWrite(CONVEYOR_PIN, LOW);     // Stop conveyor motor
    }

    if (slurryState == 1) {
      servoMotor.write(90);                // Move servo to mix slurry
      previousMillis = millis();
      while (millis() - previousMillis < interval) {
        // Do nothing, just wait for the interval to pass
      }
      servoMotor.write(0);                 // Move servo to initial position
    }
  }
}

if (Serial.available() > 0) {
  String command = Serial.readString();
  handleCommand(command);
}

delay(2000);  // Wait before taking the next set of readings