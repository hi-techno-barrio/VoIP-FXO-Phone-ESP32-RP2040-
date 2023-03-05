#include <PJSIP.h>
#include <LiquidCrystal.h>

// Pin assignments
#define ADC_PIN 32
#define PWM_LEFT_PIN 25
#define PWM_RIGHT_PIN 26
#define LCD_RS 12
#define LCD_EN 13
#define LCD_D4 14
#define LCD_D5 27
#define LCD_D6 26
#define LCD_D7 25

// Audio buffer settings
#define SAMPLE_RATE 16000
#define FRAME_SIZE 20
#define BUFFER_SIZE (SAMPLE_RATE * FRAME_SIZE / 1000)
#define BUFFER_THRESHOLD (BUFFER_SIZE / 2)

// Global variables
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
uint8_t audio_buffer[BUFFER_SIZE];
volatile uint32_t audio_buffer_index = 0;
volatile bool audio_buffer_ready = false;

void setup() {
  Serial.begin(115200);

  lcd.begin(20, 4);
  lcd.setCursor(0, 0);
  lcd.print("ESP32 VoIP Client");

  // Initialize PJSIP library
  pj_status_t status;
  status = pjsua_create();
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, );

  // Initialize SIP settings
  pjsua_config cfg;
  pjsua_logging_config log_cfg;
  pjsua_media_config media_cfg;

  pjsua_config_default(&cfg);
  cfg.cb.on_incoming_call = &on_incoming_call;
  cfg.cb.on_call_media_state = &on_call_media_state;
  cfg.cb.on_call_state = &on_call_state;

  pjsua_logging_config_default(&log_cfg);
  log_cfg.console_level = 4;

  pjsua_media_config_default(&media_cfg);
  media_cfg.clock_rate = SAMPLE_RATE;

  // Initialize endpoint
  status = pjsua_init(&cfg, &log_cfg, &media_cfg);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, );

  // Add UDP transport
  pjsua_transport_config tp_cfg;
  pjsua_transport_config_default(&tp_cfg);

  status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &tp_cfg, NULL);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, );

  // Start PJSIP
  status = pjsua_start();
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, );

  // Register with SIP server
  pjsua_acc_id acc_id;
  pjsua_acc_config acc_cfg;
  pjsua_acc_config_default(&acc_cfg);

  acc_cfg.id = pj_str("sip:" SIP_USERNAME "@" SIP_DOMAIN);
  acc_cfg.reg_uri = pj_str("sip:" SIP_DOMAIN);
  acc_cfg.cred_count = 1;
  acc_cfg.cred_info[0].realm = pj_str(SIP_DOMAIN);
  acc_cfg.cred_info[0].scheme = pj_str("digest");
  acc_cfg.cred_info[0].username = pj_str(SIP_USERNAME);
  acc_cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
  acc_cfg.cred_info[0].data = pj_str(SIP_PASSWORD);

  status = pjsua_acc_add(&acc_cfg, PJ_TRUE, &acc_id);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, );

  // Configure PWM pins for audio output
  ledcSetup(0, SAMPLE_RATE, 8);
  ledcSetup(1, SAMPLE_RATE, 
9);

ledcAttachPin(PWM_LEFT_PIN, 0);
ledcAttachPin(PWM_RIGHT_PIN, 1);

ledcWrite(0, 0);
ledcWrite(1, 0);

// Configure ADC pin for audio input
adcAttachPin(ADC_PIN);
adcStart(ADC_PIN);
adc1_config_width(ADC_WIDTH_12Bit);
adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_0db);

// Configure ADC interrupt for audio input
adc1_esp32_isr_register(&adc_interrupt_handler, NULL, 1, NULL);

// Start serial communication
Serial.begin(115200);
while (!Serial) {
;
}

Serial.println("ESP32 VoIP Client Initialized");
}

void loop() {
// Handle incoming and outgoing calls
pjsua_handle_events(0);

// If audio buffer is full, write audio data to PWM pins
if (audio_buffer_ready) {
for (int i = 0; i < BUFFER_SIZE; i += 2) {
ledcWrite(0, audio_buffer[i]);
ledcWrite(1, audio_buffer[i + 1]);
}
audio_buffer_ready = false;
}

// Handle LCD updates
static uint32_t last_lcd_update = 0;
if (millis() - last_lcd_update >= 1000) {
last_lcd_update = millis();
  // Update LCD with call state information
pjsua_call_info ci;
pjsua_call_id call_id = pjsua_call_get_first();

if (call_id != PJSUA_INVALID_ID) {
  pjsua_call_get_info(call_id, &ci);

  lcd.setCursor(0, 1);
  lcd.print("Call: ");
  lcd.print(ci.remote_info.ptr);

  lcd.setCursor(0, 2);
  lcd.print("State: ");
  lcd.print(ci.state_text.ptr);
} else {
  lcd.setCursor(0, 1);
  lcd.print("Idle");
}

lcd.setCursor(0, 3);
lcd.print("Time: ");
lcd.print(get_current_time());
}
}

void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata) {
// Answer incoming call
pjsua_call_answer(call_id, 200, NULL, NULL);
}

void on_call_media_state(pjsua_call_id call_id) {
// Set audio device to use ADC for input and PWM for output
pjsua_aud_dev_route route;

route = PJMEDIA_AUD_DEV_ROUTE_CAPTURE;
route.channel = 0;
route.slot = 0;
route.priority = 0;
pjsua_aud_dev_set_setting(0, &route, PJMEDIA_AUD_DEV_CAP_INPUT_ROUTE, ADC_PIN);

route = PJMEDIA_AUD_DEV_ROUTE_PLAYBACK;
route.channel = 0;
route.slot = 0;
route.priority = 0;
pjsua_aud_dev_set_setting(0, &route, PJMEDIA_AUD_DEV_CAP_OUTPUT_ROUTE, PWM_LEFT_PIN);

route = PJMEDIA_AUD_DEV_ROUTE_PLAYBACK;
route.channel = 0;
route.slot = 1;
route.priority = 0;
pjsua_aud_dev_set_setting(0, &route, PJMEDIA_AUD_DEV_CAP_OUTPUT_ROUTE, PWM_RIGHT_PIN);
}

void on_call_state(pjsua_call_id call_id, pjsip_event *e) {
// Do nothing
}
{
uint32_t adc_value = adc1_get_raw(ADC1_CHANNEL_4);

if (audio_buffer_index < BUFFER_SIZE) {
audio_buffer[audio_buffer_index++] = adc_value >> 8;
audio_buffer[audio_buffer_index++] = adc_value & 0xFF;
}

if (audio_buffer_index >= BUFFER_THRESHOLD) {
audio_buffer_ready = true;
audio_buffer_index = 0;
}
}

String get_current_time() {
time_t now = time(nullptr);
struct tm *timeinfo = localtime(&now);
char time_str[20];
strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);
return String(time_str);
}
void adc_interrupt_handler(void *arg)
