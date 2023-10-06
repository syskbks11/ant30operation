      SUBROUTINE R_4300 ( DPORGN, DPOFFS, DPRSLT, JSCR, JERCOD )
C----------------------------------------------------------------------C
C     MODULE NAME : R_4300                                             C
C                                                                      C
C     FUNCTION    : DERIVATION OF COORDINATES OF A POINT IN QUESTION,  C
C                   BASED ON THE SPHERICAL TRIGONOMETRY.               C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1990.12.27                      C
C                                                                      C
C     -NOTE-                                                           C
C                                                                      C
C      NONE                                                            C
C                                                                      C
C----------------------------------------------------------------------C
C
C 宣言
#include      "__DFTF"
C
C コモン領域
#include      "XMATHC"
C
C 引数
      REAL*8     DPORGN(2)              ! オフセットを入れるベクトル
      REAL*8     DPOFFS(2)              ! オフセット
      REAL*8     DPRSLT(2)              ! オフセット挿入後
      INTEGER*2  JSCR                   ! 実虚角識別フラグ
      INTEGER*2  JERCOD                 ! リターンコード
C                                       * COMMON AREA OF INSTRUCTION   *
C                                       * WORK VARIABLE DEFINITION     *
      REAL*8     DWKV                                                   
C
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C                                      *-------------------------------*
C                                      * INITIALIZE                    *
C                                      *-------------------------------*
      JERCOD = 0                                                        
C                                                                       
C                                      *-------------------------------*
C                                      * LATITUDE                      *
C                                      *-------------------------------*
      DPRSLT(2) = DPORGN(2) + DPOFFS(2)                                 
C                                                                       
C                                      *-------------------------------*
C                                      * IF IRREGULAR POINT, THEN 9000 *
C                                      *-------------------------------*
      IF ( DABS( DPRSLT(2) ) .EQ. DHLFPI ) THEN
        IF ( DPOFFS(1) .NE. 0.0D0 ) THEN
          GOTO 9000
        ENDIF
      ENDIF
C                                                                       
C                                      *-------------------------------*
C                                      * LONGITUDE                     *
C                                      *-------------------------------*
      IF ( ( JSCR .GE. 4 ) .AND. ( JSCR .LE. 6 ) ) THEN                 
        IF ( DPOFFS(1) .EQ. 0.0D0 ) THEN
          DPRSLT(1) = DPORGN(1)
        ELSE
          DWKV = DSIN( 0.5D0 * DPOFFS(1) ) / DCOS( DPRSLT(2) )            
C                                                                       
C                                      * PARAMETER ERROR               *
          IF ( DWKV .GT. 1D0 ) THEN                                       
            JERCOD = 1                                                    
            DWKV = 1D0                                                    
          ELSE IF ( DWKV .LT. -1D0 ) THEN                                 
            JERCOD = 1                                                    
            DWKV = -1D0                                                   
          ENDIF                                                           
C                                      * CALCULATION                   *
          DPRSLT(1) = DPORGN(1) + 2D0 * DASIN( DWKV )                     
        ENDIF
      ELSE                                                              
        DPRSLT(1) = DPORGN(1) + DPOFFS(1)                               
      ENDIF                                                             
C                                                                       
      RETURN                                                            
C----------------------------------------------------------------------C
C ERROR ENTRY                                                          C
C----------------------------------------------------------------------C
C                                                                       
 9000 CONTINUE                                                          
      JERCOD = 4                                                        
      DPRSLT(1) = DPORGN(1)                                             
      DPRSLT(2) = DPORGN(2)                                             
C                                                                       
C----------------------------------------------------------------------C
C EXIT                                                                 C
C----------------------------------------------------------------------C
      RETURN                                                            
      END                                                               
