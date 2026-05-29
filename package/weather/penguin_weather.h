#ifndef INCLUDE_GUARD_UUID_70330c07_e65b_4843_b268_104d8802c819
#define INCLUDE_GUARD_UUID_70330c07_e65b_4843_b268_104d8802c819
/**
 * Communication with weather monitor, TR-73U, TandD Co., Ltd.
 * 2008.12.11 written by NAGAI Makoto
 * For 30-cm telescope. 
 */


/*
 * singleton, since we have only one weather monitor. 
 */
int penguin_weather_init(const char* devName);
int penguin_weather_end();

int penguin_weather_getData(double*);

/* medium level API */
int penguin_weather_send_simple_command(char cmd, char subCmd);

/* low level API */
void penguin_weather_awake();
int penguin_weather_send_frame(const char* cmd, int length);
int penguin_weather_receive();
char* penguin_weather_getMassage();


#endif

