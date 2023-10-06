      SUBROUTINE R_6200 ( JERCOD )
C----------------------------------------------------------------------C
C     MODULE NAME : R_6200                                             C
C                                                                      C
C     FUNCTION    : CORRECTION OF GEOCENTRIC PARALLAX                  C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1990.09.10                      C
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
C                                       * ARGUMENT VARIABLE DEFINITION *
      INTEGER*2     JERCOD              ! リターンコード
C                                       * COMMON AREA OF INSTRUCTION   *
#include     "XOBSIN"
#include     "XVWORK"
C                                                                       
C                                       * WORK VARIABLE DEFINITION     *
C                                                                       
      REAL*8        DVW(3), DNRM
      INTEGER*2     JC
C                                                                       
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C
C リターンコードを初期化する
      JERCOD = 0
C                                                                       
C                                      *-------------------------------*
C                                      * BEGIN                         *
C                                      *-------------------------------*
      DO 1000 JC = 1, 3, 1
        DVW(JC) = DPLDIS * DPRNUT(JC) - DORTRU(JC)
 1000 CONTINUE                                                          
C                                                                       
      IF ( (DVW(1).EQ.0D0) .AND. (DVW(2).EQ.0D0)                        
     +                     .AND. (DVW(3).EQ.0D0) ) THEN                 
        GOTO 9000                                                       
      ENDIF                                                             
C                                                                       
      CALL R_3210( DVW, DNRM, DGCPRL )                                  
C                                                                       
      RETURN                                                            
C                                                                       
C----------------------------------------------------------------------C
C ERROR ENTRY                                                          C
C----------------------------------------------------------------------C
C                                                                       
 9000 CONTINUE                                                          
      JERCOD = 1                                                        
      DGCPRL(1) = DPRNUT(1)                                             
      DGCPRL(2) = DPRNUT(2)                                             
      DGCPRL(3) = DPRNUT(3)                                             
C                                                                       
C----------------------------------------------------------------------C
C EXIT                                                                 C
C----------------------------------------------------------------------C
      RETURN                                                            
      END                                                               
