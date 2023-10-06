      SUBROUTINE R_4200 ( DGLCLB , DRADEC , CESCFE )
C----------------------------------------------------------------------C
C     MODULE NAME : R_4200                                             C
C                                                                      C
C     FUNCTION    : COORDINATES TRANSFORMATION ; L, B TO RA, DEC       C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1990.09.04                      C
C                                                                      C
C     -NOTE-                                                           C
C                                                                      C
C      NONE                                                            C
C                                                                      C
C----------------------------------------------------------------------C
C                                       *------------------------------*
C                                       * DECLARE DIVITION             *
C                                       *------------------------------*
#include     "__DFTF"
C                                       * ARGUMENT VARIABLE DEFINITION *
      REAL*8       DGLCLB(2)            ! 入力値（銀河座標）
      REAL*8       DRADEC(2)            ! 変換結果（赤道座標）
      CHARACTER*6  CESCFE               ! 分点名
C                                       * COMMON AREA OF INSTRUCTION   *
#include     "XGLCST"
#include     "XMATHC"
C
C                                       * WORK VARIABLE DEFINITION     *
C
      REAL*8     DWK1 , DWK2
      INTEGER*2  JSEPC
C                                                                       
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C
C 分点の識別
      IF ( CESCFE .EQ. 'J2000' ) THEN
        JSEPC = 1
      ELSEIF ( CESCFE .EQ. 'B1950' ) THEN
        JSEPC = 2
      ENDIF
C
C                                      *-------------------------------*
C                                      * DO TRANSFORMATION             *
C                                      *-------------------------------*
      DRADEC(2) = DASIN( DCOS( DGLCLB(2) ) * DCOS( DGNPDE( JSEPC ) ) *  
     +                   DSIN( DGLCLB(1) - DGANGL( JSEPC ) ) +          
     +                   DSIN( DGLCLB(2) ) * DSIN( DGNPDE( JSEPC ) )    
     +                 )                                                
C                                                                       
      DWK1      = DCOS( DGLCLB(2) ) * DCOS( DGLCLB(1) -                 
     +                                      DGANGL( JSEPC ) )           
      DWK2      = DSIN( DGLCLB(2) ) * DCOS( DGNPDE( JSEPC ) ) -         
     +            DCOS( DGLCLB(2) ) * DSIN( DGNPDE( JSEPC ) ) *         
     +                                DSIN( DGLCLB(1) -                 
     +                                      DGANGL( JSEPC ) )           
C                                                                       
      IF ( DWK2 .EQ. 0D0 ) GOTO 9000                                    
C                                                                       
      DRADEC(1) = DATAN( DWK1 / DWK2 )                                  
C                                                                       
C                                      * REMOVE AMBIGUITY IN ATAN      *
      IF ( ( DWK1 .LT. 0D0 ) .AND. ( DWK2 .GT. 0D0 ) ) THEN             
        DRADEC(1) = DRADEC(1) + DTWOPI                                  
      ELSEIF ( DWK2 .LT. 0D0 ) THEN
        DRADEC(1) = DRADEC(1) + DPIVAL                                  
      ENDIF                                                             
C                                                                       
C                                      * ADD RA OF GALACTIC N.P.       *
      DRADEC(1) = DRADEC(1) + DGNPRA( JSEPC )                           
      DRADEC(1) = DMOD( DRADEC(1) , DTWOPI )                            
C                                                                       
      RETURN                                                            
C                                                                       
C----------------------------------------------------------------------C
C ENTRY FOR ZERO DEVISION                                              C
C----------------------------------------------------------------------C
 9000 CONTINUE                                                          
      IF ( DWK1 .GE. 0D0 ) THEN                                         
        DRADEC(1) = DHLFPI + DGNPRA(JSEPC)                              
      ELSE                                                              
        DRADEC(1) = - DHLFPI + DGNPRA(JSEPC)                            
      ENDIF                                                             
C                                                                       
C----------------------------------------------------------------------C
C EXIT                                                                 C
C----------------------------------------------------------------------C
      RETURN                                                            
      END                                                               
