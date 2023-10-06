/**
 * Communication with thermocon, HEC0*-*, SMC Co., Ltd.
 * 2009.8.5 written by NAGAI Makoto
 * For 30-cm telescope. 
 */


/*
 * singleton, since we have only one weather monitor. 
 */
int penguin_thermocon_init(const char* devName);
int penguin_thermocon_end();

int penguin_thermocon_getPresetTemperature();
int penguin_thermocon_getInsideTemperature();
int penguin_thermocon_getOutsideTemperature();
int penguin_thermocon_getAlerm();

/* medium level API */
int penguin_thermocon_send_get_command(char cmd);
int penguin_thermocon_send_set_command(char cmd, int value10);

/* low level API */
//void penguin_thermocon_awake();
int penguin_thermocon_send_frame(const char* cmd, int length);
int penguin_thermocon_receive();
char* penguin_thermocon_getMassage();


