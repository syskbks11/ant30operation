/**
 * Communication with weather monitor.
 * 2008.12.25 written by NAGAI Makoto
 * For 32-m telescope. 
 */


/*
 * singleton, since we have only one weather monitor. 
 */
int penguin_weather_tkb32_init(const char* devName);
int penguin_weather_tkb32_end();

double* penguin_weather_tkb32_getData();
int penguin_weather_tkb32_getWindDirection();

/* low level API */
int penguin_weather_tkb32_receive();
char* penguin_weather_tkb32_getMessage();


