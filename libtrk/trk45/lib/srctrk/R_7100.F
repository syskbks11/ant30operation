      SUBROUTINE R_7100 ( DWTCD , JERCOD )
C----------------------------------------------------------------------C
C     MODULE NAME : R_7100                                             C
C                                                                      C
C     FUNCTION    : CORRECTION OF ATMOSPHERIC REFRACTION               C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1990.08.09                      C
C                                                                      C
C     -NOTE-                                                           C
C                                                                      C
C      NONE                                                            C
C                                                                      C
C----------------------------------------------------------------------C
C                                       *------------------------------*
C                                       * DECLARE DIVITION             *
C                                       *------------------------------*
C                                       * DEFAULT DEFINITION           *
#include     "__DFTF"
C                                       * COMMON AREA OF INSTRUCTION   *
#include     "XMATHC"
#include     "XVWORK"
C                                       * ARGUMENT VARIABLE DEFINITION *
      REAL*8        DWTCD(3)            ! 気象パラメータ
      INTEGER*2     JERCOD              ! リターンコード
C                                                                       
C                                       * WORK VARIABLE DEFINITION     *
C                                                                       
      REAL*8        DKELV, DRATE, D0DG, DZDS, DTZ, DLTZ
C                                                     
C                                       * CONATANT DATA                *
C                                                    
      PARAMETER ( D0DG = - 2.7315D2 )   ! 絶対温度０度のセ氏
C                                                   
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C
C リターンコードの初期化
      JERCOD = 0
C
C 引数チェック
      IF ( DAZELW(2) .LT. 1D-14 ) GO TO 9000                            
      IF( (DWTCD(1).LE.D0DG) .OR.                                       
     +    (DWTCD(2).LT.0D0 ) .OR. (DWTCD(3).LT.0D0) ) GO TO 9100        
C                                                                       
C                                      *-------------------------------*
C                                      * DERIVE REFRACTION RATE        *
C                                      *-------------------------------*
      DKELV = DWTCD(1) - D0DG                                           
      DRATE = ( 77.6D0 * DWTCD(2) + 3.73D5 * DWTCD(3) / DKELV )         
     +        / DKELV                                                   
C                                                                       
C                                      *-------------------------------*
C                                      * CORRECTION                    *
C                                      *-------------------------------*
      DZDS = DHLFPI - DAZELW(2)                                         
      DTZ  = DTAN(DZDS)                                                 
      DLTZ = 1.D-6 * DRATE * (1.D0 - 1.1D-3 * DTZ**2) * DTZ             
C                                                                       
      DARFWK(1) = DAZELW(1)                                             
      DARFWK(2) = DAZELW(2) + DLTZ                                      
C                                                                       
      RETURN                                                            
C                                                                       
C----------------------------------------------------------------------C
C ERROR ENTRY                                                          C
C----------------------------------------------------------------------C
C                                                                       
 9000 CONTINUE                                                          
      JERCOD = 1                                                        
      GOTO 1000                                                         
 9100 CONTINUE                                                          
      JERCOD = 2                                                        
 1000 CONTINUE                                                          
      DARFWK(1) = DAZELW(1)                                             
      DARFWK(2) = DAZELW(2)                                             
C                                                                       
C----------------------------------------------------------------------C
C EXIT                                                                 C
C----------------------------------------------------------------------C
      RETURN                                                            
C                                                                       
      END                                                               
