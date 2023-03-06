 #include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <math.h>

#define ADC_PIN 36
#define SAMPLE_RATE 8000
#define SAMPLE_SIZE 8
#define SAMPLE_BUFFER_SIZE 64
#define SERVER_IP "192.168.1.100"  // Replace with the IP address of the server

// Low-pass filter
const float RC = 1.0 / (2.0 * M_PI * 100.0); // 100 Hz cutoff frequency
const float DT = 1.0 / SAMPLE_RATE;
const float ALPHA = DT / (RC + DT);
static float filteredValue = 0.0;
const int16_t G711_QUANTIZATION_FACTOR = 32767;

const uint8_t G711_EXPONENT_TABLE[128] = {
0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6
};


const uint8_t G711_COMPAND_TABLE[128] = {
16, 17, 19, 21, 23, 25, 28, 31, 34, 37, 41, 45, 50, 55, 61, 68,
76, 84, 94, 105, 117, 131, 147, 165, 185, 208, 233, 262, 294, 330, 370, 415,
466, 523, 587, 659, 740, 831, 932, 1046, 1175, 1319, 1480, 1661, 1864, 2093, 2349, 2637, 2960, 3322,
3729, 4186, 4699, 5274, 5919, 6644, 7458, 8372, 9397, 10548, 11835, 13267, 14846, 16686, 18789, 21168,
23834, 26801, 30087, 33720, 37731, 42155, 47030, 52403, 58326, 64852, 72046, 79968, 88792, 98527, 109591, 121999,
135891, 151407, 168698, 187941, 209335, 233108, 259522, 288867, 321549, 357907, 398318, 443193, 493090, 548648, 610567,
679612, 756708, 842891, 939192, 1046728, 1166894, 1304064, 1459817, 1636365, 1836454, 2063055, 2319478, 2609444, 2937014, 3306602,
3723271, 4192516, 4719340, 5309193, 5968088, 6702603, 7520149, 8428968, 9438393, 10556256, 11852321, 13326157, 14991223, 16872755, 18987915,
21357548, 24004614, 26955419, 30240920, 33894658, 37954212, 42461026, 47462045, 53010034, 59163228, 65986453, 73551827, 81940896, 91244829,
101606129, 113321754, 126796218, 142275834, 159938951, 180000000
};
const uint8_t ULAW_COMPAND_TABLE[256] = {
0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7,7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,8, 8, 8, 8, 8, 
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 };

const uint8_t ALAW_COMPAND_TABLE[256] = {
1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8,
9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
25, 27, 29, 31, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44,
45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
61, 62, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77,
78, 79, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94,
95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 
109, 110,111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124,
125, 126,127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 
141, 142,
143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158,
159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174,
175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190,
191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206,
207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222,
223, 224, 225, 226, 227, 228,228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 
238, 239, 240, 241, 242, 243,244, 245, 246, 247, 248, 249, 250, 251, 252, 253,
254, 255, 255, 255, 255, 255
};

WiFiUDP udp;

void setup() {
  Serial.begin(9600);
  WiFi.begin("ssid", "password");  // Replace with your network credentials
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  // Read analog input and filter it
  int16_t analogValue = analogRead(ADC_PIN);
  filteredValue = filteredValue + ALPHA * (analogValue - filteredValue);

  // Compress to uLaw and aLaw
  uint8_t uLawValue = compressULaw(filteredValue);
  uint8_t aLawValue = compressALaw(filteredValue);

  // Send the compressed data over UDP
  udp.beginPacket(SERVER_IP, 1234);  // Replace with the port number you want to use
  udp.write((const uint8_t*)&uLawValue, 1);  // Send one byte of data
  udp.write((const uint8_t*)&aLawValue, 1);
  udp.endPacket();

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

uint8_t compressULaw(int16_t sample) {
  sample = sample / 2; // Scale to 8 bits
  sample = sample ^ 0x8000; // Flip sign bit
  uint8_t sign = (sample & 0x8000) >> 8;
  if (sign != 0) {
    sample = -sample;
  }
  if (sample > 32635) {
    sample = 32635;
  }
  sample = sample + 132;
  uint8_t exponent = G711_EXPONENT_TABLE[(sample >> 7) & 0x7F];
  uint8_t mantissa = (sample >> (exponent + 3)) & 0x0F;
  return ~(sign | (exponent << 4) | mantissa);
}

uint8_t compressALaw(int16_t sample) {
  sample = sample / 2; // Scale to 8 bits
  sample = sample ^ 0x8000; // Flip sign bit
  uint8_t sign = (sample & 0x8000) >> 8;
  if (sign != 0) {
    sample = (~sample + 1) & 0x7FFF; // Take 2's complement and clear sign bit
  }
  if (sample > 32123) {
    sample = 32123;
  }
  uint8_t exponent = 7 - log2(sample); // Compute exponent
  uint8_t mantissa = (sample >> (exponent + 3)) & 0x0F;
  return ~(sign | (exponent << 4) | mantissa);
}

int main() {
  setup();
  while(1) {
    loop();
  }
}





