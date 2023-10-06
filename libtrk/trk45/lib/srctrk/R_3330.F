      SUBROUTINE R_3330 ( DOBPCR, DOBPVL )                              
C----------------------------------------------------------------------C
C     MODULE NAME : R_3330                                             C
C                                                                      C
C     FUNCTION    : SOLVING KEPLER EQUATION : PARABOLIC ORBIT          C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1991.06.18                      C
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
C                                       * CONSTANT DATA DEFINITION     *
      PARAMETER( DGS = 2.95912208D-4 )                                  
C                                                                       
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C                                      *-------------------------------*
C                                      * 3RD DEGREE ALGEBRAIC EQUATION *
C                                      *-------------------------------*
C                                                                       
      DRMU   = DSQRT( 0.5D0 * DGS / DORBJ2(2)**3 )                      
     +        * ( DXEMTM - DORBJ2(1) )                                  
      DF =  DCBRT( 1.5D0 * DRMU + DSQRT( 1 + ( 1.5D0 * DRMU )**2 ) )    
     +    - DCBRT( -1.5D0 * DRMU + DSQRT( 1 + ( 1.5D0 * DRMU )**2 ) )   
      DF = 2 * DATAN( DF )                                              
C     WRITE(7,*) ' R_3330 START DRMU = ', DRMU                          
C     WRITE(7,*) ' R_3330 START DF   = ', DF                            
C                                      *-------------------------------*
C                                      * SOLUTION                      *
C                                      *-------------------------------*
      DOBPCR(1) = 2 * DORBJ2(2) * DCOS( DF ) / ( 1 + DCOS( DF ) )       
      DOBPCR(2) = 2 * DORBJ2(2) * DSIN( DF ) / ( 1 + DCOS( DF ) )       
      DOBPCR(3) = 0                                                     
CCCC  DOBPVL(1) = -4 * DORBJ2(2) * DSIN( DF ) * DCOS( 0.5D0 * DF )**4   
CCCC +           * DRMU / ( 1 + DCOS( DF ) )**2                         
CCCC  DOBPVL(2) =  4 * DORBJ2(2) * DCOS( 0.5D0 * DF )**4 * DRMU         
CCCC +           / ( 1 + DCOS( DF ) )                                   
C CHANGE 1996.12.10                                                     
      DOBPVL(1) = -DSIN( DF ) * DSQRT( DGS / (2.0D0 * DORBJ2(2)) )      
      DOBPVL(2) = (1.0D0 + DCOS(DF)) * DSQRT( DGS / (2.0D0 * DORBJ2(2)))
      DOBPVL(3) = 0                                                     
C                                                                       
      RETURN                                                            
      END                                                               
