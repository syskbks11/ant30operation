/*!
  ¥file srvPm.cpp
  ¥date 2007.11.12
  ¥author Y.Koide
  ¥brief 
*/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <pthread.h>

#include "../libtkb/src/thrd/thrd.h"
#include "../libtkb/src/netsv/netsv.h"
#include "../libtkb/src/u/u.h"
#include "../libtkb/src/conf2/conf2.h"
#include "../libtkb/src/tm/tm.h"

#include "../powermeter_rpcomfit/penguin_powermeter.h"

static const int srvPort = 10102;
static const char srvPmConf[] = "../etc/srvPowermeter.conf";

//=== NETSV DATA FORMAT ===//

typedef struct pmreq_s{
  long size;         //!< senddata size
  int reqFlg;        //!< request flag (1:status)
} pmreq_t;

typedef struct pmans_s{
  long size;         //!< recvdata size
  int tmp;           //!< dummy
  double power;      //!< powermeter dBm
} pmans_t;

//=========================//

typedef struct srvpm_s{
  double pmInterval; //!< data taking interval

  void* conf;        //!< conf
  netServerClass_t* netsv;       //!< netsv
  thrdClass_t* thrd; //!< thred
  penguin_powermeter_t* penpowermeter; //!< powermeter

  char log[128];     //!< logfilename
  double power;      //!< data [dBm]

  int thrdRun;       //!< thread flag (0:NotRunnig 1:Running)

} srvpm_t;

static int port = 10003;

static srvpm_t* srvPmInit(const char* confFile, const int port);
static int srvPmEnd(srvpm_t* dat);
static void* thrdFunc(void* p);
static void setParam(srvpm_t* dat);

int main(int argc, char* argv[]){
  srvpm_t* srvpm;
  pmreq_t req;
  pmans_t ans;

  int ret;
  memset(&req, 0, sizeof(req));
  memset(&ans, 0, sizeof(ans));

  srvpm = srvPmInit(srvPmConf, port);
  if(srvpm==NULL) return -1;

  while(1){
    if(netsvWaiting(srvpm->netsv) < 0) continue;
    while(1){
      // Recieve request
      memset(&req, 0, sizeof(req));
      ret = netsvRead(srvpm->netsv, (unsigned char*)&req, sizeof(req));
      if(ret <= 0){
        uM1("main(); netsvRead(); return %d",ret);
        break;
      }
      uM2("main(); netsvRead(); size[%ld] reqFlg[%d]", req.size, req.reqFlg);

      if(req.reqFlg == 1){
        //! Send answer
        ans.power = srvpm->power;
        ans.size = sizeof(ans);
        uM2("main(); netsvWrite(); dBm[%lf] size[%ld]", ans.power, ans.size);
        ret = netsvWrite(srvpm->netsv, (const unsigned char*)&ans, ans.size);
        if(ret <= 0){
          uM1("main(); netsvWrite(); return %d", ret);
          break;
        }
      }
    }
    netsvDisconnect(srvpm->netsv);
  }

  srvPmEnd(srvpm);

  return 0;
}

srvpm_t* srvPmInit(const char* confFile, const int port){

  // log
  uInit("../log/srvPowermeter/srvPowermeter");

  // initialize
  srvpm_t* dat = (srvpm_t*)malloc(sizeof(srvpm_t));
  memset(dat, 0, sizeof(srvpm_t));

  // configuration
  dat->conf = NULL;
  dat->conf = conf2Init();
  conf2AddFile(dat->conf, confFile);
  setParam(dat);

  // netsv
  dat->netsv = NULL;
  dat->netsv = netsvInit(port);
  if(dat->netsv==NULL){
    uM("main(); netsvInit() failed.");
    return NULL;
  }

  // pengion modules
  dat->penpowermeter     = NULL;
  dat->penpowermeter = penguin_powermeter_init("192.168.10.3",5);
  if(dat->penpowermeter==NULL){
    uM("main(); penguin_powermeter_init() failed.");
    return NULL;
  }

  // thread
  dat->thrd = NULL;
  dat->thrd = thrdInit(thrdFunc, dat);
  if(dat->thrd == NULL){
    printf("begin thread error!!¥n");
    free(dat);
    return NULL;
  }
  thrdStart(dat->thrd);

  return dat;
}


int srvPmEnd(srvpm_t* dat){

  if(dat == NULL) return 0;

  if(dat->thrdRun){
    dat->thrdRun = 0;
    thrdEnd(dat->thrd);
    dat->thrd = NULL;
  }
  if(dat->conf){
    conf2End(dat->conf);
    dat->conf = NULL;
  }
  if(dat->penpowermeter){
    penguin_powermeter_end(dat->penpowermeter);
    dat->penpowermeter = NULL;
  }
  if(dat->netsv){
    netsvEnd(dat->netsv);
    dat->netsv = NULL;
  }

  free(dat);
  dat = NULL;

  uEnd();

  return 0;
}

void* thrdFunc(void* p){
  srvpm_t* dat = (srvpm_t*)p;
  tmClass_t* vtm;
  time_t t;
  struct tm* tmt;

  uM("time power[dBm]");
  vtm = tmInit();
  dat->thrdRun = 1;
  while(dat->thrdRun){
    if(tmGetLag(vtm) > dat->pmInterval){
      tmReset(vtm);
      time(&t);
      tmt = localtime(&t);

      dat->power = penguin_powermeter_get_power(dat->penpowermeter);

      uM1("%lf", dat->power);
    }
    tmSleepMSec(5000);
  }
  dat->thrdRun = 0;
  tmEnd(vtm);
  return 0;
}

void setParam(srvpm_t* dat){
  if(conf2SetKey(dat->conf,"port")) port = atoi(conf2GetVal(dat->conf));
  if(conf2SetKey(dat->conf,"pmInterval")) dat->pmInterval = atoi(conf2GetVal(dat->conf));
  return;
}

