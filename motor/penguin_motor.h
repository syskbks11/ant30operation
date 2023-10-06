/**
 * Communication with motor dirver HA770, software ver.??, Harmonic Drive Systems
 * Script for 30-cm telescope at Antarctica
 * 2008.12.10 written by NAGAI Makoto
 * 2023.04.14 updated by Shunsuke HONDA from HA730 to HA770
 * 2023.05.01 updated by Shunsuke HONDA for RPCOMFIT protcol for communication
 */

#define ALST_OK    0
#define ECRS_NO    0
#define ECRS_WAIT    2
#define ECRS_YES    3

#define PULSE_360DEG 11840000 // HA770

/* object */
typedef struct penguin_motor_s penguin_motor_t;

penguin_motor_t* penguin_motor_init(const unsigned int devId, int negativeSoftLimit, int positiveSoftLimit);
int penguin_motor_end(penguin_motor_t* pm);
int penguin_motor_getID(const penguin_motor_t* pm);

/* high level API */
int penguin_motor_configureInput(const penguin_motor_t* pm);

int penguin_motor_checkDOState(const penguin_motor_t* pm);
int penguin_motor_checkErrorCorrectionState(const penguin_motor_t* pm);
int penguin_motor_checkAlermState(const penguin_motor_t* pm);
void penguin_motor_moveToPositiveLimit(const penguin_motor_t* pm);
void penguin_motor_moveToNegativeLimit(const penguin_motor_t* pm);
int penguin_motor_checkDynamicBreakState(const penguin_motor_t* pm);
int penguin_motor_getDynamicBreakState(const penguin_motor_t* pm);
int penguin_motor_getJogVelocity(const penguin_motor_t* pm);
int penguin_motor_getGain(const penguin_motor_t* pm);
int penguin_motor_getVelocityLoopConstant(const penguin_motor_t* pm);
int penguin_motor_getPositionLoopConstant(const penguin_motor_t* pm);
int penguin_motor_getAlermState(const penguin_motor_t* pm);
int penguin_motor_getAlermHistory(const penguin_motor_t* pm, const unsigned int number);
int penguin_motor_getModeState(const penguin_motor_t* pm);

/* medium level API */
#define penguin_motor_getPulseActual(x)    (penguin_motor_send_receive_int_value((x), "PMON", 4))
#define penguin_motor_getVelocityActual(x)    (penguin_motor_send_receive_int_value((x), "VMON", 4))
#define penguin_motor_getVelocity(x)        (penguin_motor_send_receive_int_value((x), "V", 1))
#define penguin_motor_getDOState(x)        (penguin_motor_send_receive_hexint_value((x), "IO", 2))
#define penguin_motor_getErrorCorrectionState(x)    (penguin_motor_send_receive_int_value((x), "COMP", 4))
#define penguin_motor_setSERVOFF(x)    (penguin_motor_buffered_write((x), "SVOFF", 5))
#define penguin_motor_setSERVON(x)    (penguin_motor_buffered_write((x), "SVON", 4))
#define penguin_motor_stop(x)  (penguin_motor_buffered_write((x), "HOLDON",  6))
#define penguin_motor_start(x) (penguin_motor_buffered_write((x), "HOLDOFF", 7))
#define penguin_motor_goOriginPositive(x)    (penguin_motor_buffered_write((x), "ZF", 2))
#define penguin_motor_goOriginNegative(x)    (penguin_motor_buffered_write((x), "ZR", 2))
#define penguin_motor_clearAlerm(x)        (penguin_motor_buffered_write((x), "ARST", 4))
#define penguin_motor_stop(x)  (penguin_motor_buffered_write((x), "HOLDON",  6))
//#define penguin_motor_setAbsoluteMode(x)	(penguin_motor_buffered_write((x), "ABS", 3))
//#define penguin_motor_setIncrementalMode(x)	(penguin_motor_buffered_write((x), "INC", 3))

void penguin_motor_setDynamicBreakOFF(const penguin_motor_t* pm);
void penguin_motor_setDynamicBreakON(const penguin_motor_t* pm);

void penguin_motor_setVelocity(const penguin_motor_t* pm, int velocity);
void penguin_motor_setJogVelocity(const penguin_motor_t* pm, int velocity);
void penguin_motor_setPulse(const penguin_motor_t* pm, int velocity);

void penguin_motor_setGain(const penguin_motor_t* pm, int velocity);
void penguin_motor_setPositionLoopConstant(const penguin_motor_t* pm, int velocity);
void penguin_motor_setVelocityLoopConstant(const penguin_motor_t* pm, int velocity);
void penguin_motor_setAbsoluteMode(const penguin_motor_t* pm);
void penguin_motor_setIncrementalMode(const penguin_motor_t* pm);

int penguin_motor_checkPreviousCommand(const penguin_motor_t* pm, const char* cmd);

/* low level API */
int penguin_motor_getPreviousCommand(const penguin_motor_t* pm, char* buffer);
void penguin_motor_setParameter(const penguin_motor_t* pm, int parameter, int value);
int penguin_motor_getParameter(const penguin_motor_t* pm, int param);


/* lowest level API */
void penguin_motor_buffered_write(const penguin_motor_t* pm, char* buf, const unsigned int size);
int penguin_motor_send_receive(const penguin_motor_t* pm, char* buf, const unsigned int bufSize, char* buffer);
int penguin_motor_send_receive_int_value(const penguin_motor_t* pm, char* cmd, const unsigned int cmdSize);
int penguin_motor_send_receive_hexint_value(const penguin_motor_t* pm, char* cmd, const unsigned int cmdSize);

