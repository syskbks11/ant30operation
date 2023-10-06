      SUBROUTINE R_3200 ( CESTNM )
C----------------------------------------------------------------------C
C     MODULE NAME : R_3200                                             C
C                                                                      C
C     FUNCTION    : DERIVE EMISSION TIME AT A PLANET, CORRECTED FOR    C
C                   THE LIGHT TIME                                     C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1990.09.14                      C
C                                                                      C
C     -NOTE-                                                           C
C                                                                      C
C      NONE                                                            C
C                                                                      C
C----------------------------------------------------------------------C
C                                       *------------------------------*
C                                       * DECLARE DIVITION             *
C                                       *------------------------------*
#include      "__DFTF"
C                                       * ARGUMENT VARIABLE DEFINITION *
      CHARACTER  CESTNM*16              ! 天体名
C                                       * COMMON AREA OF INSTRUCTION   *
#include      "XIERST"
#include      "XTMERT"
#include      "XEPHEM"
#include      "XVWORK"
C
C                                       * WORK VARIABLE DEFINITION     *
C
      REAL*8        DNMEAR, DNMPLT, DPV, DFN, DAY, DLT, DUMMY(3)
      INTEGER*2     JC
      CHARACTER     CSUN*16
C                                       * CONSTANT DATA DEFINITION     *
      DATA    CSUN / 'SUN' /
      DATA    DAY  / 86400D0 /
C
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C
C 惑星の太陽からの位置ベクトル
C       地球からの位置ベクトルを計算
      DO 1000 JC = 1, 3                                                 
        DPLHCP(JC) = DPLPOS(JC) - DSLPOS(JC)                            
        DPLGEP(JC) = DPLHCP(JC) - DOPHEL(JC)                            
 1000 CONTINUE                                                          
C
C 太陽中心地球位置ベクトルの規格化
C 太陽中心惑星位置ベクトルの規格化
C 地球中心惑星位置ベクトルの規格化
      CALL R_3210( DOPHEL, DNMEAR, DUMMY )
      CALL R_3210( DPLHCP, DNMPLT, DUMMY )
      CALL R_3210( DPLGEP, DPLDIS, DPLUNT )
C
C 光行差の補正
C 太陽の場合の補正量
      IF ( CESTNM .EQ. CSUN ) THEN
        DFN = 0D0
C 太陽以外の場合の補正量
      ELSE
        CALL DSUM( DPLUNT, DPLBVL, 3, 1, 1, DPV )
        DAG1 = DNMEAR + DNMPLT + DPLDIS
        DAG2 = DNMEAR + DNMPLT - DPLDIS
        DFN    =   2 * DGSHGS / DVLLGT**2 * DLOG( DAG1 / DAG2 )
      ENDIF
C
C 補正量の適応
      DLT =  ( DVLLGT * ( DTDBJD-DXEMTM ) * DAY - ( DPLDIS+DFN ) )
     +     / ( DVLLGT + DPV )
C 単位変換 [day] -> [sec]
C     write(*,*)'DXEMTM=',DXEMTM,' DLT=',DLT, ' DAY=',DAY
      DXEMTM = DXEMTM + DLT / DAY
C
C
      RETURN
C
      END
