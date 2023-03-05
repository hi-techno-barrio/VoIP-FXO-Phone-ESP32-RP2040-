#include "web_server.h"

// Web server
WiFiServer server(80);

// Initialize the web server
void init_web_server() {
server.begin();
}

// Handle incoming HTTP requests
void handle_http_request() {
WiFiClient client = server.available();
if (client) {
String current_line = "";
while (client.connected()) {
if (client.available()) {
char c = client.read();
if (c == '\n') {
if (current_line.length() == 0) {
break;
} else {
current_line = "";
}
} else if (c != '\r') {
current_line += c;
}    if (current_line.endsWith("GET /network_settings")) {
      handle_network_settings_request(client);
      break;
    } else if (current_line.endsWith("GET /save_network_settings")) {
      handle_save_network_settings_request(client);
      break;
    }
  }
}
client.stop();
}
}

// Handle a request for network settings
void handle_network_settings_request(WiFiClient& client) {
char ssid[64];
char password[64];
IPAddress ip_addr;
IPAddress subnet_mask;
IPAddress gateway;
IPAddress dns1;
IPAddress dns2;

get_network_config(ssid, password, ip_addr, subnet_mask, gateway, dns1, dns2);

String response = "<html><body><h1>Network Settings</h1>";
response += "<form method='get' action='/save_network_settings'>";
response += "<label for='ssid'>SSID:</label>";
response += "<input type='text' name='ssid' value='";
response += ssid;
response += "'><br>";
response += "<label for='password'>Password:</label>";
response += "<input type='password' name='password' value='";
response += password;
response += "'><br>";
response += "<label for='ip'>IP Address:</label>";
response += "<input type='text' name='ip' value='";
response += ip_addr.toString();
response += "'><br>";
response += "<label for='subnet_mask'>Subnet Mask:</label>";
response += "<input type='text' name='subnet_mask' value='";
response += subnet_mask.toString();
response += "'><br>";
response += "<label for='gateway'>Gateway:</label>";
response += "<input type='text' name='gateway' value='";
response += gateway.toString();
response += "'><br>";
response += "<label for='dns1'>DNS 1:</label>";
response += "<input type='text' name='dns1' value='";
response += dns1.toString();
response += "'><br>";
response += "<label for='dns2'>DNS 2:</label>";
response += "<input type='text' name='dns2' value='";
response += dns2.toString();
response += "'><br>";
response += "<input type='submit' value='Save'>";
response += "</form></body></html>";

client.println("HTTP/1.1 200 OK");
client.println("Content-Type: text/html");
client.println("Connection: close");
client.println("");
client.println(response);
}

// Handle a request to save network settings
void handle_save_network_settings_request(WiFiClient& client) {
String ssid;
String password;
String ip_str;
String subnet_mask_str;
String gateway_str;
String dns1_str;
String dns2_str;

String request = client.readString();
int ssid_index = request.indexOf("ssid=");
int
int password_index = request.indexOf("password=");
int ip_index = request.indexOf("ip=");
int subnet_mask_index = request.indexOf("subnet_mask=");
int gateway_index = request.indexOf("gateway=");
int dns1_index = request.indexOf("dns1=");
int dns2_index = request.indexOf("dns2=");

if (ssid_index != -1) {
ssid = request.substring(ssid_index + 5, password_index - 1);
}
if (password_index != -1) {
password = request.substring(password_index + 9, ip_index - 1);
}
if (ip_index != -1) {
ip_str = request.substring(ip_index + 3, subnet_mask_index - 1);
}
if (subnet_mask_index != -1) {
subnet_mask_str = request.substring(subnet_mask_index + 12, gateway_index - 1);
}
if (gateway_index != -1) {
gateway_str = request.substring(gateway_index + 8, dns1_index - 1);
}
if (dns1_index != -1) {
dns1_str = request.substring(dns1_index + 5, dns2_index - 1);
}
if (dns2_index != -1) {
dns2_str = request.substring(dns2_index + 5);
}

// Parse IP addresses
IPAddress ip;
IPAddress subnet_mask;
IPAddress gateway;
IPAddress dns1;
IPAddress dns2;

if (!ip.fromString(ip_str)) {
client.println("Invalid IP address.");
return;
}

if (!subnet_mask.fromString(subnet_mask_str)) {
client.println("Invalid subnet mask.");
return;
}

if (!gateway.fromString(gateway_str)) {
client.println("Invalid gateway.");
return;
}

if (!dns1.fromString(dns1_str)) {
client.println("Invalid DNS 1.");
return;
}

if (!dns2.fromString(dns2_str)) {
client.println("Invalid DNS 2.");
return;
}

// Save network configuration
set_network_config(ssid.c_str(), password.c_str(), ip, subnet_mask, gateway, dns1, dns2);

// Redirect to network settings page
client.println("HTTP/1.1 301 Moved Permanently");
client.println("Location: /network_settings");
client.println("Connection: close");
client.println("");
}

// Handle incoming web requests
void handle_web_request() {
handle_http_request();
}

// Get the web server IP address
IPAddress get_web_server_ip() {
return WiFi.localIP();
}
