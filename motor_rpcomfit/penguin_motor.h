/**
 * Communication with motor dirver HA770, software ver.1.0, Harmonic Drive Systems
 * Script for 30-cm telescope at Antarctica
 * 2008.12.10 written by NAGAI Makoto
 * 2023.04.14 updated by Shunsuke HONDA from HA730 to HA770
 * 2023.07.12 updated by Shunsuke HONDA for RPCOMFIT protcol for communication
 */

#include "rpcomfit.h"
#include "fitprot.h"

#define PULSE_360DEG 11840000 // HA770

/* object */
typedef struct penguin_motor_s{
  rpcomfitctrl* pctrl;
  int positiveSoftLimit;
  int negativeSoftLimit;
} penguin_motor_t;

penguin_motor_t* penguin_motor_init(char* ipaddress, const unsigned int deviceid, int positiveSoftLimit, int negativeSoftLimit);
void penguin_motor_fitopen(penguin_motor_t* ppenmtr);
void penguin_motor_fitclose(penguin_motor_t* ppenmtr);
int penguin_motor_initconf(penguin_motor_t* ppenmtr);
int penguin_motor_end(penguin_motor_t* ppenmtr);

/* misc functions */
#define penguin_motor_calcAngle(x) (((float)(x))/PULSE_360DEG*360)
#define penguin_motor_calcPulse(x) ((int)((x)/360*PULSE_360DEG))

void print_err(const char* const err);
void print_alarm(const int ecrs);

/* high level API */
int penguin_motor_checkIOState(penguin_motor_t* ppenmtr);
int penguin_motor_checkErrorCorrecState(penguin_motor_t* ppenmtr);
int penguin_motor_checkDynamicBreakState(penguin_motor_t* ppenmtr);
int penguin_motor_checkAlarmState(penguin_motor_t* ppenmtr);
int penguin_motor_checkModeState(penguin_motor_t* ppenmtr);
int penguin_motor_checkParameter(penguin_motor_t* ppenmtr);

/* medium level API */
void penguin_motor_setPulse(penguin_motor_t* ppenmtr, int value);
int penguin_motor_servOn(penguin_motor_t* ppenmtr);
int penguin_motor_servOff(penguin_motor_t* ppenmtr);

#define penguin_motor_moveToPositiveLimit(x) (penguin_motor_setCommand((x), "P",(x)->positiveSoftLimit))
#define penguin_motor_moveToNegativeLimit(x) (penguin_motor_setCommand((x), "P",(x)->negativeSoftLimit))

#define penguin_motor_setVelocity(x,v)       (penguin_motor_setCommand((x),"VF",(v)))

#define penguin_motor_getPulse(x)            (penguin_motor_sendrecvData_int((x), "PMON"))
#define penguin_motor_getVelocity(x)         (penguin_motor_sendrecvData_int((x), "VMON"))
#define penguin_motor_getNominalVelocity(x)  (penguin_motor_sendrecvData_int((x), "VF"))
#define penguin_motor_getIOState(x)          (penguin_motor_sendrecvData_hexint((x), "IO"))
#define penguin_motor_getErrorCorrecState(x) (penguin_motor_sendrecvData_int((x), "COMP"))

#define penguin_motor_stop(x)                (penguin_motor_sendData((x), "HOLDON"))
#define penguin_motor_start(x)               (penguin_motor_sendData((x), "HOLDOFF"))
#define penguin_motor_goOriginPositive(x)    (penguin_motor_sendData((x), "ZF"))
#define penguin_motor_goOriginNegative(x)    (penguin_motor_sendData((x), "ZR"))
#define penguin_motor_goOrigin(x)            (penguin_motor_sendData((x), "ZSTRON"))
#define penguin_motor_stopOriginSearch(x)    (penguin_motor_sendData((x), "STOP"))
#define penguin_motor_clearAlarm(x)          (penguin_motor_sendData((x), "ARST"))

#define penguin_motor_setPosiLoopConst(x,v)  (penguin_motor_setParameter((x),  2, (v)))
#define penguin_motor_setVeloLoopConst(x,v)  (penguin_motor_setParameter((x),  3, (v)))
#define penguin_motor_setGain(x,v)           (penguin_motor_setParameter((x),  4, (v)))
#define penguin_motor_setAbsoluteMode(x)     (penguin_motor_setParameter((x), 17, (0)))
#define penguin_motor_setIncrementalMode(x)  (penguin_motor_setParameter((x), 17, (1)))
#define penguin_motor_setJogVelocity(x,v)    (penguin_motor_setParameter((x), 30, (v)))
#define penguin_motor_setDynamicBreakOff(x)  (penguin_motor_setParameter((x), 56, (1)))
#define penguin_motor_setDynamicBreakOn(x)   (penguin_motor_setParameter((x), 56, (2)))
#define penguin_motor_setErrorCorrecOn(x)    (penguin_motor_setParameter((x), 70, (0))))
#define penguin_motor_setErrorCorrecOff(x)   (penguin_motor_setParameter((x), 70, (1))))

#define penguin_motor_getPosiLoopConst(x)    (penguin_motor_getParameter((x),  2))
#define penguin_motor_getVeloLoopConst(x)    (penguin_motor_getParameter((x),  3))
#define penguin_motor_getGain(x)             (penguin_motor_getParameter((x),  4))
#define penguin_motor_getMode(x)             (penguin_motor_getParameter((x), 17))
#define penguin_motor_getJogVelocity(x)      (penguin_motor_getParameter((x), 30))
#define penguin_motor_getDynamicBreak(x)     (penguin_motor_getParameter((x), 56))

#define penguin_motor_getDeviceid(x)         ((x)->pctrl->deviceid)

#define penguin_motor_isServOn(x)            (penguin_motor_getIOState_outbit((x),0))
#define penguin_motor_isInPosition(x)        (penguin_motor_getIOState_outbit((x),1))
#define penguin_motor_isReady(x)             (penguin_motor_getIOState_outbit((x),4))
#define penguin_motor_isOriginDetected(x)    (penguin_motor_getIOState_outbit((x),5))
#define penguin_motor_isPositiveLimit(x)     (penguin_motor_getIOState_inbit((x),1))
#define penguin_motor_isNegativeLimit(x)     (penguin_motor_getIOState_inbit((x),2))
#define penguin_motor_isLimit(x)             ((penguin_motor_getIOState_inbit((x),1))&(penguin_motor_getIOState_inbit((x),2)))

int penguin_motor_getAlarmHistory(penguin_motor_t* ppenmtr, const unsigned int number);
int penguin_motor_getAlarmState(penguin_motor_t* ppenmtr);
int penguin_motor_getModeState(penguin_motor_t* ppenmtr);
int penguin_motor_getIOState_outbit(penguin_motor_t* ppenmtr, const unsigned int bitnumber);
int penguin_motor_getIOState_inbit(penguin_motor_t* ppenmtr, const unsigned int bitnumber);

/* low level API */
void penguin_motor_setCommand(penguin_motor_t* ppenmtr, const char* const cmd, const int value);
int  penguin_motor_setParameter(penguin_motor_t* ppenmtr, const int parameter, const int value);
int  penguin_motor_getParameter(penguin_motor_t* ppenmtr, const int param);

/* lowest level API */
int penguin_motor_sendData(penguin_motor_t* ppenmtr, const char* const cmd);
int penguin_motor_sendrecvData(penguin_motor_t* ppenmtr, const char* const cmd, char* buffer);
int penguin_motor_sendrecvData_int(penguin_motor_t* ppenmtr, const char* const cmd);
int penguin_motor_sendrecvData_hexint(penguin_motor_t* ppenmtr, const char* const cmd);

/* non-supported API */
int penguin_motor_getPreviousCommand(penguin_motor_t* ppenmtr, char* buffer);
int penguin_motor_checkPreviousCommand(penguin_motor_t* ppenmtr, const char* cmd);



