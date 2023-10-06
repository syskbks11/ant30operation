/*!
\file rs232c.h
\author Y.Koide
\date 2006.08.15
\brief RS-232C‚ğ§Œä‚·‚éƒ‰ƒCƒuƒ‰ƒŠ
*/
#ifndef __RS232C_H__
#define __RS232C_H__

#ifdef __cplusplus
extern "C"{
#endif

  const char* rs232cVersion();
  int rs232cInit(int comNo, int bps, int byteSize, int parityBit, int stopBit);
  int rs232cEnd(int comNo);
  int rs232cWrite(int comNo, const unsigned char* buf, const unsigned int bufSize);
  int rs232cRead(int comNo, unsigned char* buf, const unsigned int bufSize);

#ifdef __cplusplus
}
#endif

#endif
