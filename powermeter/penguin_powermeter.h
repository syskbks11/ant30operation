/**
 * Communication with Powermeter, E4418B, Agilent Co., Ltd.
 * 2009.8.6 written by NAGAI Makoto
 * For 30-cm telescope. 
 */


#ifdef __cplusplus
extern "C"{
#endif

  /*
   * singleton, since we have only one weather monitor. 
   */
  int penguin_powermeter_init(int cliantGPIBAddress);
  int penguin_powermeter_end();

  /* higher level API */
  int penguin_powermeter_freerun();
  double penguin_powermeter_freerun_getData();



  /* low level API */
  int penguin_CmdTxCommand(const char* cmd);
  int penguin_CmdRxData();
  char* penguin_chopper_getMessage();

#ifdef __cplusplus
}
#endif

