#include <Adafruit_Sensor.h> 
#include <DHT.h> 
#include <DHT_U.h> 
#include <ESP32Servo.h> 
#include <BluetoothSerial.h> 
 
String voice; 
 
//TempFanSpeed Control 
#define DHTPIN 13         // DHT11 sensor data pin 
#define DHTTYPE DHT11    // DHT sensor type 
#define MOTOR_PIN_ENA 12  // Enable pin for motor driver (PWM pin) 
#define MOTOR_PIN_IN1 14  // Input pin 1 for motor driver 
#define MOTOR_PIN_IN2 27  // Input pin 2 for motor driver 
 
#define TEMPERATURE_THRESHOLD 15  //Med   
#define TEMPERATURE_THRESHOLD1 20 // Temperature threshold to adjust motor speed  MAX 
DHT dht(DHTPIN, DHTTYPE); 
 
//Fire Alarm 
// const int flamepin=33; 
// const int buzpin=25; 
// const int threshold=200; // sets threshold value for flame sensor 
 
 
//Smoke Alarm 
#define BUZZER_PIN 25    
 
#define SENSOR_PIN 35 
 
 
//Door 
#define TRIGGER_PIN  32 // GPIO pin connected to the trigger pin of ultrasonic sensor 
#define ECHO_PIN     26 // GPIO pin connected to the echo pin of ultrasonic sensor 
#define SERVO_PIN    4  // GPIO pin connected to the servo motor 
#define MAX_DISTANCE 5 // Maximum distance (in centimeters) to detect an object 
 
 
//LED 
#define LED_PIN 2 
 
Servo servo; // Create a servo object 
 
BluetoothSerial SerialBT; 
 
void setup() { 
  Serial.begin(9600); 
 
 
  SerialBT.begin("Smart Home Automation"); 
  dht.begin(); 
  // //Firealaram 
  // pinMode(flamepin,INPUT); 
  // pinMode(buzpin,OUTPUT); 
 
  //Smokealarm 
  pinMode(BUZZER_PIN, OUTPUT); 
 
  //Door 
  pinMode(TRIGGER_PIN, OUTPUT); 
  pinMode(ECHO_PIN, INPUT); 
  servo.attach(SERVO_PIN); 
 
  //LED 
  pinMode(LED_PIN, OUTPUT); 
 
  //fan 
  pinMode(MOTOR_PIN_ENA, OUTPUT); 
  pinMode(MOTOR_PIN_IN1, OUTPUT); 
  pinMode(MOTOR_PIN_IN2, OUTPUT); 
} 
 
 
void loop() { 
  // Read from Bluetooth 
  while (SerialBT.available()) { 
    delay(10); 
    char c = SerialBT.read(); 
    if (c == '#') { 
      break; 
    } 
    voice += c; 
  } 
    voice.trim(); 
 
  // Handle voice commands if any 
  if (voice.length() > 0) { 
    Serial.println(voice); 
 
    if (voice == "fanon") { 
      Serial.println("Fan is ON"); 
      TempFanSpeed(); 
    } 
    else if (voice == "fanoff") { 
      Serial.println("Fan is OFF"); 
      digitalWrite(MOTOR_PIN_ENA, LOW); 
      digitalWrite(MOTOR_PIN_IN1, LOW); 
      digitalWrite(MOTOR_PIN_IN2, LOW); 
    } 
    else if (voice == "ledon") { 
      Serial.println("LED is ON"); 
      digitalWrite(LED_PIN, HIGH); 
 
 

    } 
    else if (voice == "ledoff") { 
      Serial.println("LED is OFF"); 
      digitalWrite(LED_PIN, LOW); 
    } 
 
    // Clear the voice command 
    voice = ""; 
  } 
 
  // Always check the smoke alarm and door status 
  SmokeAlarm(); 
  Door(); 
  delay(2000); 
} 
 
void TempFanSpeed(){ 
   
  float temperature = dht.readTemperature(); // Read temperature in Celsius 
  if (isnan(temperature)) { 
    Serial.println("Failed to read temperature from DHT sensor!"); 
    return; 
  } 
  Serial.println(); 
  Serial.print("Temperature: "); 
  Serial.print(temperature); 
  Serial.print(" °C"); 
 
  // Adjust motor speed based on temperature 
  if (temperature > TEMPERATURE_THRESHOLD1) { 
    // Increase motor speed 
    analogWrite(MOTOR_PIN_ENA, 255);    // Set the motor speed to maximum (255) 
    digitalWrite(MOTOR_PIN_IN1, HIGH);  // Set motor direction (forward) 
    digitalWrite(MOTOR_PIN_IN2, LOW); 
    Serial.print("  Motor Speed: Max  "); 
  } 
  else if (temperature > TEMPERATURE_THRESHOLD) { 
    // Increase motor speed 
    analogWrite(MOTOR_PIN_ENA, 150);    // Set the motor speed to a value (150) 
    digitalWrite(MOTOR_PIN_IN1, HIGH);  // Set motor direction (forward) 
    digitalWrite(MOTOR_PIN_IN2, LOW); 
    Serial.print("  Motor Speed: Med  "); 
  } 
  else { 
    // Decrease motor speed 
    analogWrite(MOTOR_PIN_ENA, 45);    // Set the motor speed to a lower value (150) 
    digitalWrite(MOTOR_PIN_IN1, HIGH);  // Set motor direction (forward) 
    digitalWrite(MOTOR_PIN_IN2, LOW); 
    Serial.print("  Motor Speed: Low  "); 
    } 
} 
 
  void FireAlarm() { 
 
 
 
    int flamesensvalue = analogRead(flamepin); 
    Serial.println(); 
    Serial.print("FlamesValue: "); 
    Serial.print(flamesensvalue); 
    if (flamesensvalue <= threshold) { 
        Serial.println(" - Flames Detected"); 
        digitalWrite(buzpin, HIGH); 
    } else { 
        digitalWrite(buzpin, LOW); 
        Serial.println(" - Flames Not Detected"); 
    } 
} 
 
void SmokeAlarm(){ 
    int sensorValue = analogRead(SENSOR_PIN);  // Read the analog value from the smoke sensor 
    Serial.println(); 
    Serial.print("Smoke: "); 
    // Check if the sensor value exceeds the threshold 
    Serial.println(sensorValue); 
    if (sensorValue > 1000) { 
        // Activate the buzzer if the sensor value is greater than the threshold 
        Serial.println("Buzzer sound ON"); 
        digitalWrite(BUZZER_PIN, HIGH); 
    } else { 
        // Turn off the buzzer if the sensor value is less than or equal to the threshold 
        Serial.println("Buzzer sound OFF"); 
        digitalWrite(BUZZER_PIN, LOW); 
    } 
} 
 
void Door(){ 
  long duration, distance; 
  // Triggering ultrasonic sensor 
  digitalWrite(TRIGGER_PIN, LOW); 
  delayMicroseconds(2); 
  digitalWrite(TRIGGER_PIN, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(TRIGGER_PIN, LOW); 
 
  // Measuring echo time to calculate distance 
  duration = pulseIn(ECHO_PIN, HIGH); 
   
  // Calculating distance in centimeters 
  distance = duration * 0.034 / 2; 
   
  Serial.print("Distance: "); 
  Serial.print(distance); 
  Serial.println(" cm"); 
 
  // If an object is within range, open the door 
  if (distance > 0 && distance < MAX_DISTANCE) { 
    Serial.println("Door Open"); 
    servo.write(90); // 90 degrees (adjust as needed) 
 
  } else { 
    Serial.println("Door Close"); 
    servo.write(0); // 0 degrees (adjust as needed) 
  } 
}