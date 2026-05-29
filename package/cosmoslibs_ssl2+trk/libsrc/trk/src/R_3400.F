      SUBROUTINE R_3400 ( DOBPCR, DOBPVL, DEQACR, DEQAVL )              
C----------------------------------------------------------------------C
C     MODULE NAME : R_3400                                             C
C                                                                      C
C     FUNCTION    : COORDINATES TRANSFORMATION ;                       C
C                   ECLIPTIC TO EQUATORIAL ( CARTESIAN )               C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1991.06.13                      C
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
C                                                                       
      REAL*8       DOBPCR(3), DOBPVL(3), DEQACR(3), DEQAVL(3)           
C                                                                       
C                                       * COMMON AREA OF INSTRUCTION   *
      INCLUDE 'XEPHEM'                                                  
      INCLUDE 'XPRCNT'                                                  
C                                                                       
C                                       * WORK VARIABLE DEFINITION     *
C                                                                       
      REAL*8     DRTM1(3,3), DRTM2(3,3), DVW1(3), DVW2(3)               
C                                                                       
C                                       * CONSTANT DATA DEFINITION     *
      DATA    JX, JZ / 1, 3 /                                           
C                                                                       
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C                                      *-------------------------------*
C                                      * BEGIN                         *
C                                      *-------------------------------*
C                                      * TRANSFORMATION MATRIX         *
C     WRITE(7,*) ' R_3400 START ]]'                                     
      DWK = - DORBJ2(4)                                                 
      CALL R_5410( DWK, DRTM1, JZ, JRC )                                
C     WRITE(7,*) ' R_3400 MATRIX 1 = ', DRTM1                           
      DWK = - DORBJ2(6)                                                 
      CALL R_5410( DWK, DRTM2, JX, JRC )                                
C     WRITE(7,*) ' R_3400 MATRIX 2 = ', DRTM2                           
C                                                                       
      CALL DMGGM( DRTM2, 3, DRTM1, 3, DRTM2, 3, 3, 3, 3, DVW1, IRC )    
C                                                                       
      DWK = - DORBJ2(5)                                                 
      CALL R_5410( DWK, DRTM1, JZ, JRC )                                
C     WRITE(7,*) ' R_3400 MATRIX 3 = ', DRTM1                           
C                                                                       
      CALL DMGGM( DRTM1, 3, DRTM2, 3, DRTM1, 3, 3, 3, 3, DVW1, IRC )    
C                                                                       
      DWK = - DMNOBL                                                    
      CALL R_5410( DWK, DRTM2, JX, JRC )                                
C     WRITE(7,*) ' R_3400 MATRIX 4 = ', DRTM2                           
C                                                                       
      CALL DMGGM( DRTM2, 3, DRTM1, 3, DRTM2, 3, 3, 3, 3, DVW1, IRC )    
C     WRITE(7,*) ' R_3400 TRANSFORMATION MATRIX = ', DRTM2              
C                                                                       
C                                      * TRANSFORM POSITION VECTOR     *
      DVW1(1) = DOBPCR(1)                                               
      DVW1(2) = DOBPCR(2)                                               
      DVW1(3) = DOBPCR(3)                                               
      CALL DMAV( DRTM2, 3, 3, 3, DVW1, DVW2, ICON )                     
      DEQACR(1) = DVW2(1)                                               
      DEQACR(2) = DVW2(2)                                               
      DEQACR(3) = DVW2(3)                                               
C                                      * TRANSFORM VELOCITY VECTOR     *
      DVW1(1) = DOBPVL(1)                                               
      DVW1(2) = DOBPVL(2)                                               
      DVW1(3) = DOBPVL(3)                                               
      CALL DMAV( DRTM2, 3, 3, 3, DVW1, DVW2, ICON )                     
      DEQAVL(1) = DVW2(1)                                               
      DEQAVL(2) = DVW2(2)                                               
      DEQAVL(3) = DVW2(3)                                               
C                                                                       
      RETURN                                                            
      END                                                               
