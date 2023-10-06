/*!
\file srvIf2Test.c
\author Y.Koide
\date 2007.01.08
\brief 2ndIF controle program
*/
#ifdef DEBUG
# pragma comment (lib, "import/libtkbD.lib")
#else
# pragma comment (lib, "import/libtkb.lib")
#endif

#pragma comment (lib, "import/gpibWdm.lib")

#include <stdio.h>
#include <memory.h>
#include "import/libtkb.h"
#include "import/gpibWdm.h"

const int srvPort = 10005;
static void* net = NULL;

int main(int argc, char* argv[]){
  const int myId = 0;
  const int if2Id = 20;
  double ans[2];
  double req[2];
  int ret;
  const int tmpSize = 256;
  char tmp[256];

  memset(ans, 0, sizeof(double)*2);

  //sprintf("./%s", argv[0]);
  uInit("srvIf2");
  uM2("This program is Compiled at %s %s", __DATE__, __TIME__);
  ret = gpibWdmInit(myId);
  if(ret){
    uM1("main(); gpibWdmInit(); return %d", ret);
    return -1;
  }
  gpibWdmWrite(if2Id, "*RST");
  gpibWdmWrite(if2Id, "OUTP:STAT ON");
  net=netsvInit(srvPort);
  uEnd();
  if(net==NULL)
    return -1;
  while(1){
    if(netsvWaiting(net) < 0){
      continue;
    }
    uInit("srvIf2");
    gpibWdmWrite(if2Id, "OUTP:STAT ON");
    while(1){
      memset(req, 0, sizeof(double)*3);
      ret = netsvRead(net, (unsigned char*)req, sizeof(double)*2);
      if(ret <= 0){
        uM1("main(); netsvRead(); return %d",ret);
        break;
      }
      uM2("main(); req FREQ=%.10lf GHz POW=%5.1lf dBm", req[0], req[1]);
      if(req[0] >= 0){
        //! Set Lo Freq
        sprintf(tmp, "FREQ %.10e GHz", req[0]);
        gpibWdmWrite(if2Id, tmp);
      }
      if(req[1] >= 0){
        //! Set Lo Amp
        sprintf(tmp, "POW:AMPL %.2e dBm", req[1]);
        gpibWdmWrite(if2Id, tmp);
      }
      gpibWdmWrite(if2Id, "FREQ:CW?");
      gpibWdmRead(if2Id, tmp, tmpSize);
      sscanf(tmp, "%lf", &ans[0]);
      ans[0] /= 1.0e9; //!< [Hz] -> [GHz]
      gpibWdmWrite(if2Id, "POW:AMPL?");
      gpibWdmRead(if2Id, tmp, tmpSize);
      sscanf(tmp, "%lf", &ans[1]);

      uM2("main(); ans FREQ=%.10lf GHz POW=%5.1lf dBm", ans[0], ans[1]);
      ret = netsvWrite(net, (const unsigned char*)ans, sizeof(double)*2);
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
