#ifndef NETWORK_H
#define NETWORK_H

#include <WiFi.h>

void init_wifi();
IPAddress get_local_ip();
void set_network_config(char* ssid, char* password, IPAddress ip_addr, IPAddress subnet_mask, IPAddress gateway, IPAddress dns1, IPAddress dns2);
void get_network_config(char* ssid_out, char* password_out, IPAddress& ip_addr_out, IPAddress& subnet_mask_out, IPAddress& gateway_out, IPAddress& dns1_out, IPAddress& dns2_out);

#endif
