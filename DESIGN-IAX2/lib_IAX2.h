#ifndef IAX2_H
#define IAX2_H

#include <Ethernet.h>

// Define the IAX2 message types
#define IAX_NEW 2
#define IAX_ACK 6
#define IAX_INCALL 8
#define IAX_HANGUP 9
#define IAX_ACCEPT 10
#define IAX_REJECT 11
#define IAX_AUTHREQ 12
#define IAX_AUTHREP 13
#define IAX_REGREQ 14
#define IAX_REGAUTH 15
#define IAX_REGACK 16
#define IAX_REGREJ 17
#define IAX_REGREL 18
#define IAX_PING 24
#define IAX_PONG 25

// Define the IAX2 message header structure
typedef struct {
  uint16_t messageLength;
  uint8_t messageType;
  uint8_t subclass;
  uint32_t timestamp;
  uint32_t sourceCallNumber;
  uint32_t destinationCallNumber;
  uint8_t reserved[12];
} iaxHeader;

// Define the IAX2 call initiation message structure
typedef struct {
  uint16_t messageLength;
  uint8_t messageType;
  uint8_t subclass;
  uint32_t timestamp;
  uint32_t sourceCallNumber;
  uint32_t destinationCallNumber;
  uint8_t reserved[12];
  // ... other fields ...
} iaxNew;

// Define the IAX2 call initiation response structure
typedef struct {
  uint16_t messageLength;
  uint8_t messageType;
  uint8_t subclass;
  uint32_t timestamp;
  uint32_t sourceCallNumber;
  uint32_t destinationCallNumber;
  uint8_t reserved[12];
  uint8_t acceptCode;
  uint16_t codecPreference;
  uint16_t sampleRate;
  uint8_t reserved2[20];
} iaxNewResponse;

// Define the IAX2 hangup message structure
typedef struct {
  uint16_t messageLength;
  uint8_t messageType;
  uint8_t subclass;
  uint32_t timestamp;
  uint32_t sourceCallNumber;
  uint32_t destinationCallNumber;
  uint8_t reserved[12];
  uint8_t cause;
  // ... other fields ...
} iaxHangup;

// Define the IAX2 registration request message structure
typedef struct {
  uint16_t messageLength;
  uint8_t messageType;
  uint8_t subclass;
  uint32_t timestamp;
  uint32_t sourceCallNumber;
  uint32_t destinationCallNumber;
  uint8_t reserved[12];
  uint8_t username[20];
  uint8_t password[20];
  uint8_t registerServer[40];
  uint8_t refleshInterval[4];
} iaxRegReq;

// Define the IAX2 registration acknowledgement message structure
typedef struct {
  uint16_t messageLength;
  uint8_t messageType;
  uint8_t subclass;
  uint32_t timestamp;
  uint32_t sourceCallNumber;
  uint32_t destinationCallNumber;
  uint8_t reserved[12];
  uint8_t registrationStatus[4];
  uint8_t registrationMessage[40];
  uint8_t registrationServer[40];
  uint8_t refleshInterval[4];
} iaxRegAck;

// Define the IAX2 incoming call message structure
typedef struct {
  uint16_t messageLength;
  uint8_t messageType;
  uint8_t subclass;
  uint32_t timestamp;
  uint32_t sourceCall

    
    //Number and IP of the calling party
uint8_t callingNumber[32];
uint8_t callingName[32];
uint8_t callingIP[32];
//Codec information
uint16_t codecPref;
uint16_t sampleRate;
uint16_t callingFormat;
uint8_t callingCapabilities[32];
//Other fields
uint8_t cause[4];
uint8_t privateKey[16];
} iaxInCall;

// Define the IAX2 incoming call acknowledgement message structure
typedef struct {
uint16_t messageLength;
uint8_t messageType;
uint8_t subclass;
uint32_t timestamp;
uint32_t sourceCallNumber;
uint32_t destinationCallNumber;
uint8_t reserved[12];
uint32_t newCallNumber;
// ... other fields ...
} iaxInCallAck;

// Define the IAX2 forward message structure
typedef struct {
uint16_t messageLength;
uint8_t messageType;
uint8_t subclass;
uint32_t timestamp;
uint32_t sourceCallNumber;
uint32_t destinationCallNumber;
uint8_t reserved[12];
// ... other fields ...
} iaxForward;

// Define the IAX2 forward acknowledgement message structure
typedef struct {
uint16_t messageLength;
uint8_t messageType;
uint8_t subclass;
uint32_t timestamp;
uint32_t sourceCallNumber;
uint32_t destinationCallNumber;
uint8_t reserved[12];
uint8_t forwardCode;
// ... other fields ...
} iaxForwardAck;

class IAX2 {
public:
IAX2(byte mac[], IPAddress localIP, int localPort);
bool connect(IPAddress serverIP, int serverPort);
void disconnect();
bool sendNew(iaxNew *newMessage, iaxNewResponse *response);
bool sendHangup(iaxHangup *hangupMessage);
bool sendRegReq(iaxRegReq *regReqMessage, iaxRegAck *regAckMessage);
bool receive();
private:
EthernetClient client;
byte mac[6];
IPAddress localIP;
int localPort;
uint8_t iaxBuffer[IAX_BUFFER_SIZE];
void parseHeader(iaxHeader *header);
};

#endif
