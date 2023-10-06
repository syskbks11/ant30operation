/*!
\file srvIf1.cpp
\author Y.Koide
\date 2007.01.08
\brief 1stIF controle program
*/
#ifdef DEBUG
# pragma comment (lib, "import/libtkbD.lib")
#else
# pragma comment (lib, "import/libtkb.lib")
#endif

#pragma comment (lib, "import/gpibWdm.lib")

#include <stdio.h>
#include <memory.h>
#include <string.h>
#include "import/libtkb.h"
#include "import/gpibWdm.h"

const int srvPort = 10004;
static void* net = NULL;

int main(int argc, char* argv[]){
  const int myId = 0;
  const int if1Id = 19;
  const int attId = 28;
  double req[3];          //!< LoFreq[GHz], LoAmp[dBm], StepAtt[dB]
  double ans[3];          //!< LoFreq[GHz], LoAmp[dBm], StepAtt[dB]
  double reqLoFreq = 0;
  double reqLoAmp = 0;
  double reqAtt = 0;
  double ansAtt = 0;
  const int tmpSize = 256;
  char tmp[256];
  char* c;
  int ret;

  memset(ans, 0, sizeof(double)*3);
  
  //sprintf("./%s", argv[0]);
  uInit("srvIf1");
  uM2("This program is Compiled at %s %s", __DATE__, __TIME__);
  ret = gpibWdmInit(myId);
  if(ret){
    uM1("main(); gpibWdmInit(); return %d", ret);
    return -1;
  }
  gpibWdmWrite(if1Id, "*RST");
  gpibWdmWrite(if1Id, "OUTP:STAT ON");
  net = netsvInit(srvPort);
  if(net==NULL)
    return -1;
  uEnd();
  while(1){
    if(netsvWaiting(net) < 0){
      continue;
    }
    uInit("srvIf1");
    gpibWdmWrite(if1Id, "OUTP:STAT ON");
    while(1){
      memset(req, 0, sizeof(double)*3);
      ret = netsvRead(net, (unsigned char*)req, sizeof(double)*3);
      if(ret <= 0){
        uM1("main(); netsvRead(); return %d",ret);
        break;
      }
      uM3("main(); req FREQ=%.10lf GHz POW=%5.1lf dBm Att=%5.1lf dB", req[0], req[1], req[2]);

      //! Get Request
      if(req[0] > 0){
        reqLoFreq = req[0];
      }
      if(req[1] > 0){
        reqLoAmp = req[1];
      }
      if(req[2] <= 0){
        reqAtt = req[2];
      }

      //! Set Lo Freq
      sprintf(tmp, "FREQ %.10e GHz", reqLoFreq);
      gpibWdmWrite(if1Id, tmp);
      //! Set Lo Amp
      sprintf(tmp, "POW:AMPL %.2e dBm", reqLoAmp);
      gpibWdmWrite(if1Id, tmp);
      //! Set Step Att 
      int att = -reqAtt;                //!< [dB]->[-dB]
      sprintf(tmp, "A");                //!< ONにするスイッチ番号の列挙
      if(((att % 4) % 2) == 1){
        sprintf(tmp + strlen(tmp), "1");
      }
      if(((att % 4) / 2) == 1){
        sprintf(tmp + strlen(tmp), "2");
      }
      if((att / 4) >= 2){
        sprintf(tmp + strlen(tmp), "34");      
      }
      else if((att / 4) >= 1){
        sprintf(tmp + strlen(tmp), "3");      
      }
      sprintf(tmp + strlen(tmp), "B");  //!< OFFにするスイッチ番号の列挙
      if(((att % 4) % 2) != 1){
        sprintf(tmp + strlen(tmp), "1");
      }
      if(((att % 4) / 2) != 1){
        sprintf(tmp + strlen(tmp), "2");
      }
      if((att / 4) < 1){
        sprintf(tmp + strlen(tmp), "34");      
      }
      else if((att / 4) < 2){
        sprintf(tmp + strlen(tmp), "4");      
      }
      sprintf(tmp + strlen(tmp), "567890");
      uM1("%s", tmp);
      gpibWdmWrite(attId, tmp);
      
      //! Step Attのデータを取得できない場合の処理
      ans[2] = reqAtt;
      //! Step Attのデータを取得できる場合の処理
      /*tmp[0] = '\0';
      gpibWdmRead(attId, tmp, tmpSize);
      c = tmp;
      ansAtt = 0
      while(*c++ != '\0'){
        if(*c == 'A'){
          flg = 1;
        }
        else if(*c == 'B'){
          flg = -1;
        }
        else if(flg){
          int n = *c - '0';
          switch(n){
            case 1:
              ansAtt += 1;
              break;
            case 2:
              ansAtt += 2;
              break;
            case 3:
            case 4:
              ansAtt += 4;
              break;
            default:
          }
        }
      }
      ans[2] = ansAtt;
      */

      //! Get Lo Status
      gpibWdmWrite(if1Id, "FREQ:CW?");
      gpibWdmRead(if1Id, tmp, tmpSize);
      sscanf(tmp, "%lf", &ans[0]);
      ans[0] /= 1.0e9; //!< [Hz] -> [GHz]

      gpibWdmWrite(if1Id, "POW:AMPL?");
      gpibWdmRead(if1Id, tmp, tmpSize);
      sscanf(tmp, "%lf", &ans[1]);

      uM3("main(); ans FREQ=%.10lf GHz POW=%5.1lf dBm Att=%5.1lf dB", ans[0], ans[1], ans[2]);
      ret = netsvWrite(net, (const unsigned char*)ans, sizeof(double)*3);
      if(ret <= 0){
        uM1("main(); netsvWrite(); return %d", ret);
        break;
      }
    }
    netsvDisconnect(net);
    uEnd();
  }
  gpibWdmEnd();
  netsvEnd(net);
}
