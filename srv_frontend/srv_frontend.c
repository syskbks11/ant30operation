/*!
  ¥file srvFe.cpp
  ¥date 2007.11.12
  ¥author Y.Koide
  ¥brief フロントエンド制御プログラム
*/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <pthread.h>

//#include "../libtkb/export/libtkb.h"
#include "../libtkb/src/thrd/thrd.h"
#include "../libtkb/src/netsv/netsv.h"
#include "../libtkb/src/u/u.h"
#include "../libtkb/src/conf2/conf2.h"
#include "../libtkb/src/tm/tm.h"

#include "../gauge_rpcomfit/penguin_gauge.h"
#include "../lakeshore_rpcomfit/penguin_lakeshore.h"

static const int srvPort = 10003;
static const char srvFeConf[] = "../etc/srvFrontEnd.conf";

//=== NETSV DATA FORMAT ===//

typedef struct fereq_s{
  long size;         //!< senddata size
  int reqFlg;        //!< request flag (1:status)
} fereq_t;

typedef struct feans_s{
  long size;         //!< recvdata size
  int tmp;           //!< dummy
  double temp[8];    //!< frontend temperature K
  double pres;       //!< frontend pressure Pa
} feans_t;

//=========================//

typedef struct srvfe_s{
  double feInterval; //!< data taking interval

  void* conf;        //!< conf
  netServerClass_t* netsv;       //!< netsv
  thrdClass_t* thrd; //!< thred
  penguin_lakeshore_t* penlakeshore; //!< lakeshore
  penguin_gauge_t* pengauge;         //!< gauge

  char log[128];     //!< logfilename
  double pres;       //!< data [Pa]
  double temp[8];    //!< data [K]

  int thrdRun;       //!< thread flag (0:NotRunnig 1:Running)

} srvfe_t;

static int port = 10003;
static int mode = 3; //1: normal (lakeshore & gauge), 0: lakeshore only

static srvfe_t* srvFeInit(const char* confFile, const int port, const int mode);
static int srvFeEnd(srvfe_t* dat);
static void* thrdFunc(void* p);
static void setParam(srvfe_t* dat);

int main(int argc, char* argv[]){
  srvfe_t* srvfe;
  fereq_t req;
  feans_t ans;

  int ret;
  unsigned int i = 0;
  memset(&req, 0, sizeof(req));
  memset(&ans, 0, sizeof(ans));

  srvfe = srvFeInit(srvFeConf, port, mode);
  if(srvfe==NULL) return -1;

  while(1){
    if(netsvWaiting(srvfe->netsv) < 0) continue;
    while(1){
      // Recieve request
      memset(&req, 0, sizeof(req));
      ret = netsvRead(srvfe->netsv, (unsigned char*)&req, sizeof(req));
      if(ret <= 0){
        uM1("main(); netsvRead(); return %d",ret);
        break;
      }
      uM2("main(); netsvRead(); size[%ld] reqFlg[%d]", req.size, req.reqFlg);

      if(req.reqFlg == 1){
        //! Send answer
        for(i=0;i<8;i++) ans.temp[i] = srvfe->temp[i];
        ans.pres = srvfe->pres;
        ans.size = sizeof(ans);
        uM3("main(); netsvWrite(); K[%lf] Pa[%e] size[%ld]", ans.temp[0], ans.pres, ans.size);
        ret = netsvWrite(srvfe->netsv, (const unsigned char*)&ans, ans.size);
        if(ret <= 0){
          uM1("main(); netsvWrite(); return %d", ret);
          break;
        }
      }
    }
    netsvDisconnect(srvfe->netsv);
  }

  srvFeEnd(srvfe);

  return 0;
}

srvfe_t* srvFeInit(const char* confFile, const int port, const int mode){

  // log
  uInit("../log/srvFrontEnd/srvFrontEnd");

  // initialize
  srvfe_t* dat = (srvfe_t*)malloc(sizeof(srvfe_t));
  memset(dat, 0, sizeof(srvfe_t));

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
  dat->penlakeshore = NULL;
  dat->pengauge     = NULL;
  if(mode & 1){
    dat->penlakeshore = penguin_lakeshore_init("192.168.10.3",4);
    if(dat->penlakeshore==NULL){
      uM("main(); penguin_lakeshore_init() failed.");
      return NULL;
    }
  }
  if(mode & 2){
    dat->pengauge = penguin_gauge_init("192.168.10.3",5);
    if(dat->pengauge==NULL){
      uM("main(); penguin_gauge_init() failed.");
      return NULL;
    }
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


int srvFeEnd(srvfe_t* dat){

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
  if(dat->penlakeshore){
    penguin_lakeshore_end(dat->penlakeshore);
    dat->penlakeshore = NULL;
  }
  if(dat->pengauge){
    penguin_gauge_end(dat->pengauge);
    dat->pengauge = NULL;
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
  srvfe_t* dat = (srvfe_t*)p;
  tmClass_t* vtm;
  time_t t;
  struct tm* tmt;

  uM("time temperature[K] pressure[Pa]");
  vtm = tmInit();
  dat->thrdRun = 1;
  while(dat->thrdRun){
    if(tmGetLag(vtm) > dat->feInterval){
      tmReset(vtm);
      time(&t);
      tmt = localtime(&t);

      dat->pres = penguin_gauge_get_pressure(dat->pengauge);
      dat->temp[0] = penguin_lakeshore_get_temperature(dat->penlakeshore,1);
      dat->temp[1] = penguin_lakeshore_get_temperature(dat->penlakeshore,2);
      dat->temp[2] = penguin_lakeshore_get_temperature(dat->penlakeshore,3);
      dat->temp[3] = penguin_lakeshore_get_temperature(dat->penlakeshore,4);
      dat->temp[4] = penguin_lakeshore_get_temperature(dat->penlakeshore,5);
      dat->temp[5] = penguin_lakeshore_get_temperature(dat->penlakeshore,6);
      dat->temp[6] = penguin_lakeshore_get_temperature(dat->penlakeshore,7);
      dat->temp[7] = penguin_lakeshore_get_temperature(dat->penlakeshore,8);

      uM4("%lf,%lf,%lf,%lf", dat->temp[0], dat->temp[1], dat->temp[2], dat->temp[3]);
      uM4("%lf,%lf,%lf,%lf", dat->temp[4], dat->temp[5], dat->temp[6], dat->temp[7]);
      uM1("%4.2E", dat->pres);
    }
    tmSleepMSec(5000);
  }
  dat->thrdRun = 0;
  tmEnd(vtm);
  return 0;
}

void setParam(srvfe_t* dat){
  if(conf2SetKey(dat->conf,"port")) port = atoi(conf2GetVal(dat->conf));
  if(conf2SetKey(dat->conf,"feInterval")) dat->feInterval = atoi(conf2GetVal(dat->conf));
  return;
}

