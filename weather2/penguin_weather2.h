/**
 * Communication with weather monitor, TR-72Ui, TandD Co., Ltd.
 * 2009.09.07 written by NAGAI Makoto
 * Original: for TR-73U
 * 2008.12.11 written by NAGAI Makoto
 * For 30-cm telescope. 
 */


/*
 * singleton, since we have only one weather monitor. 
 */
int penguin_weather_init(const char* devName);
int penguin_weather_end();

double* penguin_weather_getData();

/* medium level API */
//int penguin_weather_send_simple_command(char cmd, char subCmd);

/* low level API */
int penguin_weather_send(char cmd);
int penguin_weather_receive(int bytes);
char* penguin_weather_getMassage();


