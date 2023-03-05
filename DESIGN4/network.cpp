#include "network.h"

// Network configuration
static char ssid[64] = "default_ssid";
static char password[64] = "default_password";
static IPAddress ip_addr(192, 168, 1, 100);
static IPAddress subnet_mask(255, 255, 255, 0);
static IPAddress gateway(192, 168, 1, 1);
static IPAddress dns1(8, 8, 8, 8);
static IPAddress dns2(8, 8, 4, 4);

