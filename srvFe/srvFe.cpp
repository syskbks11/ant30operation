/*!
\file srvFe.cpp
\date 2007.11.12
\author Y.Koide
\brief フロントエンド制御プログラム
*/
#ifdef DEBUG
# pragma comment (lib, "../libtkb/Debug/libtkb.lib")
#else
# pragma comment (lib, "../libtkb/Release/libtkb.lib")
#endif

#pragma comment (lib, "import/libtkb.lib")

#ifdef WIN32
# include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include "../db1000/db1000.h"
#include "../m431/m431.h"
#include "import/libtkb.h"

const int comDB1000 = 1;
const int comM431 = 2;
const int srvPort = 10003;
const char srvFeConf[] = "srvFe.conf";

typedef struct feReq_s{
  long size;             //!< 送信サイズ
  int reqFlg;            //!< リクエストフラグ 1:Status
} feReq_t;

typedef struct feAns_s{
  long size;             //!< 受信サイズ
  int tmp;               //!< 予備(ダミー)
  double K;              //!< フロントエンド温度 K
  double Pa;              //!< フロントエンド気圧 Pa
} feAns_t;
 
//! feで使用される構造体
typedef struct srvFeDat_s{
  //! パラメータ
  //int port;               //!< サーバポート番号
  double feInterval;      //!< データ取得間隔
  int comDB1000;          //!< DB1000のシリアルポート番号
  int comM431;            //!< M431のシリアルポート番号

  //! 変数
  void* conf;             //!< conf用
  void* netsv;            //!< netsv用
  void* thrd;             //!< thredのポインタ
  CM431* m431;
  CDB1000* db1000;

  char log[128];          //!< データを記録する.csvファイルの名前
  double Pa;              //!< 気圧 Pa
  double K;               //!< 温度 K
  int thrdRun;            //!< thredのフラグ 0:NotRunnig 1:Running
  //unsigned int thrdId;    //!< thredId
} srvFeDat_t;

int port;

static void* srvFeInit(const char* confFile);
static int srvFeEnd();
static const double srvFeM431Pa(void* dat);
static const double srvFeDB1000K(void* dat);
static void* thrdFunc(void* p);
static void setParam(const char** keyVal, srvFeDat_t* dat);

int main(int argc, char argv[]){
  void* srvFe;
  void* netsv;
  feReq_t req;                //!< Requestデータ保管用
  feAns_t ans;                //!< Answerデータ
  int reqSize = sizeof(req);  //!< reqのバイト数
  int ansSize = sizeof(ans);  //!< ansのバイト数
  int size = 0;               //!< 取得バイト数のカウント 
  int ret;
  
  //! ログ初期化
  uInit("srvFe");

  //! フロントエンドモニター初期化
  srvFe = srvFeInit(srvFeConf);

  //! ネットワーク初期化
  netsv = netsvInit(port);
  netsvSetTimeOut(netsv, 1);

  while(1){
    if(netsvWaiting(netsv) != 0){
      tmSleepMSec(1000);
      continue;
    }
    while(1){
      //! Recive request
      const int bufSize = 256;
      unsigned char buf[bufSize] = {0};
      size = 0;
      int i;
      for(i = 0; i < 5; i++){
        ret = netsvRead(netsv, buf + size, reqSize - size);
        if(ret < 0){
          uM1("main(); netsvRead(); return %d",ret);
          break;
        }
        size += ret;
        if(ret == reqSize){
          break;
        }
      }
      if(i == 5 || ((feReq_t*)buf)->size != reqSize){
        uM3("main(); Request error!! reqSize[%d] size[%d] req.size[%d]", reqSize, size, req.size);
        ret = netsvRead(netsv, buf, bufSize);
      }
      memcpy(&req, buf, sizeof(req));
      //uM2("main(); netsvRead(); reqFlg[%d] size[%d]", req.reqFlg, req.size);

      if(req.reqFlg == 1){
        //! Send answer
        ans.K = srvFeDB1000K(srvFe);
        ans.Pa = srvFeM431Pa(srvFe);
        ans.size = sizeof(ans);
        //uM3("main(); netsvWrite(); K[%lf] Pa[%e] size[%d]", ans.K, ans.Pa, ans.size);
        ret = netsvWrite(netsv, (const unsigned char*)&ans, ans.size);
        if(ret != ans.size){
          uM2("main(); netsvWrite(); [%d] != ans.size[%d]", ret, ans.size);
          break;
        }
      }
    }
    netsvDisconnect(netsv); 
  }
  netsvEnd(netsv);
  return 0;
}

void* srvFeInit(const char* confFile){
	time_t t;
	struct tm* tmt;
  srvFeDat_t* dat;

  //! データ構造体初期化
  try{
    dat = new srvFeDat_t();
  }
  catch(...){
    uM("srvFeInit(); new srvFeDat_t() error!!");
    return NULL;
  }
  memset(dat, 0, sizeof(srvFeDat_t));

  //! パラメータ取得
  dat->conf = conf2Init();
  conf2AddFile(dat->conf, confFile);
  setParam(conf2GetAllKeyVal2(dat->conf), dat);

  //! logファイル
  time(&t);
	tmt=localtime(&t);
	sprintf(dat->log, "srvFe-%04d%02d%02d_%02d%02dLog.csv", tmt->tm_year+1900, tmt->tm_mon+1, tmt->tm_mday, tmt->tm_hour, tmt->tm_min);

  dat->m431 = new CM431(dat->comM431);
  dat->db1000 = new CDB1000(dat->comDB1000);

  dat->thrd = thrdInit(thrdFunc, dat);
  if(dat->thrd == 0){
    printf("begin thread error!!\n");
    delete dat;
    return NULL;
  }
  thrdStart(dat->thrd);

  return dat;
}


int srvFeEnd(void* _dat){
  srvFeDat_t* dat = (srvFeDat_t*)_dat;

  if(dat){
    if(dat->thrdRun){  
      dat->thrdRun = 0;
      thrdEnd(dat->thrd);
      dat->thrd = NULL;
    }
    if(dat->m431){
      delete dat->m431;
      dat->m431 = NULL;
    }
    if(dat->db1000){
      delete dat->db1000;
      dat->db1000 = NULL;
    }
    if(dat->conf){
      conf2End(dat->conf);
      dat->conf = NULL;
    }
    delete dat;
    dat = NULL;
  }
  return 0;
}

const double srvFeM431Pa(void* dat){
  return (const double)((srvFeDat_t*)dat)->Pa;
}

const double srvFeDB1000K(void* dat){
  return (const double)((srvFeDat_t*)dat)->K;
}

void* thrdFunc(void* p){
  srvFeDat_t* dat = (srvFeDat_t*)p;
  void* vtm;
  FILE* fp;
  time_t t;
  struct tm* tmt;

  vtm = tmInit();
  if((fp = fopen(dat->log,"a")) != NULL){//! 追加書き込みする。
    time(&t);
    tmt = localtime(&t);
    dat->Pa = dat->m431->getPa();
    dat->K = dat->db1000->getK();

    fprintf(fp,"%s", ctime(&t));//! ctimeの終わりになぜか改行コードが入っている
    fprintf(fp,"\"time\",\"K\",\"Pa\"\n");
    fprintf(fp,"\"%02d/%02d-%02d:%02d:%02d\",%lf,%lf\n",
      tmt->tm_mon+1, tmt->tm_mday, tmt->tm_hour, tmt->tm_min, tmt->tm_sec, dat->K, dat->Pa);
    printf("[%02d/%02d\-%02d:%02d:%02d] : %lf K, %4.2E Pa\n",
      tmt->tm_mon+1, tmt->tm_mday,tmt->tm_hour, tmt->tm_min, tmt->tm_sec, dat->K, dat->Pa);
    fclose(fp);
  }

  dat->thrdRun = 1;
  while(dat->thrdRun){
    if(tmGetLag(vtm) > dat->feInterval){
      tmReset(vtm);
      time(&t);
      tmt = localtime(&t);
      dat->Pa = dat->m431->getPa();
      dat->K = dat->db1000->getK();

      if((fp = fopen(dat->log,"a")) != NULL){//! 追加書き込みする。
        //char timeStr[128];
        //char tmp[1024];
        //strcpy(timeStr, ctime(&t));
        //timeStr[strlen(timeStr)-1]='\0';//! 末尾の\n文字を\0で上書き
        fprintf(fp,"\"%02d/%02d-%02d:%02d:%02d\",%lf,%lf\n",
          tmt->tm_mon+1, tmt->tm_mday, tmt->tm_hour, tmt->tm_min, tmt->tm_sec, dat->K, dat->Pa);
        printf("[%02d/%02d\-%02d:%02d:%02d] : %lf K, %4.2E Pa\n",
          tmt->tm_mon+1, tmt->tm_mday,tmt->tm_hour, tmt->tm_min, tmt->tm_sec, dat->K, dat->Pa);
        fclose(fp);
      }
      else{
        printf("thrdFunc(); fileOpenError[%s]\n", dat->log);
      }
    }
    tmSleepMSec(5000);
  }
  dat->thrdRun = 0;
  tmEnd(vtm);
  return 0;
}

/*!
\brief パラメータ取得
*/
void setParam(const char** keyVal, srvFeDat_t* dat){
  const char* key;
  const char* val;
  int i = 0;

  while(*keyVal[i] != NULL){
    key = keyVal[i];
    val = keyVal[i+1];
    i += 2;

    if(!strcmp(key, "port")){
      port = atoi(val);
    }
    else if(!strcmp(key, "feInterval")){
      dat->feInterval = atof(val);
    }
    else if(!strcmp(key, "comDB1000")){
      dat->comDB1000 = atoi(val);
    }
    else if(!strcmp(key, "comM431")){
      dat->comM431 = atoi(val);
    }
  }

  return;
}