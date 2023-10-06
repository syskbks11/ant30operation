      SUBROUTINE R_5110                                                 
C----------------------------------------------------------------------C
C     MODULE NAME : R_5110                                             C
C                                                                      C
C     FUNCTION    : CORRECTION OF THE PROPER MOTION & ANNUAL PARALLAX  C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1997.01.31                      C
C                                                                      C
C     -NOTE-                                                           C
C                                                                      C
C        FOR ONLY PLANET AND COMET.    MODIFIED FROM R_5100.           C
C                                                                      C
C----------------------------------------------------------------------C
C                                       *------------------------------*
C                                       * DECLARE DIVITION             *
C                                       *------------------------------*
      INCLUDE '__DFTF'                                                  
C                                       * COMMON AREA OF INSTRUCTION   *
      INCLUDE 'XIERST'                                                  
      INCLUDE 'XSTINF'                                                  
      INCLUDE 'XTMERT'                                                  
      INCLUDE 'XEPHEM'                                                  
      INCLUDE 'XVWORK'                                                  
      INCLUDE 'XDUMMY'                                                  
C                                       * WORK VARIABLE DEFINITION     *
C                                                                       
CCC   REAL*8      DAUSC , DWVC1(3) , DWVC2(3) , DWVC3(3) ,              
CCC  +            DNM , DMOV(3)                                         
      REAL*8              DWVC1(3) , DWVC2(3) , DWVC3(3) ,              
     +            DNM , DMOV(3)                                         
      INTEGER*2   JC                                                    
C                                       * CONSTANT DATA DEFINITION     *
CCC   PARAMETER  ( DAUSC = 2.1094502D-2 )                               
C                                                                       
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C                                      *-------------------------------*
C                                      * INITIALIZE                    *
C                                      *-------------------------------*
      DMOV(1) = 0D0                                                     
      DMOV(2) = 0D0                                                     
      DMOV(3) = 0D0                                                     
C                                                                       
C                                      *-------------------------------*
C                                      * VELOCITY VECTOR               *
C                                      *-------------------------------*
      DWVC1(1) = - DCOS( DXOBPS(2) ) * DSIN( DXOBPS(1) )                
      DWVC1(2) =   DCOS( DXOBPS(2) ) * DCOS( DXOBPS(1) )                
      DWVC1(3) =   0D0                                                  
      DWVC2(1) = - DSIN( DXOBPS(2) ) * DCOS( DXOBPS(1) )                
      DWVC2(2) = - DSIN( DXOBPS(2) ) * DSIN( DXOBPS(1) )                
      DWVC2(3) =   DCOS( DXOBPS(2) )                                    
      DWVC3(1) =   DXSTPS(1)                                            
      DWVC3(2) =   DXSTPS(2)                                            
      DWVC3(3) =   DXSTPS(3)                                            
C                                                                       
      DO 1000 JC = 1 , 3 , 1                                            
C       DMOV( JC ) =  DPRPCR(1) * DWVC1( JC ) + DPRPCR(2) * DWVC2( JC ) 
C    +              + DAUSC * DVELCR * DANCRC * DWVC3( JC )             
        DMOV( JC ) =  0                                                 
 1000 CONTINUE                                                          
C                                                                       
C                                      *-------------------------------*
C                                      * CORRECTION                    *
C                                      *-------------------------------*
      DO 2000 JC = 1 , 3 , 1                                            
C       DWVC1( JC ) = DXSTPS( JC ) + DCENTR(1) * DMOV( JC ) -           
C    +                DANCRC * DOPBAR( JC ) / DASUNT                    
        DWVC1( JC ) = DXSTPS( JC )                                      
 2000 CONTINUE                                                          
C                                                                       
      CALL R_3210( DWVC1 , DNM , DPRPMT )                               
C                                                                       
      RETURN                                                            
      END                                                               
