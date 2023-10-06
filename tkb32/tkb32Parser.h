/*!
\file tkb32/tkb32Parser.h
\author Y.Koide
\date 2007.01.01
\brief Tsukuba 32m Central Program Functions
\brief init() -> repeat() -> end()
*/
#ifndef __TKB32PARSER_H__
#define __TKB32PARSER_H__
#ifdef __cplusplus
extern "C"{
#endif

  int setReqData(unsigned char* req, int reqSize);
  int getAnsData(unsigned char** ans, int *ansSize);

#ifdef __cplusplus
}
#endif
#endif
