/*!
\file trkTest.cpp
\date 2006.12.28
\author Y.Koide
\brief trk testProgram
*/
#include <stdlib.h>
#include <stdio.h>
#include "../u/u.h"
#include "../conf/conf.h"
#include "trk.h"

static char cenvdt[3][128] = {
  "ANTFILE=./etc/ant_err.dat",
  "EPHFILE=./etc/ephfile/19241221-20500102.ORG",
  "TIMEFILE=./etc/time.dat"
};

int MAIN__(){
  //!ä¬ã´ïœêîê›íË
  for (int i = 0; i < 3; i++) {
    if (putenv(cenvdt[i]) != 0) {
      printf("Cannot add value to environment (%s)\n", cenvdt[i]);
      throw "";
    }
  }

  uInit("./log/trkTest");
  confInit();
  confAddFile("./etc/tkb32.conf");
  confAddFile("./etc/test.device");
  confAddFile("./etc/test.scan");
  confAddFile("./etc/test.source");
  confPrint();

  int ONE[1]={1};
  trkInit(ONE);
  trkStart();
  while(1){
    trkRepeat();
    //break;
    /*char s[3];
    printf("1: trkGetACU\n");
    printf("2: trkSetACU\n");
    printf("3: trkSetAcuMood\n");
    printf("4: trkSetAcuBan\n");
    printf("5: trkSetAcuStow\n");
    printf("9: quit\n");
    printf("input ->");
    scanf("%s\n",s);
    puts("");
    s[1]=NULL;
    switch(stoi(s);){
    case 1:
      trkGetACU();
      break;
    case 2:
      trkSetACU();
      break;
    case 3:
      printf("0: standby\n");
      printf("(2): programu tracking\n");
      printf("3: stowLock\n");
      printf("9: quit\n");
      printf("input ->");
      scanf("%s\n",s);
      puts("");
      s[1]=NULL;
      if(atoi(s)==0 || atoi(s)==3)
	trkSetAcuMood(atoi(s));
      break;
    case 4:
      printf("0: permit run\n");
      printf("1: ban run\n");
      printf("input ->");
      scanf("%s\n",s);
      s[1]=NULL;
      if(atoi(s)=='0' || atoi(s)=='1')
	trkSetAcuBan(atoi(s));
      break;
    case 5:
      trkSetAcuStow();
      break;
    }
    if(atoi(s)==9)
      break;
    */
  }
  trkEnd();

  return 0;
}
