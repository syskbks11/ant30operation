#ifndef TRK_H
#define TRK_H

//#include "/cosmos3/dev/45m/libsrc/trk/inc/track.h"
//#include "/cosmos3/dev/45m/libsrc/trk/inc/antvl.h"
#include "track.h"
#include "antvl.h"

#ifdef __cplusplus
extern "C"{
  int tjcd2m (char *ctime, double *dtime);
  int tjm2cd (double dtime, char *ctime);
  int trk_00 (char *cjstdate, char *cjpl, char *cerr);
  int trk_10 (int ioflg0, int ipflg0, int iaflg0, int ibjflg0, double *dsourc0, double drdlv0);
  int trk_20 (double dtime, XTRKI xin, XTRKO * xout);
  int trk_30 (double *idazel, double *idweat, double *idgdin, double idlast, double *odazel, double *odrade);
  int trk_antvl (XANTVL xvin, double *dantvl, char *cerror);
  void trk_ant_ (double *DPOOUT, double *DMAPRD, double *DMAPLB, double *DMAPAE, double *DSCNRD, double *DSCNLB,
                 double *DSCNAE, double *DXRDEC, double *DLOCAL, double *DSUNDT, double *DSUNAG);
  void trk_velo_ (double *DDPANT);
}
#else
extern int tjcd2m (char *ctime, double *dtime);
extern int tjm2cd (double dtime, char *ctime);
extern int trk_00 (char *cjstdate, char *cjpl, char *cerr);
extern int trk_10 (int ioflg0, int ipflg0, int iaflg0, int ibjflg0, double *dsourc0, double drdlv0);
extern int trk_20 (double dtime, XTRKI xin, XTRKO * xout);
extern int trk_30 (double *idazel, double *idweat, double *idgdin, double idlast, double *odazel, double *odrade);
extern int trk_antvl (XANTVL xvin, double *dantvl, char *cerror);
extern void trk_ant_ (double *DPOOUT, double *DMAPRD, double *DMAPLB, double *DMAPAE, double *DSCNRD, double *DSCNLB,
                      double *DSCNAE, double *DXRDEC, double *DLOCAL, double *DSUNDT, double *DSUNAG);
extern void trk_velo_ (double *DDPANT);
#endif


#endif /* ! TRK_H */

