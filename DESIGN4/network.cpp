#include "network.h"

// Network configuration
static char ssid[64] = "default_ssid";
static char password[64] = "default_password";
static IPAddress ip_addr(192, 168, 1, 100);
static IPAddress subnet_mask(255, 255, 255, 0);
static IPAddress gateway(192, 168, 1, 1);
static IPAddress dns1(8, 8, 8, 8);
static IPAddress dns2(8, 8, 4, 4);
// Initialize the WiFi connection
void init_wifi() {
// Set WiFi mode to station
WiFi.mode(WIFI_STA);

// Connect to the WiFi network
WiFi.begin(ssid, password);

// Wait for connection
while (WiFi.status() != WL_CONNECTED) {
delay(1000);
}

// Configure network settings
IPAddress gateway_ip = WiFi.gatewayIP();
if (gateway_ip != (uint32_t)0) {
gateway = gateway_ip;
}

IPAddress subnet_mask_ip = WiFi.subnetMask();
if (subnet_mask_ip != (uint32_t)0) {
subnet_mask = subnet_mask_ip;
}

IPAddress dns1_ip = WiFi.dnsIP(0);
if (dns1_ip != (uint32_t)0) {
dns1 = dns1_ip;
}

IPAddress dns2_ip = WiFi.dnsIP(1);
if (dns2_ip != (uint32_t)0) {
dns2 = dns2_ip;
}

// Configure IP address
IPAddress ip;
bool success = false;
for (int i = 0; i < 10 && !success; i++) {
success = WiFi.config(ip_addr, gateway, subnet_mask, dns1, dns2);
delay(1000);
}

if (!success) {
Serial.println("Failed to configure network settings.");
}

// Print network information
Serial.println("WiFi connected");
Serial.print("IP address: ");
Serial.println(WiFi.localIP());
Serial.print("Subnet mask: ");
Serial.println(subnet_mask);
Serial.print("Gateway: ");
Serial.println(gateway);
Serial.print("DNS server 1: ");
Serial.println(dns1);
Serial.print("DNS server 2: ");
Serial.println(dns2);
}

// Get the local IP address
IPAddress get_local_ip() {
return WiFi.localIP();
}

// Set the network configuration
void set_network_config(char* new_ssid, char* new_password, IPAddress new_ip_addr, IPAddress new_subnet_mask, IPAddress new_gateway, IPAddress new_dns1, IPAddress new_dns2) {
strncpy(ssid, new_ssid, sizeof(ssid));
strncpy(password, new_password, sizeof(password));
ip_addr = new_ip_addr;
subnet_mask = new_subnet_mask;
gateway = new_gateway;
dns1 = new_dns1;
dns2 = new_dns2;
}

// Get the network configuration
void get_network_config(char* ssid_out, char* password_out, IPAddress& ip_addr_out, IPAddress& subnet_mask_out, IPAddress& gateway_out, IPAddress& dns1_out, IPAddress& dns2_out) {
strncpy(ssid_out, ssid, sizeof(ssid));
strncpy(password_out, password, sizeof(password));
ip_addr_out = ip_addr;
subnet_mask_out = subnet_mask;
gateway_out = gateway;
dns1_out = dns1;
dns2_out = dns2;
}
