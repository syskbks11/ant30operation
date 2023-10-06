#
#  ����ۡ���ǥ��쥯�ȥ�
#
TRK_HOME        = /home/nagai/workspace/libtrk/trk45
#TRK_HOME        = /usr/trk45

#  ���󥯥롼�ɥե�����ǥ��쥯�ȥ�
TRK_INCLUDE	= $(TRK_HOME)/include

#  ����ѥ���̾
#CC		= /usr/bin/gcc
#FC		= /usr/bin/gcc
CC		= /usr/local/FFC/bin/fcc
FC		= /usr/local/FFC/bin/frt

#  ����ѥ��饪�ץ����
CFLAGS		= -I$(TRK_INCLUDE)
FFLAGS		= -I$(TRK_INCLUDE)
#CFLAGS		= -g -I$(TRK_INCLUDE) -I$(FX_INCLUDE) -DDEBUG
#FFLAGS		= -g -I$(TRK_INCLUDE) -DDEBUG

#  ��������
TARGET		= ../libtrk.a

#  �������ե�����
CSRCS1		= tjcd2m.c tjm2cd.c
CSRCS2		= trk_00.c trk_02.c trk_03_cmt.c trk_10.c trk_20.c trk_20_cmt.c trk_30.c\
		  trk_antvl.c
FSRCS1		= R_0000.F R_1300.F R_2200.F R_3100.F\
		  R_3200.F R_3210.F R_4200.F R_4300.F\
		  R_3300.F R_3310.F R_3320.F R_3330.F\
		  R_3340.F R_3400.F R_3500.F R_1200.F\
		  R_4400.F R_4500.F R_4500_cmt.F R_4600.F R_5100.F R_5110.F\
		  R_5200.F R_5300.F R_5400.F R_5410.F\
		  R_6100.F R_6200.F R_6300.F R_6400.F\
		  R_7100.F R_8100.F R_4100.F
FSRCS2		= trk_01.F   trk_11.F   trk_12.F  trk_21.F\
		  trk_22rd.F trk_22lb.F trk_23.F  trk_24.F\
		  trk_26.F   trk_31.F\
		  trk_frq.F  trk_ant.F  trk_velo.F
FSRCS3		= EI_305.F EI_320.F EI_330.F EI_350.F\
		  EI_360.F EI_370.F EI_371.F EI_375.F\
		  EI_381.F EI_382.F\
		  O_MSG2.F

# ���֥������ȥե�����
COBJS1		= ${CSRCS1:.c=.o}
COBJS2		= ${CSRCS2:.c=.o}
COBJS		= $(COBJS1) $(COBJS2)
FOBJS1		= ${FSRCS1:.F=.o}
FOBJS2		= ${FSRCS2:.F=.o}
FOBJS3		= ${FSRCS3:.F=.o}
FOBJS		= $(FOBJS1) $(FOBJS2) $(FOBJS3)

OBJS		= $(COBJS) $(FOBJS)

#  ���󥯥롼�ɥե�����
HFILES		= $(TRK_INCLUDE)/EI_ATCN\
		  $(TRK_INCLUDE)/EI_LATR\
		  $(TRK_INCLUDE)/EI_LMAI\
		  $(TRK_INCLUDE)/EI_TRMT\
		  $(TRK_INCLUDE)/EI_TTME\
		  $(TRK_INCLUDE)/XCNUTA\
		  $(TRK_INCLUDE)/XDFWRK\
		  $(TRK_INCLUDE)/XDUMMY\
		  $(TRK_INCLUDE)/XEPHEM\
		  $(TRK_INCLUDE)/XGLCST\
		  $(TRK_INCLUDE)/XIERST\
		  $(TRK_INCLUDE)/XMATHC\
		  $(TRK_INCLUDE)/XOBSEI\
		  $(TRK_INCLUDE)/XOBSIN\
		  $(TRK_INCLUDE)/XPRCNT\
		  $(TRK_INCLUDE)/XSTINF\
		  $(TRK_INCLUDE)/XTMERT\
		  $(TRK_INCLUDE)/XTRK00\
		  $(TRK_INCLUDE)/XTRKTP\
		  $(TRK_INCLUDE)/XUNTCV\
		  $(TRK_INCLUDE)/XVWORK\
		  $(TRK_INCLUDE)/XANTEN\
		  $(TRK_INCLUDE)/XOTHER\
		  $(TRK_INCLUDE)/__DFTF\
		  $(TRK_INCLUDE)/track.h\
		  $(TRK_INCLUDE)/trkmod.h\
		  $(TRK_INCLUDE)/trkmsg.h\
		  $(TRK_INCLUDE)/dopin.h\
		  $(FX_INCLUDE)/fx_xprm.h

#  ����ѥ���
$(TARGET) : $(OBJS)
	ar rc $(TARGET) $(OBJS)
#$(COBJS1) : $(CSRCS1) $(HFILES)
#	$(CC) -c $(CFLAGS) $<
#$(COBJS2) : $(CSRCS2) $(HFILES)
#	$(CC) -c $(CFLAGS) $<
#$(FOBJS1) : $(FSRCS1) $(HFILES)
#	$(FC) -c $(FFLAGS) $<
#$(FOBJS2) : $(FSRCS2) $(HFILES)
#	$(FC) -c $(FFLAGS) $<
#$(FOBJS3) : $(FSRCS3) $(HFILES)
#	$(FC) -c $(FFLAGS) $<

clean:
	/bin/rm -f *.o core *.BAK
	
INSTALL_DIR = /home/nagai/workspace/libtrk/trk45
#INSTALL_DIR = /usr/trk45
install:
	/bin/rm -f $(INSTALL_DIR)/include/*
	/bin/rm -f $(INSTALL_DIR)/lib/libtrk.a
	/bin/rm -f $(INSTALL_DIR)/lib/srctrk/*
	/bin/cp ../../include/* $(INSTALL_DIR)/include
	/bin/cp ../libtrk.a     $(INSTALL_DIR)/lib
	/bin/cp *.o             $(INSTALL_DIR)/lib/srctrk
	/bin/chmod 644 $(INSTALL_DIR)/include/*
	/bin/chmod 644 $(INSTALL_DIR)/lib/libtrk.a
	/bin/chmod 644 $(INSTALL_DIR)/lib/srctrk/*.o
