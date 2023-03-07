#include <WiFi.h>
#include <WebServer.h>
#include <driver/dac.h>
#include <EEPROM.h>

#define IAX2_USER "user"
#define IAX2_PASSWD "password"
#define IAX2_SERVER "192.168.0.1"
#define IAX2_PORT 4569

#define MIC_PIN 34
#define SIGNAL_CONDITIONER_PIN 35
#define SPEAKER_PIN 25
#define LCD_ADDRESS 0x27
#define LCD_ROWS 2
#define LCD_COLS 16

#define MIC_SAMPLE_RATE 16000
#define MIC_SAMPLE_BITS 12

#define CALL_BUTTON_PIN 4
#define CALL_FORWARD_NUMBER "0987654321"
bool callForwardingEnabled = true;

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_ROWS, LCD_COLS);
IAX2Client iax2Client;
volatile bool callActive = false;
WebServer server(80);

void applyDigitalFilter(int *samples, int numSamples) {
  // Define filter coefficients
  float b[] = {0.0976, 0.4306, 0.4306, 0.0976};
  float a[] = {1.0000, -0.5970, 0.1918, -0.0180};

  // Initialize filter state variables
  float x[4] = {0};
  float y[4] = {0};

  // Apply filter to samples
  for (int i = 0; i < numSamples; i++) {
    // Shift state variables
    for (int j = 3; j > 0; j--) {
      x[j] = x[j - 1];
      y[j] = y[j - 1];
    }

    // Update input
    x[0] = samples[i];

    // Compute output
    y[0] = b[0] * x[0] + b[1] * x[1] + b[2] * x[2] + b[3] * x[3]
         - a[1] * y[1] - a[2] * y[2] - a[3] * y[3];

    // Save output
    samples[i] = (int) y[0];
  }
}

void processAudioSample() {
  // Read audio sample
  int sample = adc1_get_raw(ADC1_CHANNEL_6);

  // Apply a digital filter to remove noise
  applyDigitalFilter(&sample, 1);

  // Scale sample to match PAM4803 input range
  int output = map(sample, 0, 4095, 0, 255);

  // Output sample to PAM4803 amplifier
  dac_output_voltage(DAC_CHANNEL_1, output);
}

void handleRoot() {
  server.send(200, "text/html", "<h1>ESP32 Network Configuration</h1>"
                                 "<form method='POST' action='/save'>"
                                 "IP address: <input type='text' name='ip' value=''><br>"
                                 "Subnet mask: <input type='text' name='subnet' value=''><br>"
                                 "Gateway: <input type='text' name='gateway' value=''><br>"
                                 "Primary DNS: <input type='text' name='dns1' value=''><br>"
                                 "Secondary DNS: <input type='text' name='dns2' value=''><br>"
                                 "<input type='submit' value='Save'>"
                                 "</form>");
}

void handleSave() {
  String ip = server.arg("ip");
  String subnet = server.arg("subnet");
  String gateway = server.arg("gateway");
  String dns1 = server.arg("dns1");
  String dns2 = server.arg("dns2");

  // Save network settings
  EEPROM.begin(512);
  EEPROM.writeString(0, ip);
  EEPROM.writeString(16, subnet);
  EEPROM.writeString(32, gateway);
  EEPROM.writeString(48, dns1);
  EEPROM.writeString(64, dns2);
  EEPROM.commit();
  EEPROM.end();

  server.send(200, "text/html", "<h1>Settings saved!</h1>");
}

void setup() {
// Connect to WiFi
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
delay(1000);
Serial.println("Connecting to WiFi...");
}
Serial.println("Connected to WiFi");

// Configure ADC and DAC pins
adc1_config_width(ADC_WIDTH_BIT_12);
dac_output_enable(SPEAKER_PIN);

// Configure audio input
esp_err_t err = adc_gpio_init(MIC_PIN);
if (err != ESP_OK) {
Serial.println("Failed to initialize ADC GPIO");
}

err = adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_0);
if (err != ESP_OK) {
Serial.println("Failed to configure ADC channel attenuation");
}

err = adc1_sample_rate_config(MIC_SAMPLE_RATE);
if (err != ESP_OK) {
Serial.println("Failed to configure ADC sample rate");
}

// Connect to IAX2 server
iax2Client.begin(IAX2_SERVER, IAX2_PORT);
iax2Client.setUser(IAX2_USER);
iax2Client.setPassword(IAX2_PASSWD);
iax2Client.connect();

// Configure LCD display
lcd.init();
lcd.backlight();
lcd.setCursor(0, 0);
lcd.print("Caller ID:");
lcd.setCursor(0, 1);
lcd.print("Time:");

// Configure web server
server.on("/", handleRoot);
server.on("/save", handleSave);
server.begin();
Serial.println("Web server started");

// Configure GPIO pins
pinMode(SPEAKER_PIN, OUTPUT);
pinMode(SIGNAL_CONDITIONER_PIN, OUTPUT);

// Set initial output values
digitalWrite(SPEAKER_PIN, LOW);
digitalWrite(SIGNAL_CONDITIONER_PIN, LOW);

// Attach interrupt for call button
attachInterrupt(digitalPinToInterrupt(CALL_BUTTON_PIN), handleCallButton, FALLING);

// Start audio processing
err = adc1_start();
if (err != ESP_OK) {
Serial.println("Failed to start ADC");
}

// Print IP address
Serial.print("Local IP address: ");
Serial.println(WiFi.localIP());
}

void loop() {
// Process audio sample
processAudioSample();

// Handle web server requests
server.handleClient();

// Handle IAX2 client events
iax2Client.loop();

// Handle incoming calls
if (iax2Client.isIncomingCall() && !callActive) {
// Answer incoming call
iax2Client.answerCall();
callActive = true;

// Display caller ID
lcd.setCursor(10, 0);
lcd.print(iax2Client.getCallerID());

// Enable signal conditioning for speaker
digitalWrite(SIGNAL_CONDITIONER_PIN, HIGH);
}


// Handle call forwarding
if (iax2Client.isIncomingCall() && callForwardingEnabled) {
iax2Client.forwardCall(CALL_FORWARD_NUMBER);
}

// Update call timer
if (callActive) {
unsigned long currentMillis = millis();
unsigned long elapsedSeconds = (currentMillis - callStartTime) / 1000;
unsigned long minutes = elapsedSeconds / 60;
unsigned long seconds = elapsedSeconds % 60;// Display call time
lcd.setCursor(5, 1);
lcd.print(String(minutes, DEC));
lcd.print(":");
lcd.print(String(seconds, DEC));}
}

void handleCallButton() {
if (!callActive) {
// Place outgoing call
iax2Client.placeCall(CALL_NUMBER);
callActive = true;// Display call time
lcd.setCursor(5, 1);
lcd.print("00:00");

// Record call start time
callStartTime = millis();
// Clear call time
lcd.setCursor(5, 1);
lcd.print("    ");
}
}



