#include "audio.h"

// Audio buffer
#define AUDIO_BUFFER_SIZE 4096
static int16_t audio_buffer[AUDIO_BUFFER_SIZE];
static size_t audio_buffer_pos = 0;

// Audio output pins
#define AUDIO_OUTPUT_PIN1 25
#define AUDIO_OUTPUT_PIN2 26

// Audio sampling rate and channel count
#define AUDIO_SAMPLING_RATE 16000
#define AUDIO_CHANNEL_COUNT 1

// Callback function for the ADC
void adc_interrupt_handler(void *arg) {
  int16_t value = adc1_get_raw(ADC1_CHANNEL_0);

  if (audio_buffer_pos < AUDIO_BUFFER_SIZE) {
    audio_buffer[audio_buffer_pos++] = value;
  }
}

// Initialize the audio input and output
void init_audio() {
  // Initialize ADC
  adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
  adc1_intr_enable(ADC1_CHANNEL_0);
  adc1_isr_register(&adc_interrupt_handler, NULL, 0, NULL);
  adc_chars = new esp_adc_cal_characteristics_t;
  esp_adc_cal_value_t adc_type = esp_adc_cal_characterize(
    ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, adc_chars
  );
  adc_set_clk_div(ADC_UNIT_1, ADC_CLOCK_DIV_1);

  // Initialize PWM channels
  setup_gpio();
  ledcSetup(0, AUDIO_SAMPLING_RATE, 16);
  ledcAttachPin(AUDIO_OUTPUT_PIN1, 0);
  ledcSetup(1, AUDIO_SAMPLING_RATE, 16);
  ledcAttachPin(AUDIO_OUTPUT_PIN2, 1);

  // Connect audio output to PWM channels
  connect_audio_output();
}

// Read audio data from the buffer and send it to the output
void read_audio_buffer() {
  // Check if there is any audio data in the buffer
  if (audio_buffer_pos > 0) {
    size_t audio_sample_count = audio_buffer_pos;
    audio_buffer_pos = 0;

    // Convert audio buffer to signed 16-bit PCM format
    int16_t *audio_samples = new int16_t[audio_sample_count];
    memcpy(audio_samples, audio_buffer, audio_sample_count * sizeof(int16_t));

    // Reset audio buffer
    memset(audio_buffer, 0, AUDIO_BUFFER_SIZE * sizeof(int16_t));

    // Send audio data to PWM channels
    ledcWrite(0, (uint32_t *)audio_samples, audio_sample_count * AUDIO_CHANNEL_COUNT);
    ledcWrite(1, (uint32_t *)audio_samples, audio_sample_count * AUDIO_CHANNEL_COUNT);

    // Free memory
    delete[] audio_samples;
  }
}

// Connect audio output to PWM channels
void connect_audio_output() {
  dac_output_enable(DAC_CHANNEL_1);
  dac_output_enable(DAC_CHANNEL_2);
  gpio_set_direction(AUDIO_OUTPUT_PIN1, GPIO_MODE_OUTPUT);
  gpio_set_direction(AUDIO_OUTPUT_PIN2, GPIO_MODE_OUTPUT);
  gpio_matrix_out(AUDIO_OUTPUT_PIN1, DAC_CHANNEL_1_IDX, false, false);
  gpio_matrix_out(AUDIO_OUTPUT_PIN2, DAC_CHANNEL_2_IDX, false, false);
}

// Disconnect audio output from PWM channels
void disconnect_audio_output() {
  gpio_matrix_out(AUDIO_OUTPUT_PIN1, GPIO_FUNC_GPIO25, false, false);
  gpio_matrix_out(AUDIO_OUTPUT_PIN2, GPIO_FUNC_GPIO26, false, false);
  gpio_set_direction(AUDIO_OUTPUT_PIN1, GPIO_MODE_INPUT);
  gpio_set_direction(AUDIO_OUTPUT_PIN2, GPIO_MODE_INPUT);
}

// Initialize the ADC
void init_adc() {
  // Configure ADC1 channel 0
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
}

// Get the current audio volume (0-100)
int get_audio_volume() {
  uint8_t volume = 0;

  if (ledcRead(0) > 0 && ledcRead(1) > 0) {
    volume = (ledcRead(0) + ledcRead(1)) / 2 * 100 / 65536;
  }

  return volume;
}
