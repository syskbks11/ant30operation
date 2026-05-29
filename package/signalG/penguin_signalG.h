/**
 * Communication with signal generator SMB100A, Rohde & Schwarz GmbH.
 * 2008.12.11 written by NAGAI Makoto
 * For 30-cm telescope. 
 */



/*
 * singleton, since we have only one signal generator. 
 */
int penguin_signalG_init();
int penguin_signalG_end();



/* high level API */
void penguin_signalG_setFrequency(double frequency);
void penguin_signalG_setPower(double power);
void penguin_signalG_setOutput(int isOn);
double penguin_signalG_getFrequency();
double penguin_signalG_getPower();

/* medium level API */
int penguin_signalG_send_and_receive_int(const char* cmd);


/* low level API */
char* penguin_signalG_getMassage();
long penguin_signalG_send_and_receive(const char* cmd);
long penguin_signalG_receive();
int penguin_signalG_send_command(const char* cmd);

