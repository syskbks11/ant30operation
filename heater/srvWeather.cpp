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
#include "penguin_chopper.h"

const int srvPort = 10004;
static netServerClass_t* net = NULL;



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

	ret = penguin_chopper_init();
	//ret = gpibWdmInit(myId);

	if(ret){
		uM1("main(); penguin_signalG_init(); return %d", ret);
		return -1;
	}
	penguin_chopper_send_command("*RST");
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
		uInit("srvSignalG");
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
			//gpibWdmWrite(if1Id, tmp);
			//! Set Lo Amp
			sprintf(tmp, "POW:AMPL %.2e dBm", reqLoAmp);
			//gpibWdmWrite(if1Id, tmp);


			//_setStepAttenuator(reqAtt);

			//! Step Attのデータを取得できない場合の処理
			ans[2] = reqAtt;
			//! Step Attのデータを取得できる場合の処理
			//ans[2] = _getCurrentAttenuation();

			//! Get Lo Status
			//gpibWdmWrite(if1Id, "FREQ:CW?");
			//gpibWdmRead(if1Id, tmp, tmpSize);
			sscanf(tmp, "%lf", &ans[0]);
			ans[0] /= 1.0e9; //!< [Hz] -> [GHz]

			//gpibWdmWrite(if1Id, "POW:AMPL?");
			//gpibWdmRead(if1Id, tmp, tmpSize);
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
	//gpibWdmEnd();
	penguin_chopper_end();
	netsvEnd(net);
}
