/*!
¥file srvFe.cpp
¥date 2007.11.12
¥author Y.Koide
¥brief フロントエンド制御プログラム
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
#include <pthread.h>
#include "../libtkb/export/libtkb.h"
#include "../gauge/penguin_gauge.h"
#include "../lakeshore/penguin_lakeshore.h"

static const int srvPort = 10003;
static const char srvFeConf[] = "../etc/srvFrontEnd.conf";

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
  double feInterval;      //!< データ取得間隔

  //! 変数
  void* conf;             //!< conf用
  void* netsv;            //!< netsv用
  thrdClass_t* thrd;             //!< thredのポインタ

  char log[128];          //!< データを記録する.csvファイルの名前
  double Pa;              //!< 気圧 Pa
  double K1;               //!< 温度 K
  double K2;               //!< 温度 K
  double K3;               //!< 温度 K
  int thrdRun;            //!< thredのフラグ 0:NotRunnig 1:Running
  //unsigned int thrdId;    //!< thredId
} srvFeDat_t;

static int port;
static int mode = 1;//1: normal (lakeshore & gauge), 0: lakeshore only


static void* srvFeInit(const char* confFile);
static int srvFeEnd(void* dat);
static const double srvFrontEndPressure(void* dat);
static const double srvFrontEndTemperature(void* dat);
static void* thrdFunc(void* p);
static void setParam(srvFeDat_t* dat);
//static void setParam(const char** keyVal, srvFeDat_t* dat);

int main(int argc, char argv[]){
  void* srvFe;
  netServerClass_t* netsv;
  feReq_t req;                //!< Requestデータ保管用
  feAns_t ans;                //!< Answerデータ

/*
  int reqSize = sizeof(req);  //!< reqのバイト数
  int ansSize = sizeof(ans);  //!< ansのバイト数
*/
/*
  int size = 0;               //!< 取得バイト数のカウント 
*/
  int ret;

  
  //! ログ初期化
  uInit("../log/srvFrontEnd/srvFrontEnd");//090813 in
/* 090813 out
  uInit("../log/srvFrontEnd");
*/
	if(argc == 2){
		uM("main(); run as LAKESHORE ONLY mode.");
		mode = 0;
	}

	memset(&ans, 0, sizeof(ans));
	
	if(mode){//090131 in
		if(penguin_gauge_init()){
			uM("main(); penguin_gauge_init() failed.");
			exit(1);
		}
	}
	if(penguin_lakeshore_init()){
		uM("main(); penguin_lakeshore_init() failed.");
		exit(1);
	}

  //! フロントエンドモニター初期化
	srvFe = srvFeInit(srvFeConf);
		//printf("main();\n");

  //! ネットワーク初期化
	netsv = netsvInit(port);

	if(netsv==NULL)//081219 in
		return -1;

/*
	netsvSetTimeOut(netsv, 1);
*/
	while(1){
		if(netsvWaiting(netsv) < 0){//081219 in
			continue;
		}
/* 081219 out
		if(netsvWaiting(netsv) != 0){
			tmSleepMSec(1000);
			continue;
		}
*/
		while(1){
      //! Recive request
			memset(&req, 0, sizeof(req));
			ret = netsvRead(netsv, (unsigned char*)&req, sizeof(req));
			if(ret <= 0){
				uM1("main(); netsvRead(); return %d",ret);
				break;
			}
			uM2("main(); netsvRead(); size[%ld] reqFlg[%d]", req.size, req.reqFlg);
/*
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
*/
      //uM2("main(); netsvRead(); reqFlg[%d] size[%d]", req.reqFlg, req.size);

			if(req.reqFlg == 1){
				//continue;
		      //! Send answer
			ans.K = srvFrontEndTemperature(srvFe);
			ans.Pa = srvFrontEndPressure(srvFe);
			ans.size = sizeof(ans);
			uM3("main(); netsvWrite(); K[%lf] Pa[%e] size[%ld]", ans.K, ans.Pa, ans.size);
			ret = netsvWrite(netsv, (const unsigned char*)&ans, ans.size);
			if(ret <= 0){
				uM1("main(); netsvWrite(); return %d", ret);
				break;
			}
			}
/*
			if(ret != ans.size){
				uM2("main(); netsvWrite(); [%d] != ans.size[%d]", ret, ans.size);
				break;
			}
*/
/*
			if(req.reqFlg == 1){
			      //! Send answer
				ans.K = srvFrontEndTemperature(srvFe);
				ans.Pa = srvFrontEndPressure(srvFe);
				ans.size = sizeof(ans);
				//uM3("main(); netsvWrite(); K[%lf] Pa[%e] size[%d]", ans.K, ans.Pa, ans.size);
				ret = netsvWrite(netsv, (const unsigned char*)&ans, ans.size);
				if(ret != ans.size){
					uM2("main(); netsvWrite(); [%d] != ans.size[%d]", ret, ans.size);
					break;
				}
			}
*/
		}
		netsvDisconnect(netsv); 
	}
	netsvEnd(netsv);
	srvFeEnd(srvFe);
	uEnd();
	return 0;
}

void* srvFeInit(const char* confFile){
/*
	time_t t;
	struct tm* tmt;
*/
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
	confInit();
	confAddFile(confFile);
	setParam(dat);
/*
  dat->conf = conf2Init();
  conf2AddFile(dat->conf, confFile);
		printf("srvFeInit();\n");
  setParam(conf2GetAllKeyVal2(dat->conf), dat);
		printf("srvFeInit();\n");
*/
  //! logファイル
/*
  time(&t);
	tmt=localtime(&t);
	sprintf(dat->log, "srvFe-%04d%02d%02d_%02d%02dLog.csv", tmt->tm_year+1900, tmt->tm_mon+1, tmt->tm_mday, tmt->tm_hour, tmt->tm_min);
*/


  dat->thrd = thrdInit(thrdFunc, dat);
  if(dat->thrd == 0){
    printf("begin thread error!!¥n");
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
    if(dat->conf){
      conf2End(dat->conf);
      dat->conf = NULL;
    }
    delete dat;
    dat = NULL;
  }
	penguin_lakeshore_end();
	penguin_gauge_end();
  return 0;
}

const double srvFrontEndPressure(void* dat){
  return (const double)((srvFeDat_t*)dat)->Pa;
}

const double srvFrontEndTemperature(void* dat){
  return (const double)((srvFeDat_t*)dat)->K1;
}

void* thrdFunc(void* p){
  srvFeDat_t* dat = (srvFeDat_t*)p;
  tmClass_t* vtm;
/*
  FILE* fp;
*/
  time_t t;
  struct tm* tmt;

	uM("time temperature[K] pressure[Pa]");
  vtm = tmInit();
/*
  if((fp = fopen(dat->log,"a")) != NULL){//! 追加書き込みする。
    time(&t);
    fprintf(fp,"%s", ctime(&t));//! ctimeの終わりになぜか改行コードが入っている
*/
/* 081219 removed
    tmt = localtime(&t);
	dat->Pa = penguin_gauge_get_pressure(1);
	dat->K1 = penguin_lakeshore_get_temperature(1);
	dat->K2 = penguin_lakeshore_get_temperature(2);
	dat->K3 = penguin_lakeshore_get_temperature(3);

    fprintf(fp,"\"time\",\"K\",\"Pa\"\n");
        fprintf(fp,"\"%02d/%02d-%02d:%02d:%02d\",%lf,%lf,%lf,%lf\n",
          tmt->tm_mon+1, tmt->tm_mday, tmt->tm_hour, tmt->tm_min, tmt->tm_sec, dat->K1, dat->K2, dat->K3, dat->Pa);
        printf("[%02d/%02d-%02d:%02d:%02d] : %lf K, %lf K, %lf K, %4.2E Pa\n",
          tmt->tm_mon+1, tmt->tm_mday,tmt->tm_hour, tmt->tm_min, tmt->tm_sec, dat->K1, dat->K2, dat->K3, dat->Pa);
    fclose(fp);
  }
*/
  dat->thrdRun = 1;
  while(dat->thrdRun){
    if(tmGetLag(vtm) > dat->feInterval){
      tmReset(vtm);
      time(&t);
      tmt = localtime(&t);
	if(mode){
		dat->Pa = penguin_gauge_get_pressure(1);
	}else{
		dat->Pa = -1;
	}
	dat->K1 = penguin_lakeshore_get_temperature(1);
	dat->K2 = penguin_lakeshore_get_temperature(2);
	dat->K3 = penguin_lakeshore_get_temperature(3);
	double K5 = penguin_lakeshore_get_temperature(5);
	double K6 = penguin_lakeshore_get_temperature(6);
	
	uM4("%lf,%lf,%lf,%4.2E", dat->K1, dat->K2, dat->K3, dat->Pa);
	uM2("%lf,%lf", K5, K6);
//	uM6("%lf,%lf,%lf,%lf,%lf,%4.2E", dat->K1, dat->K2, dat->K3, K5, K6, dat->Pa);
/*
      if((fp = fopen(dat->log,"a")) != NULL){//! 追加書き込みする。
        //char timeStr[128];
        //char tmp[1024];
        //strcpy(timeStr, ctime(&t));
        //timeStr[strlen(timeStr)-1]='¥0';//! 末尾の¥n文字を¥0で上書き
        fprintf(fp,"\"%02d/%02d-%02d:%02d:%02d\",%lf,%lf,%lf,%lf\n",
          tmt->tm_mon+1, tmt->tm_mday, tmt->tm_hour, tmt->tm_min, tmt->tm_sec, dat->K1, dat->K2, dat->K3, dat->Pa);
        printf("[%02d/%02d-%02d:%02d:%02d] : %lf K, %lf K, %lf K, %4.2E Pa\n",
          tmt->tm_mon+1, tmt->tm_mday,tmt->tm_hour, tmt->tm_min, tmt->tm_sec, dat->K1, dat->K2, dat->K3, dat->Pa);
        fclose(fp);
      }
      else{
        printf("thrdFunc(); fileOpenError[%s]\n", dat->log);
      }
*/
    }
    tmSleepMSec(5000);
  }
  dat->thrdRun = 0;
  tmEnd(vtm);
  return 0;
}

/*!
¥brief パラメータ取得
*/
void setParam(srvFeDat_t* dat){
	if(confSetKey("port"))
		port = atoi(confGetVal());
	if(confSetKey("feInterval"))
		dat->feInterval = atoi(confGetVal());
	return;
}
/*
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
	}

	return;
}
*/

