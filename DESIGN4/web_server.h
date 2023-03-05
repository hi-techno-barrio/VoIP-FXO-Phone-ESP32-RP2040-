#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <WiFi.h>
#include "network.h"

void init_web_server();
void handle_http_request();
void handle_network_settings_request(WiFiClient& client);
void handle_save_network_settings_request(WiFiClient& client);
void handle_web_request();
IPAddress get_web_server_ip();

#endif
