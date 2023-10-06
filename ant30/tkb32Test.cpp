/*!
\file tkb32/tkb32Test.cpp
\author Y.Koide
\date 2007.01.01
\brief Tsukuba 32m Central Program Test Version
*/

#include <memory.h>
#include "../u/u.h"
#include "../conf/conf.h"
#include "tkb32Func.h"

//typedef struct sParamTkb32Test{
//}tParamTkb32Test;

//static   tParamTkb32Test p;

int MAIN__(void){
  const char dir[] = "../etc";
  char tmp[256];
  
  try{
    confInit();
    sprintf(tmp, "%s/tkb32.conf", dir);
    if(confAddFile(tmp)){
      throw "";
    }
    //sprintf(tmp, "%s/%s", dir, confGetStr("SysConfig"));
    //if(confAddFile(tmp)){
    //  throw "";
    //}
    sprintf(tmp, "%s/test.obs", dir);
    if(confAddFile(tmp)){
      throw "";
    }
    sprintf(tmp, "%s/%s", dir, confGetStr("DeviceTable"));
    if(confAddFile(tmp)){
      throw "";
    }
    sprintf(tmp, "%s/%s", dir, confGetStr("SourceTable"));
    if(confAddFile(tmp)){
      throw "";
    }
    sprintf(tmp, "%s/%s", dir, confGetStr("ScanTable"));
    if(confAddFile(tmp)){
      throw "";
    }
  }
  catch(...){
    confPrint();
    uM("Error in read parameters");
    return -1;
  }

  try{
    init();
    start();
  }
  catch(...){
    uM("Error in init();");
    end();
    return -1;
  }

  try{
    repeat(NULL);
  }
  catch(...){
    uM("Error in repeat();");
    end();
    return -1;
  }

  try{
    end();
  }
  catch(...){
    uM("Error in end();");
    return -1;
  }
  return 0;
}
