      SUBROUTINE R_3320 ( DCEPSL, DOBPCR, DOBPVL, JECODE )              
C----------------------------------------------------------------------C
C     MODULE NAME : R_3320                                             C
C                                                                      C
C     FUNCTION    : SOLVING KEPLER EQUATION : ECLIPTIC ORBIT WHEN      C
C                   IT IS CLOSE TO PARABOLIC ORBIT                     C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1996.04.30                      C
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
C     WRITE(7,*) ' R_3320 START ]]'                                     
      JECODE = 0                                                        
      IF ( DCEPSL .LE. 0 ) GOTO 9100                                    
      DTCR   =  DSQRT( DGS ) * ( DXEMTM - DORBJ2(1) )                   
C                                                                       
C                                      *-------------------------------*
C                                      * ITERATION UPTO 100 COUNTS     *
C                                      *-------------------------------*
C                                      * Step (i)                      *
      DB1 = 1D0                                                         
C                                      * Start iteration               *
      DO 1000 ITC = 1, 100                                              
C                                      * Step (ii)                     *
C                                                                       
        DC =  DTCR * DSQRT( 0.1D0 * ( 1 + 9 * DORBJ2(3) ) )             
     +      / DSQRT( 2 * DORBJ2(2)**3 ) / DB1                           
        DX =  DCBRT( 1.5D0 * DC + DSQRT( 1 + ( 1.5D0 * DC )**2 ) )      
     +      - DCBRT( -1.5D0 * DC + DSQRT( 1 + ( 1.5D0 * DC )**2 ) )     
C                                                                       
C                                      * Step (iii)                    *
C                                                                       
        DA =  DX**2 * 5 * DABS( 1 - DORBJ2(3) )                         
     +      / ( 1 + 9 * DORBJ2(3) )                                     
C                                                                       
C                                      * Step (iv)                     *
C                                                                       
        DB2 = 1 + 3 * DA**2 / 175 + 2 * DA**3 / 525                     
C                                                                       
C                                      * Step (v)                      *
C                                                                       
C     WRITE(7,*) ' R_3320 ITERATION NUMBER = ', ITC                     
C     WRITE(7,*) '   DB1, DC, DX, DA, DB2 = ', DB1,DC,DX,DA,DB2         
        IF ( DABS( DB2 - DB1 ) .LT. DCEPSL * DABS( DB1 ) ) THEN         
          DC =  DTCR * DSQRT( 0.1D0 * ( 1 + 9 * DORBJ2(3) ) )           
     +        / DSQRT( 2 * DORBJ2(2)**3 ) / DB2                         
          DXSL =  DCBRT( 1.5D0 * DC + DSQRT( 1 + ( 1.5D0 * DC )**2 ) )  
     +          - DCBRT( -1.5D0 * DC + DSQRT( 1 + ( 1.5D0 * DC )**2 ) ) 
          DASL =  DXSL**2 * 5 * DABS( 1 - DORBJ2(3) )                   
     +          / ( 1 + 9 * DORBJ2(3) )                                 
          GOTO 100                                                      
        ELSE                                                            
          DB1 = DB2                                                     
        ENDIF                                                           
 1000 CONTINUE                                                          
      GOTO 9000                                                         
C                                      *-------------------------------*
C                                      * SOLUTION                      *
C                                      *-------------------------------*
  100 CONTINUE                                                          
C     WRITE(7,*) ' R_3320 ITERATION GOOD DESU (^_^)'                    
C     WRITE(7,*) '   DC, DX, & A SOLVED = ', DC,DXSL,DASL               
      DS = 1 / DSQRT( 1 - 0.8D0 * DASL + 8 * DASL**2 / 175              
     +         + 8 * DASL**3 / 525 + 1896 * DASL**4 / 336875 )          
      DFSL = 2 * DATAN( DS * DXSL                                       
     +       * DSQRT( 5 * ( 1 + DORBJ2(3) ) / ( 1 + 9 * DORBJ2(3) ) ) ) 
C     WRITE(7,*) '   f SOLVED = ', DFSL                                 
C                                                                       
      DOBPCR(1) =  DCOS( DFSL ) / ( 1 + DORBJ2(3) * DCOS ( DFSL ) )     
     +           * DORBJ2(2) * ( 1 + DORBJ2(3) )                        
      DOBPCR(2) =  DSIN( DFSL ) / ( 1 + DORBJ2(3) * DCOS ( DFSL ) )     
     +           * DORBJ2(2) * ( 1 + DORBJ2(3) )                        
      DOBPCR(3) = 0D0                                                   
      DOBPVL(1) = - DSIN( DFSL )                                        
     +           * DSQRT( DGS / DORBJ2(2) / ( 1 + DORBJ2(3) ) )         
      DOBPVL(2) = ( DORBJ2(3) + DCOS( DFSL ) )                          
     +           * DSQRT( DGS / DORBJ2(2) / ( 1 + DORBJ2(3) ) )         
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
C     WRITE(7,*) ' R_3320 ERROR END ]]'                                 
      RETURN                                                            
C                                                                       
 9100 CONTINUE                                                          
      JECODE = 10000                                                    
C     WRITE(7,*) ' R_3320 DCEPSL MUST BE POSITIVE ]]'                   
      RETURN                                                            
      END                                                               
