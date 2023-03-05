#include "sip_client.h"

void SIPClient::setup() {
  pjsua_config cfg;
  pjsua_logging_config log_cfg;
  pjsua_media_config media_cfg;
  pjsua_transport_config trans_cfg;

  // Initialize PJSUA
  pjsua_config_default(&cfg);
  cfg.cb.on_incoming_call = &SIPClient::on_incoming_call;
  cfg.cb.on_call_media_state = &SIPClient::on_call_media_state;
  cfg.cb.on_call_state = &SIPClient::on_call_state;
  pjsua_logging_config_default(&log_cfg);
  media_cfg.clock_rate = 8000;
  media_cfg.snd_clock_rate = 44100;
  media_cfg.ec_options = PJSUA_ECHO_USE_NOISE_SUPPRESSOR;
  pjsua_transport_config_default(&trans_cfg);
  trans_cfg.port = DEFAULT_PORT;
  trans_cfg.tls_setting.method = PJSIP_TLSV1_0;
  trans_cfg.tls_setting.verify_client = PJSIP_TLS_VERIFY_DISABLED;

  // Initialize PJSUA
  pj_status_t status = pjsua_create();
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, "Error initializing PJSUA");
  pjsua_set_null_snd_dev();
  status = pjsua_init(&cfg, &log_cfg, &media_cfg);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, "Error initializing PJSUA");
  status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &trans_cfg, NULL);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, "Error creating transport");
  pjsua_start();
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
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, "Error adding account");
}

void SIPClient::loop() {
  if (call_id != PJSUA_INVALID_ID) {
    pjsua_call_info call_info;
    pjsua_call_get_info(call_id, &call_info);
    lcd.setCursor(0, 1);
    lcd.print(call_info.state_text.ptr);
    lcd.print(" ");
  } else {
    lcd.setCursor(0, 1);
    lcd.print("No call   ");
    lcd.print(" ");
  }
}

void SIPClient::make_call() {
  pjsua_call_setting call_setting;
  pjsua_call_setting_default(&call_setting);
  call_setting.vid_cnt = 0;

  pjsua_msg_data msg_data;
  pjsua_msg_data_init(&msg_data);
  pj_str_t dest_uri = pj_str("sip:" CALL_DEST);
  pj_status_t status = pjsua_call_make_call(acc_id, &dest_uri, &call_setting, NULL, &msg_data, &call_id);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, "Error making call");
}

void SIPClient::hangup_call() {
  if (call_id != P
void SIPClient::loop() {
  if (call_id != PJSUA_INVALID_ID) {
    pjsua_call_info call_info;
    pjsua_call_get_info(call_id, &call_info);
    lcd.setCursor(0, 1);
    lcd.print(call_info.state_text.ptr);
    lcd.print(" ");
  } else {
    lcd.setCursor(0, 1);
    lcd.print("READY");
    lcd.print(" ");
  }

  // Check for incoming call
  pjsua_call_id incoming_call_id = pjsua_call_get_id(pjsua_call_get_count() - 1);
  if (incoming_call_id != PJSUA_INVALID_ID) {
    call_id = incoming_call_id;
    pjsua_call_info call_info;
    pjsua_call_get_info(call_id, &call_info);
    pjsua_call_answer(call_id, 200, NULL, NULL);
    lcd.setCursor(0, 1);
    lcd.print(call_info.state_text.ptr);
    lcd.print(" ");
  }
}

void SIPClient::on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata) {
  if (SIPClient::call_id == PJSUA_INVALID_ID) {
    // Answer the call
    pjsua_call_info call_info;
    pjsua_call_get_info(call_id, &call_info);
    SIPClient::call_id = call_id;
    pjsua_call_answer(call_id, 200, NULL, NULL);
    lcd.setCursor(0, 1);
    lcd.print(call_info.state_text.ptr);
    lcd.print(" ");
  } else {
    // Reject the call
    pjsua_call_hangup(call_id, 486, NULL, NULL);
  }
}

void SIPClient::on_call_state(pjsua_call_id call_id, pjsip_event *e) {
  pjsua_call_info call_info;
  pjsua_call_get_info(call_id, &call_info);
  lcd.setCursor(0, 1);
  lcd.print(call_info.state_text.ptr);
  lcd.print(" ");
  if (call_info.state == PJSIP_INV_STATE_DISCONNECTED) {
    // Call ended
    SIPClient::call_id = PJSUA_INVALID_ID;
  }
}

void SIPClient::on_call_media_state(pjsua_call_id call_id) {
  pjsua_call_info call_info;
  pjsua_call_get_info(call_id, &call_info);

  if (call_info.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
    // Connect the call to the default sound device
    pjsua_conf_connect(call_info.conf_slot, 0);
    pjsua_conf_connect(0, call_info.conf_slot);
  }
}
