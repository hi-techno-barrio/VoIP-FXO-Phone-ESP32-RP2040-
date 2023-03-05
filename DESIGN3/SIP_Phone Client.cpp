#include <WiFi.h>
#include <WebServer.h>
#include <PJSIP.h>

// Pin definitions
#define ADC_PIN     36
#define PWM_LEFT_PIN  26
#define PWM_RIGHT_PIN  25

// Audio buffer settings
#define BUFFER_SIZE  512
#define BUFFER_THRESHOLD  256

// Network settings
#define DEFAULT_IP    IPAddress(192, 168, 1, 1)
#define DEFAULT_SUBNET  IPAddress(255, 255, 255, 0)
#define DEFAULT_GATEWAY IPAddress(192, 168, 1, 254)
#define DEFAULT_DNS1   IPAddress(8, 8, 8, 8)
#define DEFAULT_DNS2   IPAddress(8, 8, 4, 4)
#define DEFAULT_PORT   5060
#define DEFAULT_USER   "user"
#define DEFAULT_PASS   "pass"

// PJSIP settings
#define MAX_CALLS    4
#define MAX_ACCS     1
#define MAX_PROXYS   1
#define RTP_PORT_START  20000
#define RTP_PORT_END    21000
#define JITTER_BUFFER_SIZE  2000

// LCD settings
#define LCD_COLS     20
#define LCD_ROWS     4

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
  media_cfg.clock_rate = 8000;
//  pjsuamedia_cfg.clock_rate = 8000;
media_cfg.snd_clock_rate = 44100;
media_cfg.ec_options = PJSUA_ECHO_USE_NOISE_SUPPRESSOR;
pjsua_transport_config_default(&trans_cfg);
trans_cfg.port = port;
trans_cfg.tls_setting.method = PJSIP_TLSV1_0;
trans_cfg.tls_setting.verify_client = PJSIP_TLS_VERIFY_DISABLED;

// Initialize PJSUA
status = pjsua_create();
PJ_ASSERT_RETURN(status == PJ_SUCCESS, "Error initializing PJSUA",);
pjsua_set_null_snd_dev();
status = pjsua_init(&cfg, &log_cfg, &media_cfg);
PJ_ASSERT_RETURN(status == PJ_SUCCESS, "Error initializing PJSUA",);
status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &trans_cfg, NULL);
PJ_ASSERT_RETURN(status == PJ_SUCCESS, "Error creating transport",);
pjsua_start();
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
server.begin();
}

// Set up LCD display
void setup_lcd() {
lcd.begin(LCD_COLS, LCD_ROWS);
lcd.clear();
lcd.print("VoIP Phone");
}

// Main setup function
void setup() {
Serial.begin(115200);
setup_wifi();
setup_pjsip();
setup_web_server();
setup_lcd();
pinMode(ADC_PIN, INPUT);
analogReadResolution(12);
analogSetPinAttenuation(ADC_PIN, ADC_11db);
ledcAttachPin(PWM_LEFT_PIN, 0);
ledcAttachPin(PWM_RIGHT_PIN, 1);
ledcSetup(0, 44100, 16);
ledcSetup(1, 44100, 16);
ledcAttachPin(PWM_LEFT_PIN, 0);
ledcAttachPin(PWM_RIGHT_PIN, 1);
ledcWrite(0, 0);
ledcWrite(1, 0);
}

// Main loop function
void loop() {
server.handleClient();

if (call_id != PJSUA_INVALID_ID) {
pjsua_call_info call_info;
pjsua_call_get_info(call_id, &call_info);
lcd.setCursor(0, 1);
lcd.print(call_info.state_text.ptr);
lcd.print(" ");
} else {
lcd.setCursor(0, 1);
lcd.print("Idle ");
}

if (audio_buffer_ready) {
ledcWrite(0, audio_buffer[0]);
ledcWrite(1, audio_buffer[BUFFER_THRESHOLD]);
for (size_t i = BUFFER_THRESHOLD; i < BUFFER_SIZE; i += 2) {
  ledcWrite(0, audio_buffer[i]);
  ledcWrite(1, audio_buffer[i + 1]);
}
audio_buffer_ready = false;
}
}

// Callback function for incoming calls
void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata) {
if (pjsua_call_get_count() >= MAX_CALLS) {
pjsua_call_answer(call_id, PJSIP_SC_BUSY_HERE, NULL, NULL);
return;
}
if (call_id != PJSUA_INVALID_ID) {
pjsua_call_hangup(call_id, 0, NULL, NULL);
}
pjsua_call_setting call_setting;
pjsua_call_setting_default(&call_setting);
call_setting.aud_cnt = 1;
call_setting.vid_cnt = 0;
pjsua_call_answer(call_id, 180, NULL, NULL);
pjsua_call_set_setting(call_id, &call_setting);
pjsua_call_answer(call_id, 200, NULL, NULL);
this->call_id = call_id;
}

// Callback function for call media state
void on_call_media_state(pjsua_call_id call_id) {
pjsua_call_info call_info;
pjsua_call_get_info(call_id, &call_info);
if (call_info.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
pjsua_conf_connect(call_info.conf_slot, 0);
pjsua_conf_connect(0, call_info.conf_slot);
}
}

// Callback function for call state
void on_call_state(pjsua_call_id call_id, pjsip_event *e) {
pjsua_call_info call_info;
pjsua_call_get_info(call_id, &call_info);
if (call_info.state == PJSIP_INV_STATE_DISCONNECTED) {
lcd.setCursor(0, 2);
lcd.print("Call ended");
lcd.print(" ");
call_id = PJSUA_INVALID_ID;
}
}

// ADC interrupt handler
void adc_interrupt_handler(void *arg) {
uint16_t sample = analogRead(ADC_PIN);
audio_buffer[audio_buffer_index++] = (sample >> 8) & 0xFF;
audio_buffer[audio_buffer_index++] = sample & 0xFF;
if (audio_buffer_index >= BUFFER_SIZE) {
audio_buffer_ready = true;
audio_buffer_index = 0;
}
}

// Get the current time as a string
String get_current_time() {
char buffer[20];
time_t now = time(NULL);
struct tm *tm_now = localtime(&now);
strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_now);
return String(buffer);
}

// Handle root request
void handle_root() {
String html = "<html><body><form method='post' action='/submit'>";
html += "<h2>Network Settings</h2><p>";
html += "IP Address: <input type='text' name='ip' value='" + ip.toString() + "'></p><p>";
html += "Subnet Mask: <input type='text' name='subnet' value='" + subnet.toString() + "'></p><p>";
html += "Default Gateway: <input type='text' name='gateway' value='" + gateway.toString() + "'></p><p>";
html += "Primary DNS: <input type='text' name='dns1' value='" + dns1.toString() + "'></p><p>";
html += "Secondary DNS: <input type='text' name='dns2' value='" + dns2.toString() + "'></p><p>";
html += "<input type='submit' value='Submit'></form></body></html>";
server.send(200, "text/html", html);
}

// Handle submit request
void handle_submit() {
String ip_str = server.arg("ip");
String subnet_str = server.arg("subnet");
String gateway_str = server.arg("gateway");
String dns1_str = server.arg("dns1");
String dns2_str = server.arg("dns2");

IPAddress new_ip, new_subnet, new_gateway, new_dns1, new_dns2;

if (!new_ip.fromString(ip_str) || !new_subnet.fromString(subnet_str) || !new_gateway.fromString(gateway_str) ||
!new_dns1.fromString(dns1_str) || !new_dns2.fromString(dns2_str)) {
server.send(400, "text/plain", "Invalid input");
return;
}

ip = new_ip;
subnet = new_subnet;
gateway = new_gateway;
dns1 = new_dns1;
dns2 = new_dns2;

IPAddress dns[] = {dns1, dns2};
WiFi.config(ip, gateway, subnet, dns);

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

