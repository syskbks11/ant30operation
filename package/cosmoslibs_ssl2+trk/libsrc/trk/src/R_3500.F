      SUBROUTINE R_3500 ( DTSTJD, DEQACR, DEQAVL, JECODE )              
C----------------------------------------------------------------------C
C     MODULE NAME : R_3500                                             C
C                                                                      C
C     FUNCTION    : COORDINATES TRANSFORMATION ;                       C
C                   HELIOCENTRIC TO BARYCENTRIC                        C
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
C                                                                       
      REAL*8       DTSTJD(8), DEQACR(3), DEQAVL(3)                      
C                                                                       
C                                       * COMMON AREA OF INSTRUCTION   *
      INCLUDE 'XEPHEM'                                                  
C                                                                       
C                                       * WORK VARIABLE DEFINITION     *
C                                                                       
      REAL*8     DVWC(8), DVWN(6), DVW(8)                               
C                                                                       
C                                       * CONSTANT DATA DEFINITION     *
      DATA    DAY, IDPOS / 86400D0, 2 /                                 
C                                                                       
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C                                      *-------------------------------*
C                                      * INITIALIZE                    *
C                                      *-------------------------------*
C     WRITE(7,*) ' R_3500 START ]]'                                     
      JECODE = 0                                                        
C                                      *-------------------------------*
C                                      * BEGINE                        *
C                                      *-------------------------------*
C                                      * SOLAR POS. AT TIME OF EMMITION*
      DO 1000 JC1 = 1, 3                                                
C                                                                       
        DO 1100 JC2 = 1, 8                                              
          DVWC( JC2 ) = DINTBR( JC2, JC1, 1 )                           
          IF ( JC2 .LE. 6 ) THEN                                        
            DVWN( JC2 ) = DBRNOD( JC2, JC1, 1 )                         
          ENDIF                                                         
 1100   CONTINUE                                                        
C                                      * DERIVE POSITION               *
C                                                                       
C       write(*,*) DTSTJD, 8, 3, DVWC, DVWN, 0, DXEMTM, IDPOS, DPV,    
C    +              DVW, IRC
        CALL DBIF3( DTSTJD, 8, 3, DVWC, DVWN, 0, DXEMTM, IDPOS, DPV,    
     +              DVW, IRC )                                          
        IF ( IRC .EQ. 30000 ) GOTO 9000                                 
C C     WRITE(7,*) ' R_3500 SOLAR POS. (',JC1,') = ', DPV                 
C                                                                       
        DPLPOS( JC1 ) = DEQACR( JC1 ) + DPV                             
C                                                                       
C                                      * DERIVE VELOCITY               *
C                                                                       
        CALL DBIF3( DTSTJD, 8, 3, DVWC, DVWN, 1, DXEMTM, IDPOS, DPV,    
     +              DVW, IRC )                                          
        IF ( IRC .EQ. 30000 ) GOTO 9000                                 
C C     WRITE(7,*) ' R_3500 SOLAR VEL. (',JC1,') = ', DPV/DAY             
C                                                                       
        DPLBVL( JC1 ) = DEQAVL( JC1 ) + DPV / DAY                       
C                                                                       
 1000 CONTINUE                                                          
C     WRITE(7,*) ' R_3500 COMET POS. = ', DPLPOS                        
C     WRITE(7,*) ' R_3500 COMET VEL. = ', DPLBVL                        
C                                                                       
      RETURN                                                            
C                                                                       
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
 9000 CONTINUE                                                          
      JECODE = 1000                                                     
      DPLPOS(1) = DEQACR(1)                                             
      DPLPOS(2) = DEQACR(2)                                             
      DPLPOS(3) = DEQACR(3)                                             
      DPLBVL(1) = DEQAVL(1)                                             
      DPLBVL(2) = DEQAVL(2)                                             
      DPLBVL(3) = DEQAVL(3)                                             
C     WRITE(7,*) ' R_3500 ERROR END ]] --- EPH'                         
C                                                                       
      RETURN                                                            
      END                                                               
