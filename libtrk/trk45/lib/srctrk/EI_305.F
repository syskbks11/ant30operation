C----------------------------------------------------------------------C
C     MODULE NAME : EI_305                                             C
C                                                                      C
C     FUNCTION    : INITIALIZATION OF PARAMETERS                       C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1990.08.08                      C
C                                                                      C
C     -NOTE-                                                           C
C                                                                      C
C      NONE                                                            C
C                                                                      C
C----------------------------------------------------------------------C
      SUBROUTINE EI_305 ( DELAST )                    
C                                       *------------------------------*
C                                       * DECLARE DIVITION             *
C                                       *------------------------------*
#include      "__DFTF"                                                
C                                       * COMMON AREA OF INSTRUCTION   *
#include      "XOBSEI"                                                
#include      "EI_ATCN"                                               
#include      "EI_TTME"                                               
C                                       * ARGUMENT VARIABLE DEFINITION *
      REAL*8      DELAST                ! ローカル視恒星時         

C                                       * WORK VARIABLE DEFINITION     *
C                                                                       
      INTEGER*2   JC                                                    
C                                                                       
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C
C  方位角、仰角の最大値、最小値
      DO 1000 JC = 1 , JANTNO                                           
        RTLMAZ(1 , JC ) = 450                                           
        RTLMAZ(2 , JC ) = -90                                           
        RTLMEL(1 , JC ) = 90                                            
        RTLMEL(2 , JC ) = 11                                            
 1000 CONTINUE                                                          
C
C  ローカル視恒星時
      DELAST = 0D0                                                      
C                                                                       
      RETURN                                                            
      END                                                               
