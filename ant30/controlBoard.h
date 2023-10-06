/*!
\file controlBoard.h
\author NAGAI Makoto
\date 2009.06.10
\brief For Tsukuba 32-m telescope & Antarctic 30-cm telescope 
*/
#ifndef __CONTROLBOARD_H__
#define __CONTROLBOARD_H__
#ifdef __cplusplus
extern "C"{
#endif

//!< 各装置を使用するか 0:使用しない, 1:使用, 2:通信以外使用
#define CONTROL_BOARD_USE_NO 0
#define CONTROL_BOARD_USE_FULL 1
#define CONTROL_BOARD_USE_LOGIC 2

/* Each implementation is in controlBoard.cpp */
int controlBoardInit();

// low level API
int controlBoardGetWeather();
int controlBoardGetAntenna();
int controlBoardGetChopper();
int controlBoardGetFrontEnd();
int controlBoardGet1stIF();
int controlBoardGet2ndIF();
int controlBoardGetBackEnd();


#ifdef __cplusplus
}
#endif
#endif
