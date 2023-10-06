/**
 * Communication with motor driver QT-CD1, Chuo precision industrial Co., Ltd.
 * 2008.12.11 written by NAGAI Makoto
 * For 30-cm telescope. 
 */


/*
 * singleton, since we have only one chopper motor. 
 */
int penguin_heater_init();
int penguin_heater_end();


/* high level API */
double penguin_heater_get_temperature(int addr);

/* medium level API */
int penguin_heater_send_get_command(int addr, char const* cmd);
int penguin_heater_send_set_command(int addr, char const* cmd, int value);

/* low level API */
int penguin_heater_send_frame(char const* cmd, int length);
int penguin_heater_receive();
char* penguin_heater_getMassage();


