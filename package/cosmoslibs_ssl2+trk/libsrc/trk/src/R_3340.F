      SUBROUTINE R_3340 ( DCEPSL, DOBPCR, DOBPVL, JECODE )              
C----------------------------------------------------------------------C
C     MODULE NAME : R_3340                                             C
C                                                                      C
C     FUNCTION    : SOLVING KEPLER EQUATION : HYPERBOLIC ORBIT         C
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
C                                      * INITIALIZATION                *
C                                      *-------------------------------*
      JECODE = 0                                                        
      IF ( DCEPSL .LE. 0 ) GOTO 9100                                    
      DRMU   = DSQRT( DGS * ( DORBJ2(3) - 1 )**3 / DORBJ2(2)**3 )       
      DLEX   = DRMU * ( DXEMTM - DORBJ2(1) )                            
C     WRITE(7,*) ' R_3340 START. DRMU, L = ', DRMU,DLEX                 
      IF ( DLEX + DSQRT( DLEX**2 + DORBJ2(3)**2 ) .GE. 2 ) JECODE = 10  
C                                                                       
C                                      *-------------------------------*
C                                      * ITERATION UPTO 100 COUNTS     *
C                                      *-------------------------------*
      DU0 = DLOG(  DLEX / DORBJ2(3)                                     
     +           + DSQRT( ( DLEX / DORBJ2(3) )**2 + 1 ) )               
C     WRITE(7,*) ' R_3340 U INITIAL VALUE = ', DU0                      
      DU1 = DU0                                                         
      DO 1000 ITC = 1, 100                                              
C     WRITE(7,*) ' R_3340 ITERATION NUMBER = ', ITC                     
C     WRITE(7,*) '   U ASSUMED VALUE   = ', DU1                         
        DU2 = DLOG(  ( DLEX + DU1 ) / DORBJ2(3)                         
     +             + DSQRT( ( ( DLEX + DU1 ) / DORBJ2(3) )**2 + 1 ) )   
C     WRITE(7,*) '   U CORRECTED VALUE = ', DU2                         
        IF ( DABS( DU2 - DU1 ) .LT. DCEPSL * DABS( DU1 ) ) THEN         
          DUSL = DU2                                                    
          GOTO 100                                                      
        ELSE                                                            
          DU1 = DU2                                                     
        ENDIF                                                           
 1000 CONTINUE                                                          
      GOTO 9000                                                         
C                                      *-------------------------------*
C                                      * SOLUTION                      *
C                                      *-------------------------------*
  100 CONTINUE                                                          
C     WRITE(7,*) ' R_3340 ITERATION GOOD DESU (^_^)'                    
C     WRITE(7,*) '   U SOLVED = ', DUSL                                 
      DOBPCR(1) = DORBJ2(2) * ( DORBJ2(3) - DCOSH( DUSL ) )             
     +           / ( DORBJ2(3) - 1 )                                    
      DOBPCR(2) = DORBJ2(2) * DSINH( DUSL )                             
     +           * DSQRT( ( DORBJ2(3) + 1 ) / ( DORBJ2(3) - 1 ) )       
      DOBPCR(3) = 0D0                                                   
      DOBPVL(1) = - DORBJ2(2) * DSINH( DUSL ) * DRMU                    
     +           / ( DORBJ2(3) - 1 ) / ( DORBJ2(3) * DCOSH(DUSL) - 1 )  
      DOBPVL(2) = DORBJ2(2) * DCOSH( DUSL )                             
     +           * DRMU / ( DORBJ2(3) * DCOSH( DUSL ) - 1 )             
     +           * DSQRT( ( DORBJ2(3) + 1 ) / ( DORBJ2(3) - 1 ) )       
      DOBPVL(3) = 0D0                                                   
C                                                                       
      RETURN                                                            
C----------------------------------------------------------------------C
C ERROR ENTRY                                                          C
C----------------------------------------------------------------------C
 9000 CONTINUE                                                          
      JECODE = 1000                                                     
      DOBPCR(1) = 0D0                                                   
      DOBPCR(2) = 0D0                                                   
      DOBPCR(3) = 0D0                                                   
      DOBPVL(1) = 0D0                                                   
      DOBPVL(2) = 0D0                                                   
      DOBPVL(3) = 0D0                                                   
C     WRITE(7,*) ' R_3340 ERROR END ]]'                                 
      RETURN                                                            
C                                                                       
 9100 CONTINUE                                                          
      JECODE = 10000                                                    
C     WRITE(7,*) ' R_3340 DCEPSL MUST BE POSITIVE ]]'                   
      RETURN                                                            
      END                                                               
