#include "IAX2.h"

IAX2::IAX2(uint8_t mac[], IPAddress localIP, int localPort) {
  memcpy(this->mac, mac, 6);
  this->localIP = localIP;
  this->localPort = localPort;
}

bool IAX2::connect(IPAddress serverIP, int serverPort) {
  if (client.connect(serverIP, serverPort)) {
    return true;
  }
  return false;
}

void IAX2::disconnect() {
  client.stop();
}

bool IAX2::sendNew(iaxNew *newMessage, iaxNewResponse *response) {
  // Send the new call message
  client.write((uint8_t *)newMessage, sizeof(iaxNew));

  // Wait for the response
  uint16_t bytesReceived = client.read((uint8_t *)response, sizeof(iaxNewResponse));
  if (bytesReceived != sizeof(iaxNewResponse)) {
    return false;
  }
  return true;
}

bool IAX2::sendHangup(iaxHangup *hangupMessage) {
  // Send the hangup message
  client.write((uint8_t *)hangupMessage, sizeof(iaxHangup));

  // Wait for the response
  iaxHeader header;
  client.read((uint8_t *)&header, sizeof(iaxHeader));
  if (header.messageType == IAX_ACK) {
    return true;
  }
  return false;
}

bool IAX2::sendRegReq(iaxRegReq *regReqMessage, iaxRegAck *regAckMessage) {
  // Send the registration request message
  client.write((uint8_t *)regReqMessage, sizeof(iaxRegReq));

  // Wait for the response
  uint16_t bytesReceived = client.read((uint8_t *)regAckMessage, sizeof(iaxRegAck));
  if (bytesReceived != sizeof(iaxRegAck)) {
    return false;
  }
  return true;
}

bool IAX2::receive() {
  // Check if there is data available to read
  if (client.available() > 0) {
    // Read the message header
    iaxHeader header;
    client.read((uint8_t *)&header, sizeof(iaxHeader));

    // Parse the header and read the message body
    parseHeader(&header);
    uint16_t bytesRead = client.read(iaxBuffer, header.messageLength - sizeof(iaxHeader));

    // Handle the message based on its type
    switch (header.messageType) {
      case IAX_INCALL:
        handleIncomingCall((iaxInCall *)iaxBuffer);
        break;
      default:
        break;
    }
    return true;
  }
  return false;
}

bool IAX2::handleIncomingCall(iaxInCall *incomingCall) {
  if (incomingCall->state == IAX_INCOMING) {
    // Incoming call - answer it
    answerCall(incomingCall->callno);
    return true;
  } else if (incomingCall->state == IAX_CALL_PROCEEDING) {
    // Call proceeding - do nothing
    return true;
  } else if (incomingCall->state == IAX_RINGING) {
    // Call ringing - play ringing tone
    playRingingTone();
    return true;
  } else if (incomingCall->state == IAX_CALL_ESTABLISHED) {
    // Call established - set audio codec
    setAudioCodec(incomingCall->callno, audioCodec);
    return true;
  } else if (incomingCall->state == IAX_CALL_HANGUP) {
    // Call hangup - stop audio playback
    stopAudioPlayback();
    return true;
  } else {
    // Unknown call state - do nothing
    return false;
  }
}



bool IAX2::forwardCall(iaxNew *newMessage, iaxNewResponse *response, IPAddress forwardIP, int forwardPort) {
  // Forward the call to the specified IAX2 server
  IAX2 forwardIAX(mac, localIP, localPort);
  if (!forwardIAX.connect(forwardIP, forwardPort)) {
    return false;
  }
  if (!forwardIAX.sendNew(newMessage, response)) {
    return false;
  }
  return true;
}

void IAX2::parseHeader(iaxHeader *header) {
  // Swap the byte order of the header fields
  header->messageLength = ntohs(header->messageLength);
  header->timestamp = ntohl(header->timestamp);
  header->sourceCallNumber = ntohl(header->sourceCallNumber);
  header->destinationCallNumber = ntohl(header->destinationCallNumber);
}

