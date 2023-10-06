/**
 * Communication with motor driver QT-CD1, Chuo precision industrial Co., Ltd.
 * 2008.12.11 written by NAGAI Makoto
 * For 30-cm telescope. 
 */


/*
 * singleton, since we have only one chopper motor. 
 */
int penguin_lakeshore_init();
int penguin_lakeshore_end();


/* high level API */
double penguin_lakeshore_get_temperature(int channel);
int penguin_lakeshore_get_identification();

/* medium level API */
//int penguin_lakeshore_send_get_command(int addr, char const* cmd);
//int penguin_lakeshore_send_set_command(int addr, char const* cmd, int value);

/* low level API */
int penguin_lakeshore_send(char const* cmd);
int penguin_lakeshore_receive();
char* penguin_lakeshore_getMessage();


