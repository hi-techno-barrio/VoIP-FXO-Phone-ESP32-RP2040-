#include <WiFi.h>
#include <WebServer.h>
#include <PJSIP.h>

// Pin definitions
#define ADC_PIN         36
#define PWM_LEFT_PIN    26
#define PWM_RIGHT_PIN   25

// Audio buffer settings
#define BUFFER_SIZE         512
#define BUFFER_THRESHOLD    256

// Network settings
#define DEFAULT_IP          IPAddress(192, 168, 1, 1)
#define DEFAULT_SUBNET      IPAddress(255, 255, 255, 0)
#define DEFAULT_GATEWAY     IPAddress(192, 168, 1, 254)
#define DEFAULT_DNS1        IPAddress(8, 8, 8, 8)
#define DEFAULT_DNS2        IPAddress(8, 8, 4, 4)
#define DEFAULT_PORT        5060
#define DEFAULT_USER        "user"
#define DEFAULT_PASS        "pass"
#define DEFAULT_CODEC       "G.711"

// PJSIP settings
#define MAX_CALLS           4
#define MAX_ACCS            1
#define MAX_PROXYS          1
#define RTP_PORT_START      20000
#define RTP_PORT_END        21000
#define JITTER_BUFFER_SIZE  2000

// LCD settings
#define LCD_COLS            20
#define LCD_ROWS            4

// Web server settings
#define WEB_SERVER_PORT     80

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
static String codec = DEFAULT_CODEC;

// PJSIP variables
static pjsua_acc_id acc_id;
static pjsua_call_id call_id = PJSUA_INVALID_ID;
static pjsua_transport_id transport_id;

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
  pjsua_acc_config acc_cfg;

  // Initialize PJSUA
  pjsua_config_default(&cfg);
  cfg.cb.on_incoming_call = &on_incoming_call;
  cfg.cb.on_call_media_state = &on_call_media_state;
  cfg.cb.on_call_state = &on_call_state;

// Initialize logging
pjsua_logging_config_default(&log_cfg);
log_cfg.console_level = 4;

// Initialize media settings
pjsua_media_config_default(&media_cfg);
media_cfg.clock_rate = 8000;
media_cfg.quality = 5;
media_cfg.ec_tail_len = 0;
media_cfg.jb_size = JITTER_BUFFER_SIZE;
media_cfg.no_vad = PJ_TRUE;
media_cfg.snd_rec_latency = PJMEDIA_SND_DEFAULT_REC_LATENCY;

// Initialize transport settings
pjsua_transport_config_default(&trans_cfg);
trans_cfg.port = port;

// Initialize account settings
pjsua_acc_config_default(&acc_cfg);
acc_cfg.id = pj_str("sip:" + user + "@sip.example.com");
acc_cfg.reg_uri = pj_str("sip:sip.example.com");
acc_cfg.auth_cred[0].realm = pj_str("sip.example.com");
acc_cfg.auth_cred[0].username = pj_str(user.c_str());
acc_cfg.auth_cred[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
acc_cfg.auth_cred[0].data = pj_str(pass.c_str());

// Initialize PJSUA
pj_status_t status = pjsua_init(&cfg, &log_cfg, &media_cfg);
PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

// Add UDP transport
status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &trans_cfg, &transport_id);
PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

// Start PJSUA
status = pjsua_start();
PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

// Add account
status = pjsua_acc_add(&acc_cfg, PJ_TRUE, &acc_id);
PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);
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
lcd.print("Initializing...");
}

void setup() {
Serial.begin(9600);

setup_wifi();
setup_pjsip();
setup_web_server();
setup_lcd();

// Set up ADC
analogReadResolution(12);
attachInterrupt(ADC_PIN, adc_interrupt_handler, FALLING);
}

void loop() {
// Handle incoming web requests
server.handleClient();

// Check if audio buffer is ready
if (audio_buffer_ready) {
// Play audio buffer
pjsua_play_file(call_id, "/audio.wav", 0, PJMEDIA_FILE_NO_LOOP);// Reset audio buffer
audio_buffer_ready = false;
audio_buffer_index = 0;.on_call_state = &on_call_state;

// Initialize logging
pjsua_logging_config_default(&log_cfg);
log_cfg.console_level = 4;

// Initialize media settings
pjsua_media_config_default(&media_cfg);
media_cfg.clock_rate = 8000;
media_cfg.quality = 5;
media_cfg.ec_tail_len = 0;
media_cfg.jb_size = JITTER_BUFFER_SIZE;
media_cfg.no_vad = PJ_TRUE;
media_cfg.snd_rec_latency = PJMEDIA_SND_DEFAULT_REC_LATENCY;

// Initialize transport settings
pjsua_transport_config_default(&trans_cfg);
trans_cfg.port = port;

// Initialize account settings
pjsua_acc_config_default(&acc_cfg);
acc_cfg.id = pj_str("sip:" + user + "@sip.example.com");
acc_cfg.reg_uri = pj_str("sip:sip.example.com");
acc_cfg.auth_cred[0].realm = pj_str("sip.example.com");
acc_cfg.auth_cred[0].username = pj_str(user.c_str());
acc_cfg.auth_cred[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
acc_cfg.auth_cred[0].data = pj_str(pass.c_str());

// Initialize PJSUA
pj_status_t status = pjsua_init(&cfg, &log_cfg, &media_cfg);
PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

// Add UDP transport
status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &trans_cfg, &transport_id);
PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

// Start PJSUA
status = pjsua_start();
PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

// Add account
status = pjsua_acc_add(&acc_cfg, PJ_TRUE, &acc_id);
PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);
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
lcd.print("Initializing...");
}

void setup() {
Serial.begin(9600);

setup_wifi();
setup_pjsip();
setup_web_server();
setup_lcd();

// Set up ADC
analogReadResolution(12);
attachInterrupt(ADC_PIN, adc_interrupt_handler, FALLING);
}

void loop() {
// Handle incoming web requests
server.handleClient();

// Check if audio buffer is ready
if (audio_buffer_ready) {
// Play audio buffer
pjsua_play_file(call_id, "/audio.wav", 0, PJMEDIA_FILE_NO_LOOP);

javascript
Copy code
// Reset audio buffer
audio_buffer_ready = false;
audio_buffer_index = 0;
}
}

// Incoming call handler
void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata) {
// Answer call
pjsua_call_answer(call_id, 200, NULL, NULL);

// Store call ID
call_id = call_id;
}

// Call media state handler
void on_call_media_state(pjsua_call_id call_id) {
pjsua_conf_port_id conf_port_id;
pjsua_conf_port_id send_port_id;
pjsua_conf_port_id recv_port_id;

// Connect call audio to sound device
pjsua_conf_add_port(pjsua_get_conf_info()->default_audio_transport_id, audio_buffer, BUFFER_SIZE, 8000, 1, &audio_player.port_id);
 /*
0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000  
// Connect audio player to call audio
pjsua_conf_connect(audio_player.port_id, call_info.conf_slot);

// Set audio player volume to maximum
pjmedia_aud_dev_default_set_setting(audio_player.dev_id, PJMEDIA_AUD_DEV_CAP_OUTPUT_VOLUME, 255);

00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000  
*/

  // Callback function for incoming calls
void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata) {
pjsua_call_info ci;
pjsua_call_get_info(call_id, &ci);

if (pjsua_call_is_active(call_id)) {
pjsua_call_answer(call_id, PJSIP_SC_BUSY_HERE, NULL, NULL);
return;
}

// Answer the call
pjsua_call_answer(call_id, 200, NULL, NULL);
}

// Callback function for call media state
void on_call_media_state(pjsua_call_id call_id) {
pjsua_call_info ci;
pjsua_call_get_info(call_id, &ci);

if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
// Activate audio device
pjsua_conf_connect(ci.conf_slot, 0);
pjsua_conf_connect(0, ci.conf_slot);
}
}

// Callback function for call state
void on_call_state(pjsua_call_id call_id, pjsip_event *e) {
pjsua_call_info ci;
pjsua_call_get_info(call_id, &ci);

if (ci.state == PJSIP_INV_STATE_DISCONNECTED) {
// Stop audio device
pjsua_conf_disconnect(0, ci.conf_slot);
pjsua_conf_disconnect(ci.conf_slot, 0);
call_id = PJSUA_INVALID_ID;
}
}

// ADC interrupt handler
void adc_interrupt_handler(void *arg) {
uint16_t value = analogRead(ADC_PIN);
audio_buffer[audio_buffer_index++] = value & 0xFF;
audio_buffer[audio_buffer_index++] = (value >> 8) & 0xFF;

if (audio_buffer_index >= BUFFER_SIZE) {
audio_buffer_index = 0;
audio_buffer_ready = true;
}
}

// Get current time as string
String get_current_time() {
char buffer[20];
time_t now = time(nullptr);
strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&now));
return String(buffer);
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
lcd.print("Starting up...");
}

// Set up peripherals and libraries
void setup() {
// Serial communication
Serial.begin(115200);
while (!Serial);

// Wi-Fi
setup_wifi();

// PJSIP
setup_pjsip();

// ADC interrupt
attachInterrupt(ADC_PIN, adc_interrupt_handler, CHANGE);

// LCD display
setup_lcd();

// Web server
setup_web_server();
}

// Main loop
void loop() {
// Handle web server requests
server.handleClient();

// Process audio buffer
if (audio_buffer_ready) {
audio_buffer_ready = false;// Check if there is an active call
if (call_id != PJSUA_INVALID_ID) {
  // Send audio packet
  pjsua_call_info ci;
  pjsua_call_get_info(call_id, &ci);
  pjmedia_frame frame;
  pj_status_t status = pjmedia_frame_init(&frame, ci.media[0].clock_rate, BUFFER_SIZE / 2, 1, 16, 0);
  if (status != PJ_SUCCESS) {
    Serial.println("Error initializing audio frame// Set up web server
void setup_web_server() {
server.on("/", handle_root);
server.on("/submit", handle_submit);
server.begin();
}

// Set up LCD display
void setup_lcd() {
lcd.begin(LCD_COLS, LCD_ROWS);
lcd.print("Initializing...");
}

// Main setup function
void setup() {
// Set up serial connection
Serial.begin(115200);
while (!Serial);

// Set up Wi-Fi connection
setup_wifi();

// Set up PJSIP library
setup_pjsip();

// Set up web server
setup_web_server();

// Set up LCD display
setup_lcd();
}

// Main loop function
void loop() {
// Check for incoming SIP messages
pjsua_handle_events(0);

// Check for audio buffer threshold
if (!audio_buffer_ready && audio_buffer_index >= BUFFER_THRESHOLD) {
audio_buffer_ready = true;
digitalWrite(LED_BUILTIN, HIGH);
}

// Check for audio buffer overflow
if (audio_buffer_index >= BUFFER_SIZE) {
audio_buffer_index = 0;
audio_buffer_ready = false;
digitalWrite(LED_BUILTIN, LOW);
}

// Read ADC value and store in audio buffer
uint16_t adc_value = analogRead(ADC_PIN);
audio_buffer[audio_buffer_index++] = (uint8_t) (adc_value >> 2);

// Write audio buffer to PJSIP library
if (audio_buffer_ready) {
pjsua_conf_port_id conf_port_id;
pjsua_conf_create_port(&conf_port_id, PJMEDIA_CONF_NO_DEVICE, PJMEDIA_AUDIO_FMT_PCMU, 1, 8000, 1);
pjsua_port_audio_fill(conf_port_id, audio_buffer, BUFFER_SIZE);
pjsua_conf_port_destroy(conf_port_id);

audio_buffer_index = 0;
audio_buffer_ready = false;
digitalWrite(LED_BUILTIN, LOW);}

// Handle web server requests
server.handleClient();
}

// Handle incoming call event
void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata) {
// Only allow one call at a time
if (pjsua_call_get_count() >= MAX_CALLS) {
pjsua_call_hangup(call_id, 486, NULL, NULL);
return;
}

// Answer incoming call
pjsua_call_answer(call_id, 200, NULL, NULL);
}

// Handle call media state event
void on_call_media_state(pjsua_call_id call_id) {
pjsua_call_info call_info;
pjsua_call_get_info(call_id, &call_info);

// Attach sound device to call
pjsua_conf_connect(call_info.conf_slot, 0);
pjsua_conf_connect(0, call_info.conf_slot);
}

// Handle call state event
void on_call_state(pjsua_call_id call_id, pjsip_event *e) {
pjsua_call_info call_info;
pjsua_call_get_info(call_id, &call_info);

// Display call state on LCD
lcd.setCursor(0, 0);
lcd.print(" ");
lcd.setCursor(0, 0);
lcd.print(call_info.state_text.ptr);

// Hang up call if it has been terminated
if (call_info.state == PJSIP_INV_STATE_DISCONNECTED) {
pjsua_call_hangup(call_id, 0, NULL, NULL);
lcd.setCursor(0, 0);
lcd.print("Disconnected ");static void handle_submit() {
// Check if request was a POST request
if (server.method() != HTTP_POST) {
server.send(405, "text/plain", "Method Not Allowed");
return;
}

// Get form values
String new_ip = server.arg("ip");
String new_subnet = server.arg("subnet");
String new_gateway = server.arg("gateway");
String new_dns1 = server.arg("dns1");
String new_dns2 = server.arg("dns2");
String new_port = server.arg("port");
String new_user = server.arg("user");
String new_pass = server.arg("pass");
String new_codec = server.arg("codec");

// Update network settings
ip = IPAddress(new_ip);
subnet = IPAddress(new_subnet);
gateway = IPAddress(new_gateway);
dns1 = IPAddress(new_dns1);
dns2 = IPAddress(new_dns2);
port = new_port.toInt();
user = new_user;
pass = new_pass;
codec = new_codec;

// Disconnect current account
pjsua_acc_set_registration(acc_id, PJ_FALSE);
pjsua_acc_del(acc_id);

// Set up new account
pjsua_acc_config acc_cfg;
pjsua_acc_config_default(&acc_cfg);
acc_cfg.id = pj_str(user.c_str());
acc_cfg.reg_uri = pj_str("sip:" + ip.toString() + ":" + String(port).c_str());
acc_cfg.cred_count = 1;
acc_cfg.cred_info[0].realm = pj_str("*");
acc_cfg.cred_info[0].username = pj_str(user.c_str());
acc_cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
acc_cfg.cred_info[0].data = pj_str(pass.c_str());
pjsua_acc_add(&acc_cfg, PJ_TRUE, &acc_id);

// Set up SIP transport
pjsua_transport_config trans_cfg;
pjsua_transport_config_default(&trans_cfg);
trans_cfg.port = port;
pjsua_transport_create(PJSIP_TRANSPORT_UDP, &trans_cfg, NULL);

// Set up media configuration
pjsua_media_config media_cfg;
pjsua_media_config_default(&media_cfg);
media_cfg.clock_rate = 8000;
media_cfg.snd_clock_rate = 16000;
media_cfg.ec_tail_len = 0;

// Set up call settings
pjsua_call_setting call_setting;
pjsua_call_setting_default(&call_setting);
call_setting.aud_cnt = 1;
call_setting.vid_cnt = 0;

// Start the pjsua system
pjsua_start();

// Add the SIP account
pjsua_acc_add(&acc_cfg, PJ_TRUE, &acc_id);

// Set the account to online
pjsua_acc_set_online_status(acc_id, PJ_TRUE);

// Send response
server.send(200, "text/plain", "Settings updated");
}

// Set up the device
void setup() {
// Set up serial communication
Serial.begin(115200);

// Set up Wi-Fi
setup_wifi();

// Set up PJSIP
setup_pjsip();

// Set up the web server
setup_web_server();

// Set up the LCD
setup_lcd();
}

// Main loop
void loop() {
// Check if audio buffer is ready
if (audio_buffer_ready) {
// Play audio
analogWrite(PWM_LEFT_PIN, audio_buffer[audio_buffer_index++]);void on_call_state(pjsua_call_id call_id, pjsip_event *e) {
pjsua_call_info ci;
pjsua_call_get_info(call_id, &ci);

if (ci.state == PJSIP_INV_STATE_DISCONNECTED) {
lcd.clear();
lcd.print("Call ended");
call_id = PJSUA_INVALID_ID;
audio_buffer_ready = false;
}
}

// ADC interrupt handler
void IRAM_ATTR adc_interrupt_handler(void *arg) {
uint32_t adc_reading = analogRead(ADC_PIN);
audio_buffer[audio_buffer_index++] = (uint8_t)(adc_reading >> 2);

if (audio_buffer_index == BUFFER_SIZE) {
audio_buffer_index = 0;
audio_buffer_ready = true;
}
}

// Get current time as string
String get_current_time() {
time_t now = time(nullptr);
char buf[sizeof "2022-12-31 23:59:59"];
strftime(buf, sizeof buf, "%Y-%m-%d %H:%M:%S", localtime(&now));
return String(buf);
}

// Set up web server
void setup_web_server() {
server.on("/", HTTP_GET, handle_root);
server.on("/submit", HTTP_POST, handle_submit);
server.begin();
}

// Set up LCD display
void setup_lcd() {
lcd.begin(LCD_COLS, LCD_ROWS);
lcd.print("Ready");
}

// Main setup function
void setup() {
Serial.begin(115200);

// Set up Wi-Fi connection
setup_wifi();

// Set up PJSIP library
setup_pjsip();

// Set up web server
setup_web_server();

// Set up LCD display
setup_lcd();

// Set up ADC
analogReadResolution(12);
analogSetPinAttenuation(ADC_PIN, ADC_11db);
analogSetWidth(9);
adcAttachPin(ADC_PIN);
adcAttachInterrupt(ADC_PIN, adc_interrupt_handler, nullptr);
}

// Main loop function
void loop() {
// Handle web server requests
server.handleClient();

// Handle PJSIP events
pjsua_event event;
while (pjsua_get_event(&event, 0) == PJ_SUCCESS) {
pjsua_handle_events(&event);
}

// Handle incoming audio data
if (audio_buffer_ready && call_id != PJSUA_INVALID_ID) {
pjsua_call_info ci;
pjsua_call_get_info(call_id, &ci);if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
  pjmedia_port *media_port = pjsua_call_get_audio_send_port(call_id);
  pjmedia_frame frame;
  pjmedia_frame_init(&frame, media_port->info.samples_per_frame, media_port->info.bits_per_sample, media_port->info.channel_count);

  memcpy(frame.buf, audio_buffer, BUFFER_SIZE);
  pjmedia_port_put_frame(media_port, &frame);
}

audio_buffer_ready = false;
}
}

// Handle root page
void handle_root() {
String html = "<html><body><h1>ESP32 SIP Phone</h1>";
html += "<form action='/submit' method='POST'>";
html += "IP Address: <input type='text' name='ip' value='" + ip.toString() + "'><br>";
html += "Subnet Mask: <input type='text' name='subnet' value='" + subnet.toString() + "'><br>";
html += "Gateway: <input type='text' name='gateway' value='" + gateway.toString() +"'><br>";
html += "Primary DNS: <input type='text' name='dns1' value='" + dns1.toString() + "'><br>";
html += "Secondary DNS: <input type='text' name='dns2' value='" + dns2.toString() + "'><br>";
html += "SIP Port: <input type='text' name='port' value='" + String(port) + "'><br>";
html += "Username: <input type='text' name='user' value='" + user + "'><br>";
html += "Password: <input type='password' name='pass' value='" + pass + "'><br>";
html += "Codec: <select name='codec'>";
html += "<option value='G.711'" + (codec == "G.711" ? " selected" : "") + ">G.711</option>";
html += "<option value='G.729'" + (codec == "G.729" ? " selected" : "") + ">G.729</option>";
html += "<option value='opus'" + (codec == "opus" ? " selected" : "") + ">opus</option>";
html += "</select><br>";
html += "<input type='submit' value='Submit'>";
html += "</form></body></html>";
server.send(200, "text/html", html);
}

// Handle form submission
void handle_submit() {
// Update network settings
ip = IPAddress(server.arg("ip").toInt());
subnet = IPAddress(server.arg("subnet").toInt());
gateway = IPAddress(server.arg("gateway").toInt());
dns1 = IPAddress(server.arg("dns1").toInt());
dns2 = IPAddress(server.arg("dns2").toInt());
port = server.arg("port").toInt();
user = server.arg("user");
pass = server.arg("pass");
codec = server.arg("codec");

// Set up PJSIP with new settings
setup_pjsip();

// Send response
server.sendHeader("Location", "/");
server.send(302, "text/plain", "");
}

void setup() {
// Initialize serial port
Serial.begin(115200);

// Set up Wi-Fi connection
setup_wifi();

// Set up PJSIP library
setup_pjsip();

// Set up web server
setup_web_server();

// Set up LCD display
setup_lcd();
}

void loop() {
// Handle web server requests
server.handleClient();

// Update LCD display
String status = "IP: " + WiFi.localIP().toString();
if (call_id != PJSUA_INVALID_ID) {
status += " - Call in progress";
}
lcd.setCursor(0, 0);
lcd.print(status);
if (audio_buffer_ready) {
lcd.setCursor(0, 1);
lcd.print(get_current_time());
lcd.setCursor(0, 2);
lcd.print("Audio buffer ready");
lcd.setCursor(0, 3);
lcd.print("Buffer index: " + String(audio_buffer_index));
audio_buffer_ready = false;
}

// Check if audio buffer needs to be filled
if (pjsua_get_conf_port_info(0, &conf_port_info) == PJ_SUCCESS) {
if (conf_port_info.buffer_status == PJMEDIA_CONF_BUFFER_UNDERFLOW) {
uint16_t adc_value = analogRead(ADC_PIN);
audio_buffer[audio_buffer_index] = map(adc_value, 0, 4095, 0, 255);
audio_buffer_index++;
if (audio_buffer_index == BUFFER4095, 0, 255);
audio_buffer_index++;
if (audio_buffer_index == BUFFER_SIZE) {
audio_buffer_ready = true;
audio_buffer_index = 0;
}
}

// Callback function for incoming calls
void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata) {
pjsua_call_info call_info;

// Get call info
pjsua_call_get_info(call_id, &call_info);

// Answer the call automatically
pjsua_call_answer(call_id, 200, NULL, NULL);
}

// Callback function for call media state change
void on_call_media_state(pjsua_call_id call_id) {
pjsua_call_info call_info;
pjsua_call_get_info(call_id, &call_info);

// Check if audio media is active
if (call_info.media[0].dir & PJMEDIA_DIR_DECODING) {
// Connect audio media to sound device
pjsua_conf_connect(call_info.conf_slot, 0);
pjsua_conf_connect(0, call_info.conf_slot);
}
}

// Callback function for call state change
void on_call_state(pjsua_call_id call_id, pjsip_event *e) {
pjsua_call_info call_info;

// Get call info
pjsua_call_get_info(call_id, &call_info);

// Check if call has been disconnected
if (call_info.state == PJSIP_INV_STATE_DISCONNECTED) {
// Hang up call
pjsua_call_hangup(call_id, 0, NULL, NULL);
call_id = PJSUA_INVALID_ID;
}
}

// Interrupt handler for ADC
void adc_interrupt_handler(void *arg) {
uint32_t adc_value = analogRead(ADC_PIN);
audio_buffer[audio_buffer_index] = map(adc_value, 0, 4095, 0, 255);
audio_buffer_index++;
if (audio_buffer_index == BUFFER_SIZE) {
audio_buffer_ready = true;
audio_buffer_index = 0;
}
}

// Get current time in ISO format
String get_current_time() {
time_t now = time(nullptr);
char iso_time[20];
strftime(iso_time, sizeof(iso_time), "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));
return String(iso_time);
}

// Handle root page
void handle_root() {
String html = "<html><body><h1>ESP32 SIP Phone</h1>";
html += "<form action='/submit' method='POST'>";
html += "IP Address: <input type='text' name='ip' value='" + ip.toString() + "'><br>";
html += "Subnet Mask: <input type='text' name='subnet' value='" + subnet.toString() + "'><br>";
html += "Gateway: <input type='text' name='gateway' value='" + gateway.toString() + "'><br>";
html += "DNS Server 1: <input type='text' name='dns1' value='" + dns1.toString() + "'><br>";
html += "DNS Server 2: <input type='text' name='dns2' value='" + dns2.toString() + "'><br>";
html += "SIP Server Port: <input type='text' name='port' value='" + String(port) + "'><br>";
html += "SIP Username: <input type='text' name='user' value='" + user + "'><br>";
html += "SIP Password: <input type='password' name='pass' value='"value='" + String(port) + "'><br>";
html += "SIP Username: <input type='text' name='user' value='" + user + "'><br>";
html += "SIP Password: <input type='password' name='pass' value='"pass + "'><br>";
html += "Audio Codec: <select name='codec'>";
html += "<option value='G.711' " + (codec == "G.711" ? "selected" : "") + ">G.711</option>";
html += "<option value='G.729' " + (codec == "G.729" ? "selected" : "") + ">G.729</option>";
html += "</select><br>";
html += "<input type='submit' value='Submit'></form>";
html += "</body></html>";
server.send(200, "text/html", html);
}
// Handle form submission
void handle_submit() {
ip = IPAddress(server.arg("ip").toInt(), server.arg("ip").toInt(),
server.arg("ip").toInt(), server.arg("ip").toInt());
subnet = IPAddress(server.arg("subnet").toInt(), server.arg("subnet").toInt(),
server.arg("subnet").toInt(), server.arg("subnet").toInt());
gateway = IPAddress(server.arg("gateway").toInt(), server.arg("gateway").toInt(),
server.arg("gateway").toInt(), server.arg("gateway").toInt());
dns1 = IPAddress(server.arg("dns1").toInt(), server.arg("dns1").toInt(),
server.arg("dns1").toInt(), server.arg("dns1").toInt());
dns2 = IPAddress(server.arg("dns2").toInt(), server.arg("dns2").toInt(),
server.arg("dns2").toInt(), server.arg("dns2").toInt());
port = server.arg("port").toInt();
user = server.arg("user");
pass = server.arg("pass");
codec = server.arg("codec");

// Reconfigure PJSIP
pjsua_destroy();
setup_pjsip();

// Update LCD display
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("IP: " + ip.toString());
lcd.setCursor(0, 1);
lcd.print("User: " + user);
lcd.setCursor(0, 2);
lcd.print("Codec: " + codec);

server.sendHeader("Location", String("http://") + ip.toString());
server.send(302, "text/plain", "");
}

// Set up the sketch
void setup() {
Serial.begin(115200);

setup_wifi();
setup_pjsip();
setup_web_server();
setup_lcd();
}

// Main loop
void loop() {
// Check if audio buffer is ready for playback
if (audio_buffer_ready) {
pjsua_snd_playback(audio_buffer, BUFFER_SIZE);
audio_buffer_ready = false;
}

// Check for incoming call
if (call_id == PJSUA_INVALID_ID) {
return;
}

// Handle call events
pjsua_call_info ci;
pjsua_call_get_info(call_id, &ci);
if (ci.state == PJSIP_INV_STATE_DISCONNECTED) {
call_id = PJSUA_INVALID_ID;
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("Call ended");
return;
}

// Handle DTMF tones
if (ci.state == PJSIP_INV_STATE_CONFIRMED && ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
pjsua_call_dump_cap(call_id, NULL);
pjsua_call_dump(call_id, PJ_LOG_LEVEL_DEBUG, "Call status");
pjsua_call_info ci;
pjsua_call_get_info(call_id, &ci);
pjsua_call_dump_media_status(call_id, PJ_LOG_LEVEL_DEBUG, "Media status");
pjsua_call_dump_stream_info(call_id, PJ_LOG_LEVEL_DEBUG, "Stream info");
pjsua_call_dump_sdp(call_id, PJ_LOG_LEVEL_DEBUG, "SDP");

pjsua_msg_data msg_data;
pjsua_msg_data_init(&msg_data);
pjsua_call_info ci;
pjsua_call_get_info(call_id, &ci);
pjsua_msg_data_type msg_type = PJSUA_MSG_DATA_TYPE_TEXT;
pj_str_t mime_type = pj_str("text/plain");
pj_str_t content = pj_str("This is a test message");
pjsua_msg_data_set_text(&msg_data, &mime_type, &content);

// Send message
pjsua_call_send_message(call_id, &msg_type, &msg_data, NULL, NULL);

Serial.println("Message sent.");
}

// Handle web form submission
void handle_submit() {
// Update network settings
ip = IPAddress(server.arg("ip").toInt(), server.arg("ip").toInt(), server.arg("ip").toInt(), server.arg("ip").toInt());
subnet = IPAddress(server.arg("subnet").toInt(), server.arg("subnet").toInt(), server.arg("subnet").toInt(), server.arg("subnet").toInt());
gateway = IPAddress(server.arg("gateway").toInt(), server.arg("gateway").toInt(), server.arg("gateway").toInt(), server.arg("gateway").toInt());
dns1 = IPAddress(server.arg("dns1").toInt(), server.arg("dns1").toInt(), server.arg("dns1").toInt(), server.arg("dns1").toInt());
dns2 = IPAddress(server.arg("dns2").toInt(), server.arg("dns2").toInt(), server.arg("dns2").toInt(), server.arg("dns2").toInt());
port = server.arg("port").toInt();
user = server.arg("user");
pass = server.arg("pass");
codec = server.arg("codec");

// Reconfigure PJSIP with new settings
pjsua_transport_config cfg;
pjsua_transport_config_default(&cfg);
cfg.port = port;
pjsua_transport_modify(transport_id, &cfg);

pjsua_acc_config acc_cfg;
pjsua_acc_config_default(&acc_cfg);
acc_cfg.id = pj_str(user.c_str());
acc_cfg.reg_uri = pj_str("sip:" + ip.toString());
acc_cfg.cred_count = 1;
acc_cfg.cred_info[0].realm = pj_str("*");
acc_cfg.cred_info[0].scheme = pj_str("digest");
acc_cfg.cred_info[0].username = pj_str(user.c_str());
acc_cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
acc_cfg.cred_info[0].data = pj_str(pass.c_str());
pjsua_acc_modify(acc_id, &acc_cfg);

pjsua_codec_info codec_info;
pj_str_t codec_id = pj_str(codec.c_str());
pjsua_enum_t codec_index = pjsua_codec_get_id(&codec_id);
pjsua_codec_get_info(codec_index, &codec_info);
pjsua_codec_set_priority(&codec_info, PJMEDIA_CODEC_PRIO_MAX);
}

void setup() {
// Initialize serial communication
Serial.begin(115200);

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
// Handle incoming web requests
server.handleClient();

// Handle incoming PJSIP messages and events
pjsua_event event;
if (pjsua_get_event(&event, 0) == PJ_SUCCESS) {
pjsua_handle_events(0);
}

// Check if audio buffer is ready
if (audio_buffer_ready) {
//// Send audio samples to PJSIP library
pjsua_frame frame;
frame.size = BUFFER_SIZE;
frame.buf = audio_buffer;
pj_status_t status = pjsua_recorder_write_frame(PJSUA_INVALID_ID, &frame);
if (status != PJ_SUCCESS) {
Serial.print("Error sending audio: ");
Serial.println(pj_strerror(status).ptr);
}

// Reset audio buffer
audio_buffer_ready = false;
audio_buffer_index = 0;
}

// Handle web server requests
server.handleClient();
}
}

// Handle web server requests
void loop() {
// Handle incoming PJSIP messages and events
pjsua_event event;
if (pjsua_get_event(&event, 0) == PJ_SUCCESS) {
pjsua_handle_events(0);
}

// Check if audio buffer is ready
if (audio_buffer_ready) {
// Play audio buffer
pjsua_conf_port_id playback_port;
pjmedia_port *source_port;
pjmedia_stream_info stream_info;
pj_status_t status;
// Get default playback port
playback_port = pjsua_get_conf_port(-1);

// Create stream info
pjmedia_stream_info_init(&stream_info);
stream_info.type = PJMEDIA_TYPE_AUDIO;
stream_info.dir = PJMEDIA_DIR_DECODING;
stream_info.cname = pj_str("Playback");
stream_info.clock_rate = 8000;
stream_info.channel_count = 1;
stream_info.samples_per_frame = 160;
stream_info.bits_per_sample = 16;
stream_info.frame_ptime = 20;

// Create source port
status = pjmedia_port_create_buffer(pj_get_default_pool(), BUFFER_SIZE, BUFFER_SIZE, &source_port);
if (status != PJ_SUCCESS) {
  Serial.println("Error creating source port");
  return;
}

// Write audio buffer to source port
status = pjmedia_port_write(source_port, audio_buffer, BUFFER_SIZE);
if (status != PJ_SUCCESS) {
  Serial.println("Error writing to source port");
  return;
}

// Connect source port to playback port
status = pjmedia_endpoint_create_stream(pjsua_get_pjmedia_endpoint(), &stream_info, source_port, &playback_port, 0);
if (status != PJ_SUCCESS) {
  Serial.println("Error connecting source port to playback port");
  return;
}

// Start playback
pjsua_conf_connect(playback_port, 0);
}

// Reset audio buffer
audio_buffer_ready = false;
audio_buffer_index = 0;

// Handle web server requests
server.handleClient();
}
// Handle web server form submission
void handle_submit() {
// Update network settings
ip = IPAddress(server.arg("ip").toInt());
subnet = IPAddress(server.arg("subnet").toInt());
gateway = IPAddress(server.arg("gateway").toInt());
dns1 = IPAddress(server.arg("dns1").toInt());
dns2 = IPAddress(server.arg("dns2").toInt());
port = server.arg("port").toInt();
user = server.arg("user");
pass = server.arg("pass");
codec = server.arg("codec");

// Configure network
WiFi.config(ip, gateway, subnet, dns1, dns2);

// Set up PJSIP
setup_pjsip();

// Display success message
String html = "<html><body><h1>Settings Updated</h1>";
html += "<p>IP Address: " + ip.toString() + "</p>";
html += "<p>Subnet Mask: " + subnet.toString() + "</p>";
html += "<p>Gateway: " + gateway.toString() + "</p>";
html += "<p>DNS Server 1: " + dns1.toString() + "</p>";
html += "<p>DNS Server 2: " + dns2.toString() + "</p>";
html += "<p>SIP Server Port: " + String(port) + "</p>";
html += "<p>SIP Username: " + user + "</p>";
html += "<p>Codec: " + codec + "</p>";
html += "<a href='/'>Back</a></body></html>";
server.send(200, "text/html", html);
}

// Set up web server
void setup_web_server() {
server.on("/", handle_root);
server.on("/submit", HTTP_POST, handle_submit);
server.begin();
}

// Set up LCD display
void setup_lcd() {
lcd.begin(LCD_COLS, LCD_ROWS);
lcd.print("ESP32 SIP Phone");
lcd.setCursor(0, 1);
lcd.print("Initializing...");
}

// Set up hardware
void setup() {
Serial.begin(115200);

setup_wifi();
setup_pjsip();
setup_web_server();
setup_lcd();

// Set up ADC interrupt for audio input
adcAttachInterrupt(ADC_PIN, adc_interrupt_handler, 4095);

// Set up PWM for audio output
ledcSetup(0, 20000, 8);
ledcAttachPin(PWM_LEFT_PIN, 0);
ledcAttachPin(PWM_RIGHT_PIN, 1);
}

// Main loop
void loop() {
// Handle incoming PJSIP messages and events
pjsua_event event;
if (pjsua_get_event(&event, 0) == PJ_SUCCESS) {
pjsua_handle_events(0);
}

// Check if audio buffer is ready
if (audio_buffer_ready) {
// Process audio buffer
pjsua_frame audio_frame;
audio_frame.size = BUFFER_SIZE;
audio_frame.buf = audio_buffer;pjsua_channel *channel = pjsua_call_get_channel(call_id);
if (channel) {
  pjsua_conf_port_id conf_port_id = pjsua_call_get_conf_port(call_id);
  pjsua_conf_port_info conf_port_info;
  pjsua_conf_get_port_info(conf_port_id, &conf_port_info);
  pjsua_direction dir = (conf_port_info.slot[0].channel == channel) ? PJMEDIA_DIR_ENCODING : PJMEDIA_DIR_DECODING;
  pjsua_conf_adjust_rx_level(conf_port_id, PJMEDIA_CONF_RX_LEVEL_MUTE, &dir,&channel_count, &slot);

// Deinitialize PJSUA
pjsua_call_hangup_all();
pjsua_transport_close(transport_id);
pjsua_destroy();

// Restart ESP32
ESP.restart();
}

void loop() {
// Handle incoming PJSIP messages and events
pjsua_event event;
if (pjsua_get_event(&event, 0) == PJ_SUCCESS) {
pjsua_handle_events(0);
}

// Check if audio buffer is ready
if (audio_buffer_ready) {
// Send audio to PJSIP
pjsua_call_info call_info;
pjsua_call_get_info(call_id, &call_info);
pjsua_conf_port_id conf_port_id = call_info.conf_slot;
pjsua_conf_port_info conf_port_info;
unsigned channel_count = 1;
unsigned slot = 0;
pjsua_conf_get_port_info(conf_port_id, &conf_port_info);
pjsua_direction dir = (conf_port_info.slot[0].channel == channel) ? PJMEDIA_DIR_ENCODING : PJMEDIA_DIR_DECODING;
pjsua_conf_adjust_rx_level(conf_port_id, PJMEDIA_CONF_RX_LEVEL_MUTE, &dir, &channel_count, &slot);
pjsua_conf_adjust_rx_level(conf_port_id, 0, &dir, &channel_count, &slot);
pjsua_conf_port_get_frame_size(conf_port_id, &frame_size);pjsua_frame frame;
frame.size = frame_size;
frame.buf = audio_buffer;

pjsua_conf_port_write(conf_port_id, &frame);

// Reset audio buffer
audio_buffer_ready = false;
audio_buffer_index = 0;
pjsua_frame frame;
frame.size = frame_size;
frame.buf = audio_buffer;

pjsua_conf_port_write(conf_port_id, &frame);

// Reset audio buffer
audio_buffer_ready = false;
audio_buffer_index = 0;
}

// Handle web server requests
server.handleClient();
}
// Handle web server requests
void handle_root() {
String html = "<html><body><h1>ESP32 SIP Phone</h1>";
html += "<form action='/submit' method='POST'>";
html += "IP Address: <input type='text' name='ip' value='" + ip.toString() + "'><br>";
html += "Subnet Mask: <input type='text' name='subnet' value='" + subnet.toString() + "'><br>";
html += "Gateway: <input type='text' name='gateway' value='" + gateway.toString() + "'><br>";
html += "Primary DNS: <input type='text' name='dns1' value='" + dns1.toString() + "'><br>";
html += "Secondary DNS: <input type='text' name='dns2' value='" + dns2.toString() + "'><br>";
html += "SIP Server: <input type='text' name='server' value='" + server.toString() + "'><br>";
html += "SIP Server Port: <input type='text' name='port' value='" + String(port) + "'><br>";
html += "SIP Username: <input type='text' name='user' value='" + user + "'><br>";
html += "SIP Password: <input type='password' name='pass' value='" + pass + "'><br>";
html += "Audio Codec: <select name='codec'>";
html += "<option value='G.711' " + ((codec == "G.711") ? "selected" : "") + ">G.711</option>";
html += "<option value='G.729' " + ((codec == "G.729") ? "selected" : "") + ">G.729</option>";
html += "</select><br>";
html += "<input type='submit' value='Save'>";
html += "</form></body></html>";
server.send(200, "text/html", html);
}

void handle_submit() {
if (server.method() == HTTP_POST) {
ip = IPAddress(server.arg("ip").toInt(), server.arg("ip").toInt() >> 8, server.arg("ip").toInt() >> 16, server.arg("ip").toInt() >> 24);
subnet = IPAddress(server.arg("subnet").toInt(), server.arg("subnet").toInt() >> 8, server.arg("subnet").toInt() >> 16, server.arg("subnet").toInt() >> 24);
gateway = IPAddress(server.arg("gateway").toInt(), server.arg("gateway").toInt() >> 8, server.arg("gateway").toInt() >> 16, server.arg("gateway").toInt() >> 24);
dns1 = IPAddress(server.arg("dns1").toInt(), server.arg("dns1").toInt() >> 8, server.arg("dns1").toInt() >> 16, server.arg("dns1").toInt() >> 24);
dns2 = IPAddress(server.arg("dns2").toInt(), server.arg("dns2").toInt() >> 8, server.arg("dns2").toInt() >> 16, server.arg("dns2").toInt() >> 24);
server = server.arg("server");
port = server.arg("port").toInt();
user = server.arg("user");
pass = server.arg("pass");
codec = server.arg("codec");
// Save settings to EEPROM
EEPROM.put(0, ip);
EEPROM.put(4, subnet);
EEPROM.put(8, gateway);
EEPROM.put(12, dns1);
EEPROM.put(16, dns2);
EEPROM.put(20, port);
EEPROM.put(22, user);
EEPROM.put(54, pass);
EEPROM.put(86, codec);
EEPROM.commit();

// Update LCD display
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("Network settings");
lcd.setCursor(0, 1);
lcd.print("saved to EEPROM");
lcd.setCursor(0, 2);
lcd.print("Restarting...");
lcd.setCursor(0, 3);
lcd.print(get_current_time());
delay(5000);
ESP.restart();
}

void loop() {
// Handle incoming PJSIP messages and events
pjsua_event event;
if (pjsua_get_event(&event, 0) == PJ_SUCCESS) {
pjsua_handle_events(0);
}

// Check if audio buffer is ready
if (audio_buffer_ready) {
// Send audio to remote party
pjsua_call_info call_info;
pjsua_call_get_info(call_id, &call_info);
pjsua_conf_port_id conf_port_id = call_info.conf_slot;if (conf_port_id != PJSUA_INVALID_ID) {
  pjsua_conf_port_info conf_port_info;
  pjsua_conf_get_port_info(conf_port_id, &conf_port_info);
  pjmedia_port *port = conf_port_info.slot[0].port;

  if (port) {
    pjmedia_frame frame;
    pjmedia_frame_init(&frame, codec.c_str(), call_info.remote_info.audio.sample_rate, call_info.remote_info.audio.channel_cnt, BUFFER_SIZE / call_info.remote_info.audio.frame_ptime);

    pj_status_t status = pjmedia_codec_pack_frame(port->info.codec_id, &frame, audio_buffer, &audio_buffer_index);
    if (status == PJ_SUCCESS) {
      pjmedia_port_put_frame(port, &frame);
    }
  }
}

// Reset audio buffer
audio_buffer_ready = false;
audio_buffer_index = 0;
}

// Handle web server requests
server.handleClient();
}// Write network settings to EEPROM
void write_eeprom_settings() {
EEPROM.put(0, ip);
EEPROM.put(4, subnet);
EEPROM.put(8, gateway);
EEPROM.put(12, dns1);
EEPROM.put(16, dns2);
EEPROM.put(20, port);
EEPROM.put(24, user);
EEPROM.put(64, pass);
EEPROM.put(128, codec);
EEPROM.commit();
}

// Read network settings from EEPROM
void read_eeprom_settings() {
IPAddress stored_ip, stored_subnet, stored_gateway, stored_dns1, stored_dns2;
String stored_user, stored_pass, stored_codec;
uint16_t stored_port;

EEPROM.get(0, stored_ip);
EEPROM.get(4, stored_subnet);
EEPROM.get(8, stored_gateway);
EEPROM.get(12, stored_dns1);
EEPROM.get(16, stored_dns2);
EEPROM.get(20, stored_port);
EEPROM.get(24, stored_user);
EEPROM.get(64, stored_pass);
EEPROM.get(128, stored_codec);

if (!stored_ip.isSet()) {
Serial.println("No network settings in EEPROM, using default values.");
return;
}

ip = stored_ip;
subnet = stored_subnet;
gateway = stored_gateway;
dns1 = stored_dns1;
dns2 = stored_dns2;
port = stored_port;
user = stored_user;
pass = stored_pass;
codec = stored_codec;
}

// Set up the device
void setup() {
// Start serial communication
Serial.begin(115200);

// Read network settings from EEPROM
read_eeprom_settings();

// Set up Wi-Fi connection
setup_wifi();

// Set up PJSIP library
setup_pjsip();

// Set up web server
setup_web_server();

// Set up LCD display
setup_lcd();

// Initialize ADC and PWM pins
adcAttachPin(ADC_PIN);
ledcAttachPin(PWM_LEFT_PIN, 1);
ledcAttachPin(PWM_RIGHT_PIN, 2);
ledcSetup(1, 5000, 8);
ledcSetup(2, 5000, 8);

// Attach interrupt to ADC pin
attachInterrupt(digitalPinToInterrupt(ADC_PIN), adc_interrupt_handler, CHANGE);
}

// Main loop
void loop() {
// Handle incoming PJSIP messages and events
pjsua_event event;
if (pjsua_get_event(&event, 0) == PJ_SUCCESS) {
pjsua_handle_events(0);
}

// Check if audio buffer is ready
if (audio_buffer_ready) {
// Send audio buffer to PJSIP stream
pjsua_stream_info stream_info;
pjsua_stream_get_info(pjsua_call_get_stream_info(call_id, 0), &stream_info);
pjsua_frame frame;
frame.size = audio_buffer_index;
frame.buf = audio_buffer;
pjsua_conf_port_id conf_port_id = stream_info.aud.conf_port_id;
pjsua_conf_port_info conf_port_info;
pjsua_conf_get_port_info(conf_port_id, &conf_port_info);
pjmedia_channel *channel = conf_port_info.slot[0].channel;
pjsua_direction dir = (conf_port_info.slot[0].channel == channel) ? PJMEDIA_DIR_ENCODING : PJMEDIA_DIR_DECODING;
pjsua_conf_adjust_rx_level(conf_port_id, PJMEDIA_CONF_RX_LEVEL_MUTE, &dir, PJ_FALSE);// Mute audio playback
pjmedia_port *port = pjsua_player_get_port(0);
pjsua_conf_port_info player_info;
pjsua_conf_get_port_info(port->conf_port_id, &player_info);
pjmedia_channel *channel = player_info.slot[0].channel;
pjsua_conf_adjust_rx_level(port->conf_port_id, PJMEDIA_CONF_RX_LEVEL_MUTE, &dir, PJ_FALSE);// Mute audio playback
void mute_audio_playback() {
// Get audio player port info
pjsua_port_info player_info;
pjsua_conf_get_port_info(audio_player.port->conf_port_id, &player_info);

// Get audio player channel and direction
pjmedia_channel *channel = player_info.slot[0].channel;
pjsua_direction dir = (player_info.slot[0].channel == channel) ? PJMEDIA_DIR_ENCODING : PJMEDIA_DIR_DECODING;

// Mute audio playback
pjsua_conf_adjust_rx_level(audio_player.port->conf_port_id, PJMEDIA_CONF_RX_LEVEL_MUTE, &dir, PJ_FALSE);
}

// Unmute audio playback
void unmute_audio_playback() {
// Get audio player port info
pjsua_port_info player_info;
pjsua_conf_get_port_info(audio_player.port->conf_port_id, &player_info);

// Get audio player channel and direction
pjmedia_channel *channel = player_info.slot[0].channel;
pjsua_direction dir = (player_info.slot[0].channel == channel) ? PJMEDIA_DIR_ENCODING : PJMEDIA_DIR_DECODING;

// Unmute audio playback
pjsua_conf_adjust_rx_level(audio_player.port->conf_port_id, 0, &dir, PJ_FALSE);
}

// to be continued
