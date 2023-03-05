#ifndef SIP_CLIENT_H
#define SIP_CLIENT_H

#include <PJSIP.h>

class SIPClient {
  public:
    void setup();
    void loop();

  private:
    pjsua_acc_id acc_id;
    pjsua_call_id call_id = PJSUA_INVALID_ID;

    void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata);
    void on_call_media_state(pjsua_call_id call_id);
    void on_call_state(pjsua_call_id call_id, pjsip_event *e);
};

#endif // SIP_CLIENT_H
