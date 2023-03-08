#include <IAX2.h>
#include <Audio.h>

// Define the IAX2 connection details
#define IAX_SERVER_IP IPAddress(192, 168, 0, 100)
#define IAX_SERVER_PORT 4569

// Define the audio buffer size
#define AUDIO_BUFFER_SIZE 160

// Define the microphone pin
#define MIC_PIN 36

// Create an instance of the IAX2 library
IAX2 iax2(WiFi.macAddress().c_str(), WiFi.localIP(), 4569);

// Create an instance of the Audio library
Audio audio(MIC_PIN, AUDIO_BUFFER_SIZE);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting up...");

  // Connect to Wi-Fi
  WiFi.begin("SSID", "PASSWORD");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }
  Serial.println("Connected to Wi-Fi");

  // Connect to the IAX2 server
  while (!iax2.connect(IAX_SERVER_IP, IAX_SERVER_PORT)) {
    Serial.println("Could not connect to IAX2 server. Retrying...");
    delay(1000);
  }
  Serial.println("Connected to IAX2 server");

  // Start the audio stream
  audio.start();
}

void loop() {
  // Read audio data from the microphone
  int16_t audioData[AUDIO_BUFFER_SIZE];
  audio.read(audioData);

  // Create a new IAX2 message
  iaxNew newMessage;
  newMessage.messageLength = sizeof(newMessage) + sizeof(audioData);
  newMessage.messageType = IAX_NEW;
  newMessage.subclass = 0;
  newMessage.timestamp = 0;
  newMessage.sourceCallNumber = 0;
  newMessage.destinationCallNumber = 0;
  memset(newMessage.reserved, 0, sizeof(newMessage.reserved));

  // Send the new message and receive the response
  iaxNewResponse response;
  if (iax2.sendNew(&newMessage, &response)) {
    // Send the audio data
    if (iax2.client.write((uint8_t*)audioData, sizeof(audioData)) != sizeof(audioData)) {
      Serial.println("Error sending audio data");
    }
  } else {
    Serial.println("Error sending new message");
  }
  
  // Wait for the IAX2 PONG message to confirm that the server received the data
  while (!iax2.receive()) {
    delay(100);
  }
  if (iax2.client.available()) {
    uint8_t buffer[256];
    int bytesRead = iax2.client.read(buffer, sizeof(buffer));
    for (int i = 0; i < bytesRead; i++) {
      iax2.parseHeader((iaxHeader*)(&buffer[i]));
    }
  }
}
