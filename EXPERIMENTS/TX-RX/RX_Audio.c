#include <WiFi.h>
#include <WiFiUdp.h>

// Replace with your network credentials
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// Replace with the IP address of the receiver
const char* receiverIP = "192.168.1.2";

// Replace with the port number you want to use
const int port = 1234;

// ADC pin for analog input
const int ADC_PIN = 34;

// DAC pin for audio output
const int DAC_PIN = 25;

// Filter parameters
const float ALPHA = 0.5;
int16_t filteredValue = 0;

// Create a UDP instance
WiFiUDP udp;

void setup() {
  // Connect to Wi-Fi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize DAC
  ledcSetup(0, 2000, 8);
  ledcAttachPin(DAC_PIN, 0);

  // Begin UDP communication
  udp.begin(1234);  // Replace with the port number you want to use

  Serial.println("Ready to transmit audio");
}

void loop() {
  // Read analog input and filter it
  int16_t analogValue = analogRead(ADC_PIN);
  filteredValue = filteredValue + ALPHA * (analogValue - filteredValue);

  // Compress to uLaw and aLaw
  uint8_t uLawValue = compressULaw(filteredValue);
  uint8_t aLawValue = compressALaw(filteredValue);

  // Send the compressed data over UDP
  udp.beginPacket(receiverIP, port);
  udp.write((const uint8_t*)&uLawValue, 1);
  udp.write((const uint8_t*)&aLawValue, 1);
  udp.endPacket();

  // Output the received data to DAC
  ledcWrite(0, uLawValue); // Use uLaw value for output to DAC

  // Print the original and compressed values
  Serial.print("Analog Value: ");
  Serial.print(analogValue);
  Serial.print(" | Filtered Value: ");
  Serial.print(filteredValue);
  Serial.print(" | uLaw Value: ");
  Serial.print(uLawValue);
  Serial.print(" | aLaw Value: ");
  Serial.println(aLawValue);

  delay(1);
}
