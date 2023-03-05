// Include required libraries
#include <WiFi.h>
#include <WebServer.h>
#include <PJSIP.h>

// Pin definitions
#define ADC_PIN 36
#define PWM_LEFT_PIN 26
#define PWM_RIGHT_PIN 25

// Audio buffer settings
#define BUFFER_SIZE 512
#define BUFFER_THRESHOLD 256

// Network settings
#define DEFAULT_IP IPAddress(192, 168, 1, 1)
#define DEFAULT_SUBNET IPAddress(255, 255, 255, 0)
#define DEFAULT_GATEWAY IPAddress(192, 168, 1, 254)
#define DEFAULT_DNS1 IPAddress(8, 8, 8, 8)
#define DEFAULT_DNS2 IPAddress(8, 8, 4, 4)
#define DEFAULT_PORT 5060
#define DEFAULT_USER "user"
#define DEFAULT_PASS "pass"

// PJSIP settings
#define MAX_CALLS 4
#define MAX_ACCS 1
#define MAX_PROXYS 1
#define RTP_PORT_START 20000
#define RTP_PORT_END 21000
#define JITTER_BUFFER_SIZE 2000

// LCD settings
#define LCD_COLS 20
#define LCD_ROWS 4

// Web server settings
#define WEB_SERVER_PORT 80

// Audio buffer
static uint8_t audio_buffer[BUFFER_SIZE];
static bool audio_buffer_ready = false;
static size_t audio_buffer_index = 0;

// Network settings
static IPAddress ip = DEFAULT_IP;
static IPAddress subnet = DEFAULT_SUBNET;
static IPAddress gateway = DEFAULT_GATEWAY;
static IPAddress dns1 = DEFAULT_DNS1;
static IPAddress dns2 = DEFAULT_DNS2;
static uint16_t port = DEFAULT_PORT;
static String user = DEFAULT_USER;
static String pass = DEFAULT_PASS;

// PJSIP variables
static pjsua_acc_id acc_id;
static pjsua_call_id call_id = PJSUA_INVALID_ID;

// LCD display
static LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Web server
static WebServer server(WEB_SERVER_PORT);

// Codecs
static const char *codecs[] = {"G.711", "G.722", "G.729"};

// Function prototypes
void setup_wifi();
void setup_pjsip();
void setup_web_server();
void setup_lcd();
void setup();
void loop();
void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata);
void on_call_media_state(pjsua_call_id call_id);
void on_call_state(pjsua_call_id call_id, pjsip_event *e);
void adc_interrupt_handler(void *arg);
String get_current_time();
void handle_root();
void handle_submit();

// Set up Wi-Fi connection
void setup_wifi() {
Serial.print("Connecting to Wi-Fi");
WiFi.begin();
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("Connected");
}

// Set up PJSIP library
void setup_pjsip() {
pjsua_config cfg;
pjsua_logging_config log_cfg;
pjsua_media_config media_cfg;
pjsua_transport_config trans_cfg;

// Initialize PJSUA
pjsua_config_default(&cfg);
cfg.cb.on_incoming_call = &on_incoming_call;
cfg.cb.on_call_media_state = &on_call_media_state;
cfg.cb.on_call_state = &on_call_state;
pjsua_logging_config_default(&log_cfg);
pjsuamedia_cfg.clock_rate = 800
 
 // User-configurable settings
enum Protocol {ASTERISK, SIP};
enum Codec {PCMU, PCMA, G722, OPUS};
Protocol protocol = SIP;
Codec codec = PCMU;

// Pin definitions
#define ADC_PIN 36
#define PWM_LEFT_PIN 26
#define PWM_RIGHT_PIN 25

// Audio buffer settings
#define BUFFER_SIZE 512
#define BUFFER_THRESHOLD 256

// Network settings
#define DEFAULT_IP IPAddress(192, 168, 1, 1)
#define DEFAULT_SUBNET IPAddress(255, 255, 255, 0)
#define DEFAULT_GATEWAY IPAddress(192, 168, 1, 254)
#define DEFAULT_DNS1 IPAddress(8, 8, 8, 8)
#define DEFAULT_DNS2 IPAddress(8, 8, 4, 4)
#define DEFAULT_PORT 5060
#define DEFAULT_USER "user"
#define DEFAULT_PASS "pass"

// PJSIP settings
#define MAX_CALLS 4
#define MAX_ACCS 1
#define MAX_PROXYS 1
#define RTP_PORT_START 20000
#define RTP_PORT_END 21000
#define JITTER_BUFFER_SIZE 2000

// LCD settings
#define LCD_COLS 20
#define LCD_ROWS 4

// Web server settings
#define WEB_SERVER_PORT 80

// Audio buffer
static uint8_t audio_buffer[BUFFER_SIZE];
static bool audio_buffer_ready = false;
static size_t audio_buffer_index = 0;

// Network settings
static IPAddress ip = DEFAULT_IP;
static IPAddress subnet = DEFAULT_SUBNET;
static IPAddress gateway = DEFAULT_GATEWAY;
static IPAddress dns1 = DEFAULT_DNS1;
static IPAddress dns2 = DEFAULT_DNS2;
static uint16_t port = DEFAULT_PORT;
static String user = DEFAULT_USER;
static String pass = DEFAULT_PASS;

// PJSIP variables
static pjsua_acc_id acc_id;
static pjsua_call_id call_id = PJSUA_INVALID_ID;

// LCD display
static LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Web server
static WebServer server(WEB_SERVER_PORT);

// Function prototypes
void setup_wifi();
void setup_pjsip();
void setup_web_server();
void setup_lcd();
void setup();
void loop();
void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata);
void on_call_media_state(pjsua_call_id call_id);
void on_call_state(pjsua_call_id call_id, pjsip_event *e);
void adc_interrupt_handler(void *arg);
String get_current_time();
void handle_root();
void handle_submit();

// Set up Wi-Fi connection
void setup_wifi() {
Serial.print("Connecting to Wi-Fi");
WiFi.begin();
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("Connected");
}

// Set up PJSIP library
void setup_pjsip() {
pjsua_config cfg;
pjsua_logging_config log_cfg;
pjsua_media_config media_cfg;
pjsua_transport_config trans_cfg;

// Initialize PJSUA
pjsua_config_default(&cfg);
cfg.cb.on_incoming_call = &on_incoming_call;
cfg.cb.on_call_media_state = &on_call_media_state;
cfg.cb.on_call_state = &on_call_state;
pjsua_logging_config_default(&log_cfg);
pjsuamedia_cfg.clock_rate = 8000;
media_cfg.snd_clock_rate =
 // Set up PJSIP library
void setup_pjsip() {
pjsua_config cfg;
pjsua_logging_config log_cfg;
pjsua_media_config media_cfg;
pjsua_transport_config trans_cfg;

// Initialize PJSUA
pjsua_config_default(&cfg);
cfg.cb.on_incoming_call = &on_incoming_call;
cfg.cb.on_call_media_state = &on_call_media_state;
cfg.cb.on_call_state = &on_call_state;
pjsua_logging_config_default(&log_cfg);

// Choose protocol
if (use_asterisk) {
cfg.transport_cfg.port = 5060; // Asterisk default port
} else {
cfg.transport_cfg.port = 5062; // SIP default port
}

// Choose codecs
if (use_g711) {
// G.711 codec
pjsua_codec_set_priority(PJMEDIA_CODEC_G711U, PJMEDIA_CODEC_PRIO_HIGH);
pjsua_codec_set_priority(PJMEDIA_CODEC_G711A, PJMEDIA_CODEC_PRIO_HIGH);
} else {
// Opus codec
pjsua_codec_set_priority(PJMEDIA_CODEC_OPUS, PJMEDIA_CODEC_PRIO_HIGH);
}

// Set up media config
pjsua_media_config_default(&media_cfg);
media_cfg.clock_rate = 8000;
media_cfg.snd_clock_rate = 44100;
media_cfg.ec_options = PJSUA_ECHO_USE_NOISE_SUPPRESSOR;

// Set up transport config
pjsua_transport_config_default(&trans_cfg);
trans_cfg.port = port;

// Choose TLS settings if necessary
if (use_tls) {
trans_cfg.tls_setting.method = PJSIP_TLSV1_0;
trans_cfg.tls_setting.verify_client = PJSIP_TLS_VERIFY_DISABLED;
}

// Initialize PJSUA
status = pjsua_create();
PJ_ASSERT_RETURN(status == PJ_SUCCESS, "Error initializing PJSUA",);
pjsua_set_null_snd_dev();
status = pjsua_init(&cfg, &log_cfg, &media_cfg);
PJ_ASSERT_RETURN(status == PJ_SUCCESS, "Error initializing PJSUA",);
status = pjsua_transport_create(use_tls ? PJSIP_TRANSPORT_TLS : PJSIP_TRANSPORT_UDP, &trans_cfg, NULL);
PJ_ASSERT_RETURN(status == PJ_SUCCESS, "Error creating transport",);
pjsua_start();

// Configure account
pjsua_acc_id acc_id;
pjsua_acc_config acc_cfg;
pjsua_acc_config_default(&acc_cfg);
acc_cfg.id = pj_str("sip:" DEFAULT_USER "@" DEFAULT_IP);
acc_cfg.reg_uri = pj_str("sip:" DEFAULT_IP);
acc_cfg.cred_count = 1;
acc_cfg.cred_info[0].realm = pj_str("asterisk");
acc_cfg.cred_info[0].scheme = pj_str("digest");
acc_cfg.cred_info[0].username = pj_str(DEFAULT_USER);
acc_cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
acc_cfg.cred_info[0].data = pj_str(DEFAULT_PASS);
status = pjsua_acc_add(&acc_cfg, PJ_TRUE, &acc_id);
PJ_ASSERT_RETURN(status == PJ_SUCCESS, "Error adding account",);
}

// Set up web server
void setup_web_server() {
server.on("/", HTTP_GET, handle_root);
server.on("/submit", HTTP_POST, handle_submit);
server.begin();// Function to handle form submission
void handle_submit() {
// Parse form data
String protocol = server.arg("protocol");
String ip_str = server.arg("ip");
String subnet_str = server.arg("subnet");
String gateway_str = server.arg("gateway");
String dns1_str = server.arg("dns1");
String dns2_str = server.arg("dns2");
String port_str = server.arg("port");
String user_str = server.arg("user");
String pass_str = server.arg("pass");
String codec_str = server.arg("codec");

// Validate input
IPAddress new_ip, new_subnet, new_gateway, new_dns1, new_dns2;
uint16_t new_port;
Codec new_codec;
bool success = true;

if (!new_ip.fromString(ip_str) || !new_subnet.fromString(subnet_str) || !new_gateway.fromString(gateway_str) ||
!new_dns1.fromString(dns1_str) || !new_dns2.fromString(dns2_str)) {
server.send(400, "text/plain", "Invalid input");
success = false;
}

if (!port_str.toInt()) {
server.send(400, "text/plain", "Invalid input");
success = false;
} else {
new_port = port_str.toInt();
}

if (protocol != "asterisk" && protocol != "SIP") {
server.send(400, "text/plain", "Invalid input");
success = false;
}

if (codec_str == "G711") {
new_codec = Codec::G711;
} else if (codec_str == "G722") {
new_codec = Codec::G722;
} else if (codec_str == "OPUS") {
new_codec = Codec::OPUS;
} else {
server.send(400, "text/plain", "Invalid input");
success = false;
}

if (!success) {
return;
}

// Update settings
ip = new_ip;
subnet = new_subnet;
gateway = new_gateway;
dns1 = new_dns1;
dns2 = new_dns2;
port = new_port;
user = user_str;
pass = pass_str;
current_protocol = protocol;
current_codec = new_codec;

// Set up PJSIP
if (current_protocol == "asterisk") {
setup_asterisk();
} else {
setup_pjsip();
}

// Set up network settings
IPAddress dns[] = {dns1, dns2};
WiFi.config(ip, gateway, subnet, dns);

// Send response
String html = "<html><body><h2>Settings updated</h2><p>IP Address: ";
html += ip.toString() + "</p><p>Subnet Mask: " + subnet.toString() + "</p><p>";
html += "Default Gateway: " + gateway.toString() + "</p><p>Primary DNS: " + dns1.toString() + "</p><p>";
html += "Secondary DNS: " + dns2.toString() + "</p></body></html>";
server.send(200, "text/html", html);
}

// Main function
int main() {
setup();

while (true) {
loop();
}

return 0;
}
 
