C----------------------------------------------------------------------C
C     MODULE NAME : R_3210                                             C
C                                                                      C
C     FUNCTION    : NORMALIZATION OF A VECTOR. ITS NORM IS INCLUDED IN C
C                   OUTPUT LIST.                                       C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1990.07.31                      C
C                                                                      C
C     -NOTE-                                                           C
C                                                                      C
C      NONE                                                            C
C                                                                      C
C----------------------------------------------------------------------C
      SUBROUTINE R_3210 ( DVECCM, DVNORM, DVECNL )
C                                       *------------------------------*
C                                       * DECLARE DIVITION             *
C                                       *------------------------------*
#include      "__DFTF"
C                                       * ARGUMENT VARIABLE DEFINITION *
      REAL*8        DVECCM(3) , DVNORM , DVECNL(3)                      
C                                                                       
C                                       * WORK VARIABLE DEFINITION     *
C                                                                       
      INTEGER*2     JC                                                  
C                                                                       
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C                                      *-------------------------------*
C                                      * INITIALIZE                    *
C                                      *-------------------------------*
      DVNORM    = 0D0                                                   
      DVECNL(1) = 0D0                                                   
      DVECNL(2) = 0D0                                                   
      DVECNL(3) = 0D0                                                   
C                                                                       
C                                      *-------------------------------*
C                                      * BEGIN                         *
C                                      *-------------------------------*
C                                      * IF INPUT IS ZERO VECTOR       *
C ０ベクトルの場合処理は行わない
      IF ( ( DVECCM(1) .EQ. 0D0 ) .AND. ( DVECCM(2) .EQ. 0D0 ) .AND.    
     +     ( DVECCM(3) .EQ. 0D0 ) ) THEN                                
        GOTO 100                                                        
      ENDIF                                                             
C                                                                       
C ベクトルの絶対値を求める
      CALL DSUM( DVECCM , DVECCM , 3 , 1 , 1 , DVNORM )                 
      DVNORM = DSQRT( DVNORM )                                          
C
C ベクトルの絶対値で割る
      DO 1000 JC = 1 , 3 , 1                                            
        DVECNL( JC ) = DVECCM( JC ) / DVNORM                            
 1000 CONTINUE                                                          
C                                                                       
C----------------------------------------------------------------------C
C EXIT                                                                 C
C----------------------------------------------------------------------C
  100 CONTINUE                                                          
C                                                                       
      RETURN                                                            
      END                                                               
