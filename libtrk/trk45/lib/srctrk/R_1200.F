      SUBROUTINE R_1200 ( DOEB19, JERCOD )                              
C----------------------------------------------------------------------C
C     MODULE NAME : R_1200                                             C
C                                                                      C
C     FUNCTION    : CONVERTING ORBITAL ELEMENTS BASED ON THE EPOCH OF  C
C                   B1950 TO THOSE OF J2000                            C
C                                                                      C
C     AUTHOR      : FUJITSU LIMITED    1991.06.18                      C
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
C                                       * COMMON VARIABLE DEFINITION   *
      INCLUDE 'XEPHEM'                                                  
      INCLUDE 'XMATHC'                                                  
C                                       * ARGUMENT VARIABLE DEFINITION *
      REAL*8  DOEB19(6)                                                 
C                                       * CONSTANT DATA DEFINITION     *
      REAL*8  DRCL0 / 9.0732024839857786D-2 /                           
      REAL*8  DRCL  / 7.8540110951322568D-2 /                           
      REAL*8  DRCJ  / 1.1378953311057351D-4 /                           
      PARAMETER( DEPS = 1D-38 )                                         
C                                                                       
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C                                      *-------------------------------*
C                                      * INITIALIZE                    *
C                                      *-------------------------------*
      JERCOD = 0                                                        
      DOMEGA = DOEB19(4)                                                
      DLOMEG = DOEB19(5)                                                
      DOBINC = DOEB19(6)                                                
C                                      *-------------------------------*
C                                      * BEGIN                         *
C                                      *-------------------------------*
C                                      * DERIVE ARGUMENT OF PERIHELION *
      DWK1 = DSIN( DRCJ ) * DSIN( DRCL + DLOMEG )                       
      DWK2 =   DSIN( DOBINC ) * DCOS( DRCJ )                            
     +       + DCOS( DOBINC ) * DSIN( DRCJ ) * DCOS( DRCL0 + DLOMEG )   
C                                                                       
C                                      * ERROR PROCEDURE               *
C                                                                       
      IF ( DABS(DWK1) * DEPS .GE. DABS(DWK2) ) GOTO 9000                
C                                                                       
C                                      * NORMAL CALCULATION            *
      DWK3 = DATAN( DWK1 / DWK2 )                                       
      IF ( ( DWK1 .LT. 0D0 ) .AND. ( DWK2 .GT. 0D0 ) ) THEN             
        DWK3 = DWK3 + DTWOPI                                            
      ELSE IF ( DWK2 .LT. 0D0 ) THEN                                    
        DWK3 = DWK3 + DPIVAL                                            
      ENDIF                                                             
C     WRITE(7,*) ' R_1200 ARGUMENT OF PERIHELION'                       
C     WRITE(7,*) '   DWK1, DWK2, DWK3 = ', DWK1, DWK2, DWK3             
C                                                                       
      DORBJ2(4) = DWK3 + DOMEGA                                         
C                                      * DERIVE INCLINATION            *
C                                                                       
      DWK3 =   DCOS( DOBINC ) * DCOS( DRCJ )                            
     +       - DSIN( DOBINC ) * DSIN( DRCJ ) * DCOS( DRCL0 + DLOMEG )   
      DWK4 = DSQRT( DWK1**2 + DWK2**2 )                                 
C                                      * ERROR PROCEDURE               *
C                                                                       
      IF ( DWK4 * DEPS .GE. DABS(DWK3) ) GOTO 9000                      
C                                                                       
C                                      * NORMAL CALCULATION            *
      DWK5 = DATAN( DWK4 / DWK3 )                                       
      IF ( DWK3 .LT. 0D0 ) THEN                                         
        DWK5 = DWK5 + DPIVAL                                            
      ENDIF                                                             
C     WRITE(7,*) ' R_1200 INCLINATION'                                  
C     WRITE(7,*) '   DWK3, DWK4, DWK5 = ', DWK3, DWK4, DWK5             
C                                                                       
      DORBJ2(6) = DWK5                                                  
C                                      * DERIVE LO. OF ASCENDING NODE  *
C                                                                       
      DWK1 = DSIN( DOBINC ) * DSIN( DRCL0 + DLOMEG )                    
      DWK2 =   DCOS( DOBINC ) * DSIN( DRCJ )                            
     +       + DSIN( DOBINC ) * DCOS( DRCJ ) * DCOS( DRCL0 + DLOMEG )   
C                                                                       
C                                      * ERROR PROCEDURE               *
C                                                                       
      IF ( DABS(DWK1) * DEPS .GE. DABS(DWK2) ) GOTO 9000                
C                                                                       
C                                      * NORMAL CACULATION             *
      DWK3 = DATAN( DWK1 / DWK2 )                                       
      IF ( ( DWK1 .LT. 0D0 ) .AND. ( DWK2 .GT. 0D0 ) ) THEN             
        DWK3 = DWK3 + DTWOPI                                            
      ELSE IF ( DWK2 .LT. 0D0 ) THEN                                    
        DWK3 = DWK3 + DPIVAL                                            
      ENDIF                                                             
C     WRITE(7,*) ' R_1200 THE LONGITUDE OF ASCENDING NODE'              
C     WRITE(7,*) '   DWK1, DWK2, DWK3 = ', DWK1, DWK2, DWK3             
C                                                                       
      DORBJ2(5) = DWK3 - DRCL                                           
C                                      * THE OTHER ELEMENTS            *
      DORBJ2(1) = DOEB19(1)                                             
      DORBJ2(2) = DOEB19(2)                                             
      DORBJ2(3) = DOEB19(3)                                             
C                                                                       
      RETURN                                                            
C                                                                       
C----------------------------------------------------------------------C
C ERROR ENTRY                                                          C
C----------------------------------------------------------------------C
 9000 CONTINUE                                                          
      JERCOD = 1000                                                     
      DORBJ2(1) = DOEB19(1)                                             
      DORBJ2(2) = DOEB19(2)                                             
      DORBJ2(3) = DOEB19(3)                                             
      DORBJ2(4) = DOEB19(4)                                             
      DORBJ2(5) = DOEB19(5)                                             
      DORBJ2(6) = DOEB19(6)                                             
C     WRITE(7,*) ' R_1200 PARAMETER ERROR ]]'                           
C     WRITE(7,*) '   DWK1, DWK2, DWK3, DWK4 = ',DWK1,DWK2,DWK3,DWK4     
C                                                                       
      RETURN                                                            
      END                                                               
