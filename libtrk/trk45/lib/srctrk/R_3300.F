      SUBROUTINE R_3300 ( DOBPCR, DOBPVL, JECODE )                      
C----------------------------------------------------------------------C
C     MODULE NAME : R_3300                                             C
C                                                                      C
C     FUNCTION    : DERIVE CARTESIAN COORDINATE FROM ORBITAL ELEMENTS  C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1991.06.19                      C
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
      REAL*8       DOBPCR(3), DOBPVL(3)                                 
C                                       * COMMON AREA OF INSTRUCTION   *
      INCLUDE 'XEPHEM'                                                  
      INCLUDE 'XIERST'                                                  
C                                                                       
C                                       * WORK VARIABLE DEFINITION     *
C                                                                       
      REAL*8       DCXY(3), DCVL(3)                                     
C                                       * CONSTANT DATA DEFINITION     *
C     DATA   DCSC1, DCSC2, DEPSL / 0.9999D0, 1.01D0, 1D-14/             
      DATA   DCSC1, DCSC2, DEPSL / 0.9999D0, 1.01D0, 1D-8 /             
C**** HB COMMET TEST PARM                                               
C     DATA   DCSC1, DCSC2, DEPSL / 0.9000D0, 1.01D0, 1D-8 /             
C**** HB COMMET TEST PARM                                               
      PARAMETER( DAY = 86400D0 )                                        
C                                                                       
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C                                      *-------------------------------*
C                                      * INITIALIZE                    *
C                                      *-------------------------------*
C     WRITE(7,*) ' R_3300 START DCSC1, DCSC2, DEPSL, E :'               
C     WRITE(7,*) '   ', DCSC1, DCSC2, DEPSL, DORBJ2(3)                  
      JECODE = 0                                                        
      DECS   = DORBJ2(3)                                                
C                                      *-------------------------------*
C                                      * ECLIPTIC ORBIT                *
C                                      *-------------------------------*
      IF ( DECS .LT. 1 ) THEN                                           
C     WRITE(7,*) ' R_3300 THE ORBIT IS ECLIPTIC'                        
C                                      * NEWTON-RAPHSON METHOD         *
        IF ( DECS .LT. DCSC1 ) THEN                                     
          CALL R_3310( DEPSL, DCXY, DCVL, JRC )                         
C     WRITE(7,*) '   USING NEWTON-RAPHSON METHOD. RETURN CODE = ',JRC   
          IF ( JRC .GT. 0 ) GOTO 9000                                   
C                                      * GAUSSIAN METHOD               *
        ELSE                                                            
          CALL R_3320( DEPSL, DCXY, DCVL, JRC )                         
C     WRITE(7,*) '   USING GAUSSIAN METHOD. RETURN CODE = ',JRC         
          IF ( JRC .GT. 0 ) GOTO 9000                                   
        ENDIF                                                           
C                                      *-------------------------------*
C                                      * PARABOLIC ORBIT               *
C                                      *-------------------------------*
      ELSE IF ( DECS .EQ. 1 ) THEN                                      
C     WRITE(7,*) ' R_3300 THE ORBIT IS PARABOLIC'                       
        CALL R_3330( DCXY, DCVL )                                       
C                                      *-------------------------------*
C                                      * HYPERBOLIC ORBIT              *
C                                      *-------------------------------*
      ELSE                                                              
C     WRITE(7,*) ' R_3300 THE ORBIT IS HYPERBOLIC'                      
C                                      * GAUSSIAN METHOD               *
        IF ( DECS .LT. DCSC2 ) THEN                                     
          CALL R_3320( DEPSL, DCXY, DCVL, JRC )                         
C     WRITE(7,*) '   USING GAUSSIAN METHOD. RETURN CODE = ',JRC         
          IF ( JRC .GT. 0 ) GOTO 9000                                   
C                                      * USING THE EQUATION OF ORBIT   *
        ELSE                                                            
          CALL R_3340( DEPSL, DCXY, DCVL, JRC )                         
C     WRITE(7,*) '   USING THE EQUATION OF ORBIT. RETURN CODE = ',JRC   
          IF ( JRC .GT. 100 ) GOTO 9000                                 
        ENDIF                                                           
C                                                                       
      ENDIF                                                             
C                                      *-------------------------------*
C                                      * OUTPUT RESULTS                *
C                                      *-------------------------------*
C     WRITE(7,*) ' R_3300 DCXY = ', DCXY                                
C     WRITE(7,*) ' R_3300 DCVL = ', DCVL                                
      DOBPCR(1) = DCXY(1) * DASUNT                                      
      DOBPCR(2) = DCXY(2) * DASUNT                                      
      DOBPCR(3) = DCXY(3) * DASUNT                                      
      DOBPVL(1) = DCVL(1) * DASUNT / DAY                                
      DOBPVL(2) = DCVL(2) * DASUNT / DAY                                
      DOBPVL(3) = DCVL(3) * DASUNT / DAY                                
C                                                                       
      RETURN                                                            
C----------------------------------------------------------------------C
C ERROR ENTRY                                                          C
C----------------------------------------------------------------------C
 9000 CONTINUE                                                          
      JECODE = 1000                                                     
      DOBPCR(1) = DCXY(1) * DASUNT                                      
      DOBPCR(2) = DCXY(2) * DASUNT                                      
      DOBPCR(3) = DCXY(3) * DASUNT                                      
      DOBPVL(1) = DCVL(1) * DASUNT / DAY                                
      DOBPVL(2) = DCVL(2) * DASUNT / DAY                                
      DOBPVL(3) = DCVL(3) * DASUNT / DAY                                
C     WRITE(7,*) ' R_3300 ERROR OCCURS IN A CALLEE ]]'                  
      RETURN                                                            
      END                                                               
