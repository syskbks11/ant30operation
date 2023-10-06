      SUBROUTINE R_5200 ( JSTCOD )                                      
C----------------------------------------------------------------------C
C     MODULE NAME : R_5200                                             C
C                                                                      C
C     FUNCTION    : CORRECTION FOR LIGHT DEFLECTION                    C
C                   BY THE SUN'S GRAVITATION                           C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1990.07.31                      C
C                                                                      C
C     -NOTE-                                                           C
C                                                                      C
C      NONE                                                            C
C                                                                      C
C----------------------------------------------------------------------C
C                                       *------------------------------*
C                                       * DECLARE DIVITION             *
C                                       *------------------------------*
      INCLUDE '__DFTF'                                                  
C                                       * ARGUMENT VARIABLE DEFINITION *
      INTEGER*2   JSTCOD                                                
C                                       * COMMON AREA OF INSTRUCTION   *
      INCLUDE 'XIERST'                                                  
      INCLUDE 'XEPHEM'                                                  
      INCLUDE 'XVWORK'                                                  
C                                                                       
C                                       * WORK VARIABLE DEFINITION     *
      REAL*8     DNMEAR , DINPR1 , DINPR2 , DFCT ,                      
     +           DOPNMV(3) , DPLSNV(3) , DUMMY(3)                       
      INTEGER*2  JC                                                     
C                                                                       
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C                                      *-------------------------------*
C                                      * NORMALIZATION                 *
C                                      *-------------------------------*
      CALL R_3210( DOPHEL , DNMEAR , DOPNMV )                           
C                                                                       
C                                      *-------------------------------*
C                                      * IF ( RA , DEC ) IS GIVEN      *
C                                      *-------------------------------*
      IF ( JSTCOD .EQ. 1 ) THEN                                         
C                                                                       
C                                      * SCALAR PRODUCT                *
        CALL DSUM( DOPNMV , DPRPMT , 3 , 1 , 1 , DINPR1 )               
C                                                                       
C                                      * CORRECTION                    *
        DFCT  = 2 * DGSHGS / DVLLGT**2 / DNMEAR / ( 1D0 + DINPR1 )      
        DO 1000 JC = 1 , 3 , 1                                          
          DLTDFL(JC) =  DPRPMT(JC)                                      
     +                + DFCT * ( DOPNMV(JC) - DINPR1 * DPRPMT(JC) )     
 1000   CONTINUE                                                        
        GOTO 100                                                        
C                                                                       
      ENDIF                                                             
C                                                                       
C                                      *-------------------------------*
C                                      * IF THE OBJECT IS A PLANET     *
C                                      *-------------------------------*
      IF ( JSTCOD .EQ. 3 ) THEN                                         
C                                                                       
C                                      * NORMALIZATION                 *
        CALL R_3210( DPLHCP , DUMMY(1) , DPLSNV )                       
C                                                                       
C                                      * SCALAR PRODUCTS               *
        CALL DSUM( DOPNMV , DPLSNV , 3 , 1 , 1 , DINPR1 )               
        CALL DSUM( DOPNMV , DPLUNT , 3 , 1 , 1 , DINPR2 )               
C                                                                       
C                                      * CORRECTION                    *
        DFCT  = 2 * DGSHGS / DVLLGT**2 / DNMEAR / ( 1D0 + DINPR1 )      
        DO 1100 JC = 1 , 3 , 1                                          
          DLTDFL(JC) =  DPLUNT(JC)                                      
     +                + DFCT * ( DOPNMV(JC) - DINPR2 * DPLUNT(JC) )     
 1100   CONTINUE                                                        
C                                                                       
      ENDIF                                                             
C                                                                       
  100 CONTINUE                                                          
C                                                                       
      RETURN                                                            
C                                                                       
      END                                                               
