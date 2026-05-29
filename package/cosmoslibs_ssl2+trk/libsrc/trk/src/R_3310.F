      SUBROUTINE R_3310 ( DCEPSL, DOBPCR, DOBPVL, JECODE )              
C----------------------------------------------------------------------C
C     MODULE NAME : R_3310                                             C
C                                                                      C
C     FUNCTION    : SOLVING KEPLER EQUATION : ECLIPTIC ORBIT           C
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
      DRMU   = DSQRT( DGS * ( 1 - DORBJ2(3) )**3 / DORBJ2(2)**3 )       
      DLEX   = DRMU * ( DXEMTM - DORBJ2(1) )                            
C                                      *-------------------------------*
C                                      * INITIAL VALUE : FOURIER SERIES*
C                                      *-------------------------------*
      DU0 = DLEX + ( DORBJ2(3) - DORBJ2(3)**3 / 8                       
     +    + DORBJ2(3)**5 / 192 - DORBJ2(3)**7 / 9216 ) * DSIN( DLEX )   
     +    + ( 0.5D0 * DORBJ2(3)**2 - DORBJ2(3)**4 / 6                   
     +    +   DORBJ2(3)**6 / 48 ) * DSIN( 2 * DLEX )                    
     +    + ( 3 * DORBJ2(3)**3 / 8 - 27 * DORBJ2(3)**5 / 128            
     +    +   243 * DORBJ2(3)**7 / 5120 ) * DSIN( 3 * DLEX )            
     +    + ( DORBJ2(3)**4 / 3 - 4 * DORBJ2(3)**6 / 15 ) * DSIN(4*DLEX) 
     +    + ( 125 * DORBJ2(3)**5 / 384                                  
     +    -   3125 * DORBJ2(3)**7 / 9216 ) * DSIN( 5 * DLEX )           
     +    + 27 * DORBJ2(3)**6 / 80 * DSIN( 6 * DLEX )                   
     +    + 16807 * DORBJ2(3)**7 / 46080 * DSIN( 7 * DLEX )             
C     WRITE(7,*) ' R_3310 INITIAL VALUE = ', DU0                        
C                                                                       
C                                      *-------------------------------*
C                                      * ITERATION UPTO 100 COUNTS     *
C                                      *-------------------------------*
      DU1 = DU0                                                         
      DL1 = DU1 - DORBJ2(3) * DSIN( DU1 )                               
      DO 1000 ITC = 1, 100                                              
C     WRITE(7,*) ' R_3310 ITERATION NUMBER = ',ITC                      
C     WRITE(7,*) '   ASSUMED VALUE U & L   = ',DU1,DL1                  
        DELTU = ( DLEX - DL1 ) / ( 1 - DORBJ2(3) * DCOS( DU1 ) )        
        DU2   = DU1 + DELTU                                             
        DL2   = DU2 - DORBJ2(3) * DSIN( DU2 )                           
C     WRITE(7,*) '   CORRECTED VALUE U & L = ',DU2,DL2                  
        IF ( DABS( DLEX - DL2 ) .LT. DCEPSL ) THEN                      
          DUSL = DU2                                                    
          GOTO 100                                                      
        ELSE                                                            
          DU1 = DU2                                                     
          DL1 = DL2                                                     
        ENDIF                                                           
 1000 CONTINUE                                                          
      GOTO 9000                                                         
C                                      *-------------------------------*
C                                      * SOLUTION                      *
C                                      *-------------------------------*
  100 CONTINUE                                                          
C     WRITE(7,*) ' R_3310 ITERATION GOOD DESU (^_^)'                    
C     WRITE(7,*) '   SOLUTION U & L = ', DUSL,DLEX                      
      DOBPCR(1) = DORBJ2(2) * ( DCOS( DUSL ) - DORBJ2(3) )              
     +           / ( 1 - DORBJ2(3) )                                    
      DOBPCR(2) =  DORBJ2(2) * DSIN( DUSL )                             
     +           * DSQRT( ( 1 + DORBJ2(3) ) / ( 1 - DORBJ2(3) ) )       
      DOBPCR(3) = 0D0                                                   
      DOBPVL(1) = - DSIN( DUSL ) / ( 1 - DORBJ2(3) * DCOS( DUSL ) )     
     +           * DSQRT( DGS * ( 1 - DORBJ2(3) ) / DORBJ2(2) )         
      DOBPVL(2) =   DCOS( DUSL ) / ( 1 - DORBJ2(3) * DCOS( DUSL ) )     
     +           * DSQRT( DGS * ( 1 - DORBJ2(3) )**2 * ( 1 + DORBJ2(3) )
CHANGE 1997.01.13                                                       
CCCC +           * DSQRT( DGS * ( 1 - DORBJ2(3)**2 )                    
     +                  / DORBJ2(2) )                                   
      DOBPVL(3) = 0D0                                                   
C                                                                       
      RETURN                                                            
C----------------------------------------------------------------------C
C ERROR ENTRY                                                          C
C----------------------------------------------------------------------C
 9000 CONTINUE                                                          
      JECODE = 1000                                                     
      DOBPCR(1) = DORBJ2(2) * ( DCOS( DU0 ) - DORBJ2(3) )               
     +           / ( 1 - DORBJ2(3) )                                    
      DOBPCR(2) =  DORBJ2(2) * DSIN( DU0 )                              
     +           * DSQRT( ( 1 + DORBJ2(3) ) / ( 1 - DORBJ2(3) ) )       
      DOBPVL(1) = - DSIN( DU0 ) / ( 1 - DORBJ2(3) * DCOS( DU0 ) )       
     +           * DSQRT( DGS * ( 1 - DORBJ2(3) ) / DORBJ2(2) )         
      DOBPVL(2) =   DCOS( DU0 ) / ( 1 - DORBJ2(3) * DCOS( DU0 ) )       
     +           * DSQRT( DGS * ( 1 - DORBJ2(3) )**2 * ( 1 + DORBJ2(3) )
CHANGE 1997.11.13                                                       
CCCC +           * DSQRT( DGS * ( 1 - DORBJ2(3)**2 )                    
     +                  / DORBJ2(2) )                                   
      DOBPCR(3) = 0D0                                                   
      DOBPVL(3) = 0D0                                                   
C     WRITE(7,*) ' R_3310 ERROR END ]]'                                 
      RETURN                                                            
C                                                                       
 9100 CONTINUE                                                          
      JECODE = 10000                                                    
C     WRITE(7,*) ' R_3310 DCEPSL MUST BE POSITIVE ]]'                   
      RETURN                                                            
      END                                                               
