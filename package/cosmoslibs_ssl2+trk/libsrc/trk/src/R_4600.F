      SUBROUTINE R_4600 ( DPB19 , DPJ20 )
C----------------------------------------------------------------------C
C     MODULE NAME : R_4600                                             C
C                                                                      C
C     FUNCTION    : CONVERSION BETWEEN THE MEAN POSITIONS AT B1950     C
C                   BASED ON FK4 SYSTEM TO THOSE AT J2000 BASED ON     C
C                   FK5 SYSTEM.                                        C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1990.09.06                      C
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
#include     "XUNTCV"
#include     "XDUMMY"
C                                       * ARGUMENT VARIABLE DEFINITION *
      REAL*8      DPB19(2), DPJ20(2)                                    
C                                       * WORK VARIABLE DEFINITION     *
      REAL*8      DCEF, DAW(3), DADOT(3), DAWS, DADR,
     +            DTRMP1(3,3), DTRMP2(3,3), DTRMV1(3,3), DTRMV2(3,3),   
     +            DWK1(3), DWK2(3), DWK3(3), DWK4(3), DWK5(3), DWK6(3), 
     +            DWK7(3), DWK8(3), DVW(3)                         
C     REAL*8       DCVE, DPVS, DNRM, DWK
      INTEGER*4   ICON                                                  
      INTEGER*2   JRSC                                                  
C                                       * CONSTANT DATA DEFINITION     *
C      DATA DCEF, DCVE  / 21.094502D0 , 0.047404704D0 /
      DATA DCEF  / 21.094502D0 /                  
      DATA ( ( DTRMP1( I1, I2 ), I2 = 1, 3 ), I1 = 1, 3 )               
     +          / 0.9999256782D0, -0.0111820610D0, -0.0048579477D0,     
     +            0.0111820609D0,  0.9999374784D0, -0.0000271765D0,     
     +            0.0048579479D0, -0.0000271474D0,  0.9999881997D0 /    
      DATA ( ( DTRMP2( I1, I2 ), I2 = 1, 3 ), I1 = 1, 3 )               
     +          / 2.4239502D-6,   -2.71066D-8,     -1.17766D-8,         
     +            2.71066D-8,      2.4239788D-6,   -6.59D-11,           
     +            1.17766D-8,     -6.58D-11,        2.4241017D-6 /      
      DATA ( ( DTRMV1( I1, I2 ), I2 = 1, 3 ), I1 = 1, 3 )               
     +          /-0.00055D0,      -0.23854D0,       0.43574D0,          
     +            0.23849D0,      -0.00267D0,      -0.00854D0,          
     +           -0.43562D0,       0.01225D0,       0.00212D0 /         
      DATA ( ( DTRMV2( I1, I2 ), I2 = 1, 3 ), I1 = 1, 3 )               
     +          / 0.99994704D0,   -0.01118251D0,   -0.00485767D0,       
     +            0.01118251D0,    0.99995883D0,   -0.00002718D0,       
     +            0.00485767D0,   -0.00002714D0,    1.00000956D0 /      
C                                                                       
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C                                      *-------------------------------*
C                                      * BEGIN                         *
C                                      *-------------------------------*
C                                      * SPHERICAL TO CARTESIAN        *
C     WRITE(7,*) ' R_4600 DPB19    = ', DPB19                           
      CALL R_4500( DPB19, DWK1)                                         
C                                                                       
C                                      *-------------------------------*
C                                      * DERIVE VELOCITY VECTOR        *
C                                      *-------------------------------*
      DWK2(1) = - DPROPM(2) * DSIN( DPB19(2) ) * DCOS( DPB19(1) )       
     +          - 15D0 * DPROPM(1) * DWK1(2)                            
     +          + DCEF * DANPRL * DRVELC * DWK1(1)                      
      DWK2(2) = - DPROPM(2) * DSIN( DPB19(2) ) * DSIN( DPB19(1) )       
     +          + 15D0 * DPROPM(1) * DWK1(1)                            
     +          + DCEF * DANPRL * DRVELC * DWK1(2)                      
      DWK2(3) =   DPROPM(2) * DCOS( DPB19(2) )                          
     +          + DCEF * DANPRL * DRVELC * DWK1(3)                      
C     WRITE(7,*) ' R_4600 POSITION = ', DWK1                            
C     WRITE(7,*) ' R_4600 VELOCITY = ', DWK2                            
C                                                                       
C                                      *-------------------------------*
C                                      * REMOVE THE EFFECT OF THE      *
C                                      * E-TERM OF ABERRATION.         *
C                                      *-------------------------------*
      DAW(1) = 1D-6 * ( -1.62557D0 )                                    
      DAW(2) = 1D-6 * ( -0.31919D0 )                                    
      DAW(3) = 1D-6 * ( -0.13843D0 )                                    
C                                                                       
      DADOT(1) = 1D-3 * (  1.245D0 )                                    
      DADOT(2) = 1D-3 * ( -1.580D0 )                                    
      DADOT(3) = 1D-3 * ( -0.659D0 )                                    
C                                                                       
      CALL DSUM( DWK1, DAW, 3, 1, 1, DAWS)                              
      CALL DSUM( DWK1, DADOT, 3, 1, 1, DADR)                            
C                                                                       
      DWK3(1) = DWK1(1) - DAW(1)   + DAWS * DWK1(1)                     
      DWK3(2) = DWK1(2) - DAW(2)   + DAWS * DWK1(2)                     
      DWK3(3) = DWK1(3) - DAW(3)   + DAWS * DWK1(3)                     
C                                                                       
      DWK4(1) = DWK2(1) - DADOT(1) + DADR * DWK1(1)                     
      DWK4(2) = DWK2(2) - DADOT(2) + DADR * DWK1(2)                     
      DWK4(3) = DWK2(3) - DADOT(3) + DADR * DWK1(3)                     
C     WRITE(7,*) ' E-TERM POSITION = ', DWK3                            
C     WRITE(7,*) ' E-TERM VELOCITY = ', DWK4                            
C                                                                       
C                                      *-------------------------------*
C                                      * GET J2000 BASED POS. AND VEL. *
C                                      *-------------------------------*
C                                      * >>> POSITION <<<              *
      CALL DMGGM( DTRMP1, 3, DWK3, 3, DWK5, 3, 3, 3, 1, DVW, ICON)      
      IF ( ICON .NE. 0 ) WRITE(7,*) ' R_4600 ICON = ', ICON             
C                                                                       
      CALL DMGGM( DTRMP2, 3, DWK4, 3, DWK6, 3, 3, 3, 1, DVW, ICON)      
      IF ( ICON .NE. 0 ) WRITE(7,*) ' R_4600 ICON = ', ICON             
C                                                                       
      DWK7(1) = DWK5(1) + DWK6(1)                                       
      DWK7(2) = DWK5(2) + DWK6(2)                                       
      DWK7(3) = DWK5(3) + DWK6(3)                                       
C                                                                       
C                                      * >>> VELOCITY <<<              *
      CALL DMGGM( DTRMV1, 3, DWK3, 3, DWK5, 3, 3, 3, 1, DVW, ICON)      
      IF ( ICON .NE. 0 ) WRITE(7,*) ' R_4600 ICON = ', ICON             
C                                                                       
      CALL DMGGM( DTRMV2, 3, DWK4, 3, DWK6, 3, 3, 3, 1, DVW, ICON)      
      IF ( ICON .NE. 0 ) WRITE(7,*) ' R_4600 ICON = ', ICON             
C                                                                       
      DWK8(1) = DWK5(1) + DWK6(1)                                       
      DWK8(2) = DWK5(2) + DWK6(2)                                       
      DWK8(3) = DWK5(3) + DWK6(3)                                       
C     WRITE(7,*) ' J2000 POSITION = ', DWK7                             
C     WRITE(7,*) ' J2000 VELOCITY = ', DWK8                             
C                                                                       
C                                      *-------------------------------*
C                                      * CONVERTING TO SPHERICAL COORD.*
C                                      *-------------------------------*
C                                      * POSITION -> DPJ20             *
      CALL R_4400( DWK7, DPJ20, JRSC)
      GOTO 100
C                               
C                                      * PROPER MOTION -> DPRPCR       *
C      DWK = DWK7(1)**2 + DWK7(2)**2
C      CALL R_3210( DWK7, DNRM, DVW )
C
C      IF( ( DPROPM(1) .EQ. 0D0 ) .AND. ( DPROPM(2) .EQ. 0D0 ) ) THEN
C        DPRPCR(1) = 0D0
C        DPRPCR(2) = 0D0
C      ELSE
C        DPRPCR(1) = ( DWK7(1) * DWK8(2) - DWK7(2) * DWK8(1) )
C     +              / 15D0 / DWK
C     +              * DHSRAD
C        DPRPCR(2) =  ( DWK8(3) * DWK
C     +              - DWK7(3) * ( DWK7(1)*DWK8(1) + DWK7(2)*DWK8(2) ) )
C     +              / DNRM**2 / DSQRT( DWK )
C     +              * DACRAD
C      ENDIF
C          
C                                      * ANNUAL PARALLAX -> DANCRC     *
C      DANCRC = DANPRL / DNRM * DACRAD
C                                    
C                                      * RADIAL VELOCITY -> DVELCR     *
C      CALL DSUM( DWK7, DWK8, 3, 1, 1, DPVS)
C      IF ( DANPRL .EQ. 0D0 ) THEN
C        DVELCR = DRVELC * 1D3
C      ELSE
C        DVELCR = DCVE * DPVS / DANPRL / DNRM *1D3
C      ENDIF
C
  100 CONTINUE
      RETURN
      END                                                               
