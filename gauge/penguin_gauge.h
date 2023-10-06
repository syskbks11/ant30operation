/**
 * Communication with motor driver QT-CD1, Chuo precision industrial Co., Ltd.
 * 2008.12.11 written by NAGAI Makoto
 * For 30-cm telescope. 
 */


/*
 * singleton, since we have only one chopper motor. 
 */
int penguin_gauge_init();
int penguin_gauge_end();


/* high level API */
double penguin_gauge_get_pressure(int channel);
int penguin_gauge_get_identification();
int penguin_gauge_get_errorStatus();

/* medium level API */
int penguin_gauge_receiveAcknowledge();
int penguin_gauge_sendEnquery();
//int penguin_lakeshore_send_get_command(int addr, char const* cmd);
//int penguin_lakeshore_send_set_command(int addr, char const* cmd, int value);

/* low level API */
int penguin_gauge_send(char const* cmd);
int penguin_gauge_receive();
char* penguin_gauge_getMessage();


