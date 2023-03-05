 #include <WiFi.h>
#include <WebServer.h>
#include <PJSIP.h>

// Pin definitions
#define ADC_PIN          36
#define PWM_LEFT_PIN     26
#define PWM_RIGHT_PIN    25

// Audio buffer settings
#define BUFFER_SIZE      512
#define BUFFER_THRESHOLD 256

// Network settings
#define DEFAULT_IP       IPAddress(192, 168, 1, 1)
#define DEFAULT_SUBNET   IPAddress(255, 255, 255, 0)
#define DEFAULT_GATEWAY  IPAddress(192, 168, 1, 254)
#define DEFAULT_DNS1     IPAddress(8, 8, 8, 8)
#define DEFAULT_DNS2     IPAddress(8, 8, 4, 4)
#define DEFAULT_PORT     5060
#define DEFAULT_USER     "user"
#define DEFAULT_PASS     "pass"

// PJSIP settings
#define MAX_CALLS        4
#define MAX_ACCS         1
#define MAX_PROXYS       1
#define RTP_PORT_START   20000
#define RTP_PORT_END     21000
#define JITTER_BUFFER_SIZE 2000

// Codec settings
#define DEFAULT_CODEC    PJSIP_CODEC_G722

// LCD settings
#define LCD_COLS         20
#define LCD_ROWS         4

// Web server settings
#define WEB_SERVER_PORT  80

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
static pjsip_transport_type_e transport_type = PJSIP_TRANSPORT_UDP;
static pjsip_codec_id codec_id = DEFAULT_CODEC;

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

// Initialize logging
pjsua_logging_config_default(&log_cfg);
log_cfg.console_level = 4;

// Initialize media
pjsua_media_config_default(&media_cfg);
media_cfg.clock_rate = 16000;
media_cfg.ec_tail_len = 0;
media_cfg.jb_size = JITTER_BUFFER_SIZE;

// Initialize transport
pjsua_transport_config_default(&trans_cfg);
trans_cfg.port = port;

// Initialize PJSUA
pjsua_create();
pjsua_init(&cfg, &log_cfg, &media_cfg);
pjsua_transport_config cfg_transport;
pjsua_transport_info info;
pjsua_transport_config_default(&cfg_transport);
cfg_transport.port = port;

// Set the transport type
if (transport_type == PJSIP_TRANSPORT_UDP) {
pjsua_transport_create(PJSIP_TRANSPORT_UDP, &cfg_transport, NULL);
} else if (transport_type == PJSIP_TRANSPORT_TCP) {
pjsua_transport_create(PJSIP_TRANSPORT_TCP, &cfg_transport, NULL);
} else if (transport_type == PJSIP_TRANSPORT_TLS) {
pjsua_transport_create(PJSIP_TRANSPORT_TLS, &cfg_transport, NULL);
} else {
Serial.println("Invalid transport type");
return;
}

// Add the account
pjsua_acc_add(pjsua_id(), pjsua_id(), NULL, 0, &acc_id);

// Set the account settings
pjsua_acc_config cfg_acc;
pjsua_acc_config_default(&cfg_acc);
cfg_acc.id = pj_str((char*)user.c_str());
cfg_acc.reg_uri = pj_str("sip:" DEFAULT_IP ":" + String(port));
cfg_acc.cred_count = 1;
cfg_acc.cred_info[0].realm = pj_str("asterisk");
cfg_acc.cred_info[0].scheme = pj_str("digest");
cfg_acc.cred_info[0].username = pj_str((char*)user.c_str());
cfg_acc.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
cfg_acc.cred_info[0].data = pj_str((char*)pass.c_str());

// Add the account to PJSUA
pjsua_acc_modify(acc_id, &cfg_acc, PJ_TRUE);
}

// Set up web server
void setup_web_server() {
server.on("/", handle_root);
server.on("/submit", handle_submit);
server.begin();
}

// Set up LCD display
void setup_lcd() {
lcd.begin(LCD_COLS, LCD_ROWS);
lcd.clear();
lcd.print("Asterisk Phone");
lcd.setCursor(0, 1);
lcd.print("IP: ");
lcd.print(WiFi.localIP());
}

// Main setup function
void setup() {
// Start serial communication
Serial.begin(115200);

// Set up Wi-Fi connection
setup_wifi();

// Set up PJSIP
setup_pjsip();

// Set up web server
setup_web_server();

// Set up LCD display
setup_lcd();

// Attach ADC interrupt
adcAttachInterrupt(ADC_PIN, adc_interrupt_handler, 4096);

// Set PWM pins
ledcAttachPin(PWM_LEFT_PIN, 1);
ledcAttachPin(PWM_RIGHT_PIN, 2);
ledcSetup(1, 16000, 8);
ledcSetup(2, 16000, 8);

// Initialize audio buffer
memset_call_state;

pjsua_logging_config_default(&log_cfg);
log_cfg.console_level = 4;

pjsua_media_config_default(&media_cfg);
media_cfg.clock_rate = 16000;
media_cfg.snd_clock_rate = 16000;
media_cfg.channel_count = 1;
media_cfg.quality = 10;
media_cfg.ec_tail_len = 0;
media_cfg.jb_max_size = JITTER_BUFFER_SIZE;
media_cfg.jb_init = JITTER_BUFFER_SIZE / 4;

pjsua_transport_config_default(&trans_cfg);
trans_cfg.port = port;

// Initialize PJSUA
pjsua_init(&cfg, &log_cfg, &media_cfg);
pjsua_transport_id transport_id;
pjsua_transport_config transport_cfg;

// Create transport
pjsua_transport_config_default(&transport_cfg);
transport_cfg.port = port;
transport_cfg.ipv6_enabled = false;

switch (transport_type) {
case PJSIP_TRANSPORT_UDP:
pjsua_transport_create(PJSIP_TRANSPORT_UDP, &transport_cfg, &transport_id);
break;
case PJSIP_TRANSPORT_TCP:
pjsua_transport_create(PJSIP_TRANSPORT_TCP, &transport_cfg, &transport_id);
break;
case PJSIP_TRANSPORT_TLS:
pjsua_transport_create(PJSIP_TRANSPORT_TLS, &transport_cfg, &transport_id);
break;
case PJSIP_TRANSPORT_WS:
pjsua_transport_create(PJSIP_TRANSPORT_WS, &transport_cfg, &transport_id);
break;
case PJSIP_TRANSPORT_WSS:
pjsua_transport_create(PJSIP_TRANSPORT_WSS, &transport_cfg, &transport_id);
break;
default:
pjsua_transport_create(PJSIP_TRANSPORT_UDP, &transport_cfg, &transport_id);
break;
}

// Add UDP transport
//pjsua_transport_config_default(&trans_cfg);
//trans_cfg.port = port;
//pjsua_transport_create(PJSIP_TRANSPORT_UDP, &trans_cfg, &transport_id);

// Initialize endpoint
pjsua_start();

// Add account
pjsua_acc_config acc_cfg;
pjsua_acc_config_default(&acc_cfg);
acc_cfg.id = pj_str(user.c_str());
acc_cfg.reg_uri = pj_str("sip:" + ip.toString() + ":" + String(port));
acc_cfg.cred_count = 1;
acc_cfg.cred_info[0].realm = pj_str("*");
acc_cfg.cred_info[0].scheme = pj_str("digest");
acc_cfg.cred_info[0].username = pj_str(user.c_str());
acc_cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
acc_cfg.cred_info[0].data = pj_str(pass.c_str());

pjsua_acc_add(&acc_cfg, PJ_TRUE, &acc_id);
}

// Set up the web server
void setup_web_server() {
server.on("/", HTTP_GET, handle_root);
server.on("/", HTTP_POST, handle_submit);
server.begin();
}

// Set up the LCD display
void setup_lcd() {
lcd.begin(LCD_COLS, LCD_ROWS);
lcd.print("IP: ");
lcd.print(ip);
lcd.setCursor(0, 1);
lcd.print("Port: ");
lcd.print(port);
}

void setup() {
// Initialize serial communication
Serial.begin(115200);

// Set up Wi-Fi connection
setup_wifi();

// Set up PJSIPvoid setup() {
Serial.begin(115200);
Serial.println("Starting setup...");

// Set up Wi-Fi connection
setup_wifi();

// Set up PJSIP library
setup_pjsip();

// Set up web server
setup_web_server();

// Set up LCD display
setup_lcd();

// Enable ADC interrupt
adcAttachInterrupt(ADC_PIN, adc_interrupt_handler, 4095);

Serial.println("Setup completed.");
}

void loop() {
// Handle web server requests
server.handleClient();

// Check if audio buffer is ready
if (audio_buffer_ready) {
// Play audio from buffer
audio_play(audio_buffer, BUFFER_SIZE, codec_id);// Reset audio buffer
audio_buffer_ready = false;
audio_buffer_index = 0;
}

// Check call state
if (call_id != PJSUA_INVALID_ID) {
pjsua_call_info ci;
pjsua_call_get_info(call_id, &ci);if (ci.state == PJSIP_INV_STATE_DISCONNECTED) {
  // Call is disconnected
  call_id = PJSUA_INVALID_ID;

  // Stop audio playback
  audio_stop();

  // Clear LCD display
  lcd.clear();

  // Show idle message
  lcd.setCursor(0, 0);
  lcd.print("ESP32 SIP Phone");

  // Show IP address
  lcd.setCursor(0, 1);
  lcd.print("IP: " + WiFi.localIP().toString());
}
}

// Delay to prevent CPU hogging
delay(10);
}

// Callback function for incoming calls
void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata) {
// Check if we can accept the call
if (pjsua_call_get_count() >= MAX_CALLS) {
// Reject the call
pjsua_call_hangup(call_id, PJSIP_SC_BUSY_HERE, NULL, NULL);
return;
}

// Answer the call
pjsua_call_answer(call_id, 200, NULL, NULL);

// Get call information
pjsua_call_info ci;
pjsua_call_get_info(call_id, &ci);

// Play ringback tone
audio_play_ringback();

// Show call information on LCD display
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("Incoming call");
lcd.setCursor(0, 1);
lcd.print("From: " + String(ci.remote_info.ptr->ai_dst_addr.sa.sa_data));
}

// Callback function for call media state changes
void on_call_media_state(pjsua_call_id call_id) {
// Get call information
pjsua_call_info ci;
pjsua_call_get_info(call_id, &ci);

// Stop ringback tone
audio_stop_ringback();

// Show call information on LCD display
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("Talking to");
lcd.setCursor(0, 1);
lcd.print(String(ci.remote_info.ptr->ai_dst_addr.sa.sa_data));
}

// Callback function for call state changes
void on_call_state(pjsua_call_id call_id, pjsip_event *e) {
// Get call information
pjsua_call_info ci;
pjsua_call_get_info(call_id, &ci);

// Show call information on LCD display
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("Talking to");
lcd.setCursor(0, 1);void on_call_media_state(pjsua_call_id call_id) {
  pjsua_call_info ci;
  pjsua_call_get_info(call_id, &ci);

  if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
    // Connect audio device to call
    pjsua_conf_connect(ci.conf_slot, 0);

    // Set up audio buffer
    pjsua_conf_set_jb_size(JITTER_BUFFER_SIZE);
    pjsua_conf_set_rx_level(1, -20);
    pjsua_conf_add_port(ci.conf_slot, &audio_cap, NULL, NULL);
  }
}

void on_call_state(pjsua_call_id call_id, pjsip_event *e) {
  pjsua_call_info ci;

  PJ_UNUSED_ARG(e);

  pjsua_call_get_info(call_id, &ci);

  if (ci.state == PJSIP_INV_STATE_DISCONNECTED) {
    // Disconnect audio device from call
    pjsua_conf_disconnect(ci.conf_slot, 0);

    // Reset audio buffer
    audio_buffer_ready = false;
    audio_buffer_index = 0;

    // Reset LCD display
    lcd.clear();
  }
}

void adc_interrupt_handler(void *arg) {
  uint16_t adc_value = analogRead(ADC_PIN);
  audio_buffer[audio_buffer_index++] = map(adc_value, 0, 4095, 0, 255);

  if (audio_buffer_index >= BUFFER_SIZE) {
    audio_buffer_index = 0;
    audio_buffer_ready = true;
  }
}

String get_current_time() {
  time_t now = time(nullptr);
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);
  char time_str[20];
  strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(time_str);
}

void handle_root() {
  String html = "<html><head><title>ESP32 SIP Client</title></head><body><h1>ESP32 SIP Client</h1><p>Current time: " + get_current_time() + "</p><form method='post' action='/submit'><table><tr><td>Protocol:</td><td><select name='protocol'><option value='udp'";
  if (transport_type == PJSIP_TRANSPORT_UDP) {
    html += " selected";
  }
  html += ">UDP</option><option value='tcp'";
  if (transport_type == PJSIP_TRANSPORT_TCP) {
    html += " selected";
  }
  html += ">TCP</option><option value='tls'";
  if (transport_type == PJSIP_TRANSPORT_TLS) {
    html += " selected";
  }
  html += ">TLS</option></select></td></tr><tr><td>Codec:</td><td><select name='codec'><option value='PCMU'";
  if (codec_id == PJSIP_CODEC_PCMU) {
    html += " selected";
  }
  html += ">PCMU</option><option value='PCMA'";
  if (codec_id == PJSIP_CODEC_PCMA) {
    html += " selected";
  }
  html += ">PCMA</option><option value='G722'";
  if (codec_id == PJSIP_CODEC_G722) {
    html += " selected";
  }
  html += ">G722</option></select></td></tr><tr><td>Server:</td><td><input type='text' name='server' value='" + server_address + "'></td></tr><tr><td>Port:</td><td><input type='text' name='port
}

// LCD initialization
void setup_lcd() {
lcd.begin(LCD_COLS, LCD_ROWS);
lcd.setCursor(0, 0);
lcd.print("Initializing...");
}

void setup() {
// Initialize serial communication
Serial.begin(9600);

// Initialize Wi-Fi connection
setup_wifi();

// Initialize PJSIP library
setup_pjsip();

// Initialize web server
setup_web_server();

// Initialize LCD display
setup_lcd();
}

void loop() {
// Handle incoming network packets
pjsua_handle_events(0);

// Handle audio buffer
if (audio_buffer_ready) {
// Send audio to the other party
pjsua_call_info call_info;
pjsua_call_get_info(call_id, &call_info);
pjsua_snd_media* snd_media = pjsua_call_get_snd_media(call_id);
pjmedia_port* port = pjsua_snd_media_get_port(snd_media);if (call_info.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
  pjmedia_frame frame;
  pjmedia_frame_init(&frame, codec_id, PJSUA_CALL_DEFAULT_AUDIO_FMT.clock_rate, 1, BUFFER_SIZE, 0);

  pj_status_t status = pjmedia_port_get_frame(port, &frame);
  if (status == PJ_SUCCESS) {
    memcpy(frame.buf, audio_buffer, BUFFER_SIZE);

    status = pjmedia_port_put_frame(port, &frame);
    if (status != PJ_SUCCESS) {
      Serial.println("Failed to send audio");
    }
  } else if (status != PJ_EPENDING) {
    Serial.println("Failed to get audio frame");
  }
  pjmedia_frame_free(&frame);
}

// Reset audio buffer
audio_buffer_ready = false;
audio_buffer_index = 0;
}
}

// Callback function for incoming calls
void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata) {
// Answer the call automatically
pjsua_call_answer(call_id, 200, NULL, NULL);
}

// Callback function for call media state changes
void on_call_media_state(pjsua_call_id call_id) {
pjsua_call_info call_info;
pjsua_call_get_info(call_id, &call_info);

if (call_info.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
Serial.println("Call established");// Start the ADC
pinMode(ADC_PIN, INPUT);
attachInterrupt(ADC_PIN, adc_interrupt_handler, RISING);

// Start the PWM
ledcSetup(0, 20000, 12);
ledcAttachPin(PWM_LEFT_PIN, 0);
ledcAttachPin(PWM_RIGHT_PIN, 0);

// Set LCD message
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("Call established");
lcd.setCursor(0, 1);
lcd.print(get_current_time());
}
}

// Callback function for call state changes
void on_call_state(pjsua_call_id call_id, pjsip_event *e) {
pjsua_call_info call_info;
pjsua_call_get_info(call_id, &call_info);

if (call_info.state == PJSIP_INV_STATE_DISCONNECTED) {
Serial.println("Call ended");// Stop the ADC and PWM
detachInterrupt(ADC_PIN);
ledcDetachPin(PWM_LEFT_PIN);
ledcDetachPin(PWM_RIGHT_PIN);

// Set LCD message
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("Call ended// Codec settings
#define DEFAULT_CODEC PJMEDIA_CODEC_G722

// Audio buffer
static uint8_t audio_buffer[BUFFER_SIZE];
static size_t audio_buffer_index = 0;

// Network settings
static IPAddress ip = DEFAULT_IP;
static uint16_t port = DEFAULT_PORT;
static String user = DEFAULT_USER;
static String pass = DEFAULT_PASS;

// PJSIP variables
static pjsua_acc_id acc_id;
static pjsua_call_id call_id = PJSUA_INVALID_ID;
static pjsip_transport_type_e transport_type = PJSIP_TRANSPORT_UDP;
static pjsip_codec_id codec_id = DEFAULT_CODEC;
static pjmedia_port *audio_port = NULL;
static pjmedia_port *file_port = NULL;
static pjmedia_snd_port *snd_port = NULL;

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
void on_file_data(pjmedia_port *port, void *user_data, pjmedia_event *event);
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
// Initialize PJSUA
pjsua_config cfg;
pjsua_logging_config log_cfg;
pjsua_media_config media_cfg;
pjsua_transport_config trans_cfg;

pjsua_config_default(&cfg);
cfg.cb.on_incoming_call = &on_incoming_call;
cfg.cb.on_call_media_state = &on_call_media_state;
cfg.cb.on_call_state = &on_call_state;

// Initialize logging
pjsua_logging_config_default(&log_cfg);
log_cfg.console_level = 4;

// Initialize media
pjsua_media_config_default(&media_cfg);
media_cfg.clock_rate = 16000;
media_cfg.snd_clock_rate = 16000;
media_cfg.channel_count = 1;
media_cfg.quality = 10;
media_cfg.ec_tail_len = 0;
media_cfg.jb_max_size = JITTER_BUFFER_SIZE;
media_cfg.jb_init = JITTER_BUFFER_SIZE / 4;
pjmedia_codec_g722_init(&media_cfg);

// Initialize transport
pjsua_transport_config_default(&trans_cfg);
trans_cfg.port = port;

// Initialize PJSUA
pjsua_create();
pjsua_init(&cfg, &log_cfg, &media_cfg);

// Set the transport type
if (transport_type == PJSIP_TRANSPORT_UDP) {
pjsua_transport_create(PJSIP_TRANSPORT_UDP, &trans_cfg, NULL);
} else if (transport_type == PJSIP_TRANSPORT_TCP) {
pjsua_transport_create(PJSIP_TRANSPORT_TCP, &trans_cfg, NULL);
} else if (transport_type == PJSIP_TRANSPORT_TLS) {// Handle root page request
void handle_root() {
// Set HTTP headers
server.sendHeader("Content-Type", "text/html");
server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
server.sendHeader("Pragma", "no-cache");
server.sendHeader("Expires", "0");

// Generate HTML page
String html = "<!DOCTYPE html><html><head><title>ESP32 SIP Phone</title></head><body>";
html += "<h1>ESP32 SIP Phone</h1>";
html += "<p>IP address: " + WiFi.localIP().toString() + "</p>";
html += "<form method='post' action='/submit'>";
html += "<label for='ip'>Server IP:</label><br>";
html += "<input type='text' id='ip' name='ip' value='" + ip.toString() + "'><br>";
html += "<label for='port'>Server port:</label><br>";
html += "<input type='number' id='port' name='port' value='" + String(port) + "'><br>";
html += "<label for='user'>User name:</label><br>";
html += "<input type='text' id='user' name='user' value='" + user + "'><br>";
html += "<label for='pass'>Password:</label><br>";
html += "<input type='password' id='pass' name='pass' value='" + pass + "'><br><br>";
html += "<input type='submit' value='Submit'>";
html += "</form>";
html += "</body></html>";

// Send response to client
server.send(200, "text/html", html);
}

// Handle form submission
void handle_submit() {
// Get form values
ip = IPAddress(server.arg("ip").toInt(), server.arg("ip").toInt(), server.arg("ip").toInt(), server.arg("ip").toInt());
subnet = IPAddress(255, 255, 255, 0);
gateway = IPAddress(ip[0], ip[1], ip[2], 1);
dns1 = IPAddress(8, 8, 8, 8);
dns2 = IPAddress(8, 8, 4, 4);
port = server.arg("port").toInt();
user = server.arg("user");
pass = server.arg("pass");

// Update LCD display
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("Connecting to Wi-Fi");

// Connect to Wi-Fi network
WiFi.mode(WIFI_STA);
WiFi.config(ip, gateway, subnet, dns1, dns2);
WiFi.begin();
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}

// Update LCD display
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("Connected to Wi-Fi");
lcd.setCursor(0, 1);
lcd.print("IP: ");
lcd.print(WiFi.localIP());

// Update PJSIP settings
setup_pjsip();

// Redirect to root page
server.sendHeader("Location", "/");
server.send(302);
}
 
