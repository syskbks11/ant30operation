/**
 * Communication with motor driver QT-CD1, Chuo precision industrial Co., Ltd.
 * 2008.12.11 written by NAGAI Makoto
 * For 30-cm telescope.
 */


/*
 * singleton, since we have only one chopper motor. 
 */
int penguin_chopper_init(const char* devName);
int penguin_chopper_end();

/* high level API */
int penguin_chopper_reset();
int penguin_chopper_configure_settings();
int penguin_chopper_configure_origin();
int penguin_chopper_open();
int penguin_chopper_close();
int penguin_chopper_isMoving();
int penguin_chopper_isAtOpen();
int penguin_chopper_isAtClose();
int penguin_chopper_getCurrentPulse();//it doesn't communicate with the instrument.
char penguin_chopper_getStatus();
int penguin_chopper_getSensorStatus();


/* medium level API */
int penguin_chopper_askStatus();
int penguin_chopper_askSensorStatus();
int penguin_chopper_getPulse();//it communicates with the instrument.
int penguin_chopper_setPulse(int pulse);
int penguin_chopper_stop();


int penguin_chopper_getParameter(int num);
int penguin_chopper_setParameter(int num, int value);

/* low level API */
int penguin_chopper_send_command(char const* cmd);
int penguin_chopper_receive();
char* penguin_chopper_getMassage();


