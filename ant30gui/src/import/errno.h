/*!
\file errno.h
\date 2007.02.09
\author Y.Koide
\brief Defien error code number
*/
#ifndef __ERRNO_H__
#define __ERRNO_H__

//! Common error code (0 ~ 9 , -1 ~ -19)
#define NONE_ERR 0
#define NOT_USE NONE_ERR

#define NET_ERR -1  
#define NET_DAT_ERR -2
#define NET_TIME_OUT_ERR -3
#define COM_ERR -4
#define COM_DAT_ERR -5
#define GPIB_ERR -6
#define THRD_ERR -7
#define PARAM_ERR -8

#define TRK00_ERR -10
#define TRK10_ARG_ERR -11
#define TRK10_COMAREA_ERR -12
#define TRK20_ARG_ERR -13
#define TRK20_LOCAL_ERR -14
#define TRK20_EL_ERR -15
#define TRK30_LOCAL_ERR -16
#define TJCD2M_ERR -17


//! weath (-20 ~ -29)
#define WEATH_NOT_USE NOT_USE

#define WEATH_COM_ERR COM_ERR
#define WEATH_COM_DAT_ERR COM_DAT_ERR
#define WEATH_THRD_ERR THRD_ERR
#define WEATH_PARAM_ERR PARAM_ERR
//#define WEATH_THRD_RAN_ERR 0


//! trk (-30 ~ -39)
#define TRK_NOT_USE NOT_USE

#define TRK_COM_ERR COM_ERR
#define TRK_COM_DAT_ERR COM_DAT_ERR
#define TRK_THRD_ERR THRD_ERR
#define TRK_PARAM_ERR PARAM_ERR
#define TRK_TRK00_ERR TRK00_ERR
#define TRK_TRK10_ARG_ERR TRK10_ARG_ERR
#define TRK_TRK10_COMAREA_ERR TRK10_COMAREA_ERR
#define TRK_TRK20_ARG_ERR TRK20_ARG_ERR
#define TRK_TRK20_LOCAL_ERR TRK20_LOCAL_ERR
#define TRK_TRK20_EL_ERR TRK20_EL_ERR
#define TRK_TRK30_LOCAL_ERR TRK30_LOCAL_ERR
#define TRK_TJCD2M_ERR TJCD2M_ERR
//#define TRK_THRD_RAN_ERR 0
#define TRK_THRD_STOPPED_ERR -31
#define TRK_SET_ERR -32
#define TRK_SAFETY_ERR -33
#define TRK_LIMIT_AZ_ERR -34
#define TRK_LIMIT_EL_ERR -35
#define TRK_LIMIT_AZ_EL_ERR -36
#define TRK_TRACKING_TIMEOUT_ERR -37
#define TRK_STANDBY_ERR -38

//! chop (-40 ~ -49)
#define CHOP_NOT_USE NOT_USE

#define CHOP_NET_ERR NET_ERR
#define CHOP_NET_DAT_ERR NET_DAT_ERR
#define CHOP_PARAM_ERR PARAM_ERR
#define CHOP_SET_ERR -40
#define CHOP_STATE_ERR -41
#define CHOP_MOVING_TIMEOUT_ERR -42

//! fe (-50 ~ -59)
#define FE_NOT_USE NOT_USE

#define FE_NET_ERR NET_ERR
#define FE_NET_DAT_ERR NET_DAT_ERR
#define FE_PARAM_ERR PARAM_ERR
#define FE_NOT_INIT_ERR -50

//! if (-60 ~ -69)
#define IF_NOT_USE NOT_USE

#define IF_NET_ERR NET_ERR
#define IF_NET_DAT_ERR NET_DAT_ERR
#define IF_PARAM_ERR PARAM_ERR
#define IF_TRK00_ERR TRK00_ERR
#define IF_TRK10_ARG_ERR TRK10_ARG_ERR
#define IF_TRK10_COMAREA_ERR TRK10_COMAREA_ERR
#define IF_TRK20_ARG_ERR TRK20_ARG_ERR
#define IF_TRK20_LOCAL_ERR TRK20_LOCAL_ERR
#define IF_TJCD2M_ERR TJCD2M_ERR

#define IF_STATE_ERR -61

//! saacq (-70 ~ -79)
#define SAACQ_NOT_USE NOT_USE

#define SAACQ_NET_ERR NET_ERR
#define SAACQ_NET_DAT_ERR NET_DAT_ERR
#define SAACQ_NET_TIMEOUT_ERR NET_TIME_OUT_ERR
#define SAACQ_PARAM_ERR PARAM_ERR
#define SAACQ_REJECT -70
#define SAACQ_STATE_ERR -71
#define SAACQ_NOT_INIT_ERR -72

#ifdef __cplusplus
extern "C"{
#endif

  const char* errnoStr(int errno);
  const char* errnoEpoch(int epoch);
  const char* errnoCoord(int coord);
  const char* errnoFrame(int frame);
  const char* errnoDefinition(int definition);
  const char* errnoTrkPZoneStr(int zone);
  const char* errnoTrkRZoneStr(int zone);
  const char* errnoTrkACUStatusStr(int status, int statusNo);
  const char* errnoTrkTrackingStr(int trackstat);
  const char* errnoChopChopStr(int chop);
  const char* errnoChopMoterStr(int moter);
  const char* errnoSaacqExe(int exe);
  const char* errnoSaacqErr(int err);

#ifdef __cplusplus
}
#endif
#endif
