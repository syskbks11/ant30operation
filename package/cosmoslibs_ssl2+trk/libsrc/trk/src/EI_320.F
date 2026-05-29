      SUBROUTINE EI_320 ( DMJSEC )                                      
C----------------------------------------------------------------------C
C     MODULE NAME : EI_320                                             C
C                                                                      C
C     FUNCTION    : CONVERSIONS BETWEEN THE TIME SCALES,               C
C                   JST , UTC , TAI , UT1 , TDB , AND GAST             C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1990.09.04                      C
C                                                                      C
C     -NOTE-                                                           C
C                                                                      C
C      LAST MUST BE CALCULATED WHEN THE LONGITUDE OF AN OBSERVER       C
C      IS GIVEN.                                                       C
C                                                                      C
C----------------------------------------------------------------------C
C                                       *------------------------------*
C                                       * DECLARE DIVITION             *
C                                       *------------------------------*
#include      "__DFTF"                                                
C                                       * ARGUMENT VARIABLE DEFINITION *
      REAL*8        DMJSEC              ! 日本標準時のＭＪＤ
C                                       * COMMON AREA OF INSTRUCTION   *
#include      "EI_TTME"                                               
#include      "XTMERT"                                                
#include      "XPRCNT"                                                
#include      "XMATHC"                                                
#include      "XUNTCV"                                                
C                                                                       
C                                       * WORK VARIABLE DEFINITION     *
C                                                                       
      REAL*8        DJDUT1, DJDTDT, DXUT1, DEQ, DALPH, DWK,
     +              DACNST, DAFRST, DASCND, DATHRD
      REAL*8        DWRK
C                                                                 
C グリニッジ平均恒星時多項式係数
      DATA   DACNST , DAFRST , DASCND , DATHRD /
     +       4.8949612120958383D00 ,  6.2833197068884093D02 ,           
     +       6.7707139449033359D-6 , -4.5087672343186847D-10  /         
C
C
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C
C
C ＭＪＤ０時からの通算秒
      DWK = DMOD( DMJSEC, 86400.0D0 )

C  ＵＴＣ、ＴＤＢ、ＴＡＩ、ＵＴ１、ＴＤＴでのＭＪＤ
      DTMUJD = DMJSEC / 86400.0D0
      DJDTAI = ( DMJSEC + DFLOAT( ILEAPT ) ) / 86400.0D0
      DJDUT1 = ( DMJSEC + DFLOAT( ILEAPT ) + DCORPR ) / 86400.0D0
      DJDTDT = DJDTAI + 3.725D-04
      DTDBJD = DJDTDT
C     write(*,*) 'EI_320', DTMUJD, DJDTAI, DJDUT1, DJDTDT, DTDBJD
C
C ＴＤＢ、ＵＴ１でのユリウス世紀
      DCENTR( 1 ) = ( DTDBJD - 51544.5D0 ) / 36525.0D0
      DCENTR( 2 ) = ( DJDUT1 - 51544.5D0 ) / 36525.0D0
C
C 章動の計算
      CALL R_2200
C
C グリニッジ視恒星時
      DXUT1  = ( DWK + DFLOAT( ILEAPT ) + DCORPR ) * DHSRAD
      DWRK   = DAFRST * DCENTR( 2 )
      DWRK   = DMOD( DWRK, DTWOPI )
      DALPH  =   DACNST + DWRK
     +         + DASCND * DCENTR( 2 )**2
     +         + DATHRD * DCENTR( 2 )**3
      DEQ    = DNTLON * DCOS( DMNOBL + DNTOBL )
      DWK    = DPIVAL + DXUT1
      DWRK   = DMOD( DWK , DTWOPI )
      DWK    = DWRK + DALPH + DEQ
C ２πの不確定性を解く
      DXGAST = DMOD( DWK , DTWOPI )
C
      IF ( DXGAST .LT. 0D0 ) THEN
        DXGAST = DXGAST + DTWOPI
      ENDIF
C
C 終了
      RETURN
      END
