#include "audio.h"
#include "lcd_display.h"
#include "network.h"
#include "sip_client.h"
#include "web_server.h"

void setup() {
  // Initialize modules
  init_wifi();
  init_sip();
  init_audio();
  init_lcd();
  init_webserver();

  // Register callbacks for incoming calls and call media and state changes
  pjsua_call_set_callback(&on_incoming_call, &on_call_media_state, &on_call_state);

  // Start the SIP client
  start_sip_client();
}

void loop() {
  // Handle incoming web requests
  handle_web_requests();

  // Read audio buffer and send to output
  read_audio_buffer();
}

int main() {
  // Set up the program
  setup();

  // Enter the main loop
  while (true) {
    loop();
  }

  return 0;
}
