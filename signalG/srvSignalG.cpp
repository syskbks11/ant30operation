/*!
¥file srvSignalG.cpp
* written by NAGAI Makoto
* 2008.12.11
* for 30-cm telescope
* 
* original version is written for 32-m telescope by
¥author Y.Koide
¥date 2007.01.08
¥brief 1stIF controle program
*/

#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <time.h>
#include "../libtkb/src/libtkb.h"
#include "penguin_signalG.h"

const int srvPort = 10004;
static netServerClass_t* net = NULL;

static void _setStepAttenuator(double reqAtt);
static double _getCurrentAttenuation();

void _setStepAttenuator(double reqAtt){
	char tmp[256];
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
	//gpibWdmWrite(attId, tmp);

}

double _getCurrentAttenuation(){
	/*tmp[0] = '¥0';
	gpibWdmRead(attId, tmp, tmpSize);
	c = tmp;
	ansAtt = 0
	while(*c++ != '¥0'){
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
	return ansAtt;
	*/
}

int main(int argc, char* argv[]){
//	const int myId = 0;
//	const int if1Id = 19;
//	const int attId = 28;
	double req[3];          //!< LoFreq[GHz], LoAmp[dBm], StepAtt[dB]
	double ans[3];          //!< LoFreq[GHz], LoAmp[dBm], StepAtt[dB]
	double reqLoFreq = 0;
	double reqLoAmp = 0;
	double reqAtt = 0;
//	double ansAtt = 0;
//	const int tmpSize = 256;
	char tmp[256];
//	char* c;
	int ret;

	int outputMode = 1;//normal 1, no output 0

	memset(ans, 0, sizeof(double)*3);
  
	//sprintf("./%s", argv[0]);
	if(argc > 1){
		printf("NO OUTPUT MODE: This mode is for test, are you sure??\n");
		outputMode = 0;
	}

	uInit("../log/srvSignalG/srvSignalS");//090813 in
/* 090813 out
	uInit("../log/srvSignal");
*/
	uM2("This program is Compiled at %s %s", __DATE__, __TIME__);
	uM1("output mode is %d", outputMode);

	ret = penguin_signalG_init();
	//ret = gpibWdmInit(myId);

	if(ret){
		uM1("main(); penguin_signalG_init(); return %d", ret);
		return -1;
	}
	penguin_signalG_send_command("*RST");
	//gpibWdmWrite(if1Id, "*RST");
	//gpibWdmWrite(if1Id, "OUTP:STAT ON");

	net = netsvInit(srvPort);
	if(net==NULL)
		return -1;
	uEnd();
	while(1){
		if(netsvWaiting(net) < 0){
			continue;
		}
		uInit("../log/srvSignalG/srvSignalG");//090813 in
/* 090813 out
		uInit("../log/srvSignalG");
*/
		penguin_signalG_setOutput(outputMode);
		penguin_signalG_setPower(2.0);//default value for 30-cm telescope
		//gpibWdmWrite(if1Id, "OUTP:STAT ON");
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
			penguin_signalG_setFrequency(reqLoFreq);
			//gpibWdmWrite(if1Id, tmp);
			//! Set Lo Amp
			sprintf(tmp, "POW:AMPL %.2e dBm", reqLoAmp);
			penguin_signalG_setPower(reqLoAmp);
			//gpibWdmWrite(if1Id, tmp);


			_setStepAttenuator(reqAtt);

			//! Step Attのデータを取得できない場合の処理
			ans[2] = reqAtt;
			//! Step Attのデータを取得できる場合の処理
			//ans[2] = _getCurrentAttenuation();

			//! Get Lo Status
			//gpibWdmWrite(if1Id, "FREQ:CW?");
			//gpibWdmRead(if1Id, tmp, tmpSize);
			//sscanf(tmp, "%lf", &ans[0]);
			ans[0] = penguin_signalG_getFrequency();
			ans[0] /= 1.0e9; //!< [Hz] -> [GHz]

			//gpibWdmWrite(if1Id, "POW:AMPL?");
			//gpibWdmRead(if1Id, tmp, tmpSize);
			//sscanf(tmp, "%lf", &ans[1]);
			ans[1] = penguin_signalG_getPower();

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
	//gpibWdmEnd();
	netsvEnd(net);
}
