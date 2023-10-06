      SUBROUTINE EI_330 ( JERCOD, DCOMET )
C----------------------------------------------------------------------C
C     MODULE NAME : EI_330                                             C
C                                                                      C
C     FUNCTION    : DERIVATION OF THE MAP CENTER                       C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1990.09.19                      C
C                                                                      C
C     -NOTE-                                                           C
C                                                                      C
C       THE MODULE IS EFFECTIVE FOR THE POSITIO OF A PLANET.           C
C                                                                      C
C----------------------------------------------------------------------C
C                                       *------------------------------*
C                                       * DECLARE DIVITION             *
C                                       *------------------------------*
#include      "__DFTF"
C                                       * ARGUMENT VARIABLE DEFINITION *
      INTEGER*2   JERCOD(2)             ! リターンコード
      REAL*8      DCOMET(6)             ! 彗星軌道６要素
C                                       * COMMON AREA OF INSTRUCTION   *
#include      "EI_TTME"
#include      "EI_LMAI"
#include      "XEPHEM"
#include      "XSTINF"
#include      "XOBSEI"
#include      "XVWORK"
#include      "XTRK00"
#include      "XCOMET"
C                                                                       
C                                       * WORK VARIABLE DEFINITION     *
C                                                                       
      REAL*8      DMBUF , DTOLE, DRTOLE , DWR(2)                               
      REAL*8      DOBPCR(3), DOBPVL(3), DEQACR(3), DEQAVL(3)                 
      INTEGER*2   JST1, JST2, JRC(2)                                    
      CHARACTER   CEL, CM(3)*79                                         
C                                       * CONSTANT DATA DEFINITION     *
      DATA  JST1, JST2 / 1, 2 /                                         
      DATA  CEL, CM(1) / 'E', 'XEI301E EI_330  EPH CACULATION ERROR.' / 
      DATA       CM(2) /      'XEI301Q EI_330  ITERATION FAILED.' /     
      DATA       CM(3) /    'XEI301Q EI_330  COMET ITERATION FAILED.' /     
      PARAMETER  ( DRTOLE = 1D-07 )                                     
      PARAMETER  ( DTOLE = 1D-07 )                                       
C                                                                       
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C
C リターンコードの初期化
      JERCOD(1) = 0
      JERCOD(2) = 0
      JRC(1) = 0
      JRC(2) = 0
      IF ( DCCDST .NE. -1.0D0 ) THEN
        DPLDIS = DCCDST
        CALL R_4500_cmt( DEPSFE, DCCPLP, DCCDST )
      ENDIF
C
C 太陽の位置、地球の日心位置を求める
      CALL R_3100( DTSTJD , JST1 , JRC(1) )
      IF ( JRC(1) .NE. 0 ) GOTO 9000
C
C 観測天体の位置が元期での赤経赤緯 or 銀経銀緯で与えられた場合
      IF ( JESTCR .EQ. 1 ) THEN
        DXOBPS(1) = DEPSFE(1)
        DXOBPS(2) = DEPSFE(2)
        GOTO 100
      ENDIF
C
C 観測天体の位置が方位角、仰角で与えられた場合
      IF ( JESTCR .EQ. 2 ) THEN
        DAPSAE(1, 1 ) = DEPSFE(1)
        DAPSAE(2, 1 ) = DEPSFE(2)
        GOTO 100
      ENDIF
C
C 観測天体が彗星の場合
      IF (CESTNM .EQ. 'COMET') THEN
C                                      *-------------------------------*
C                                      * COMET : B1950 -> J2000        *
C                                      *-------------------------------*
        IF ( ITRKBJ .EQ. 1 ) THEN                             
          CALL R_1200( DCOMET, JRC )                                
        ELSE                                                        
          DO 1110 I = 1, 6                                          
            DORBJ2(I) = DCOMET(I)                                   
 1110     CONTINUE                                                  
        ENDIF                                                       
c       WRITE(*,*) ' EZ_310 ORBITAL ELEMENT :'                            
c       WRITE(*,*) '  B1950 = ', DCOMET                                   
c       WRITE(*,*) '  J2000 = ', DORBJ2                                   
C
        DO 3000 JC = 1 , 5 , 1                                          
c         WRITE(*,*) ' EZ_330 ITERATIONS (COMET) : ', JC                    
          DMBUF = DPLDIS                                                
C                                      > SOLVE KEPLER EQUATION         <
C                                                                       
          CALL R_3300( DOBPCR, DOBPVL, JRC(1) )                            
c         WRITE(*,*) ' EZ_330 DOBPCR = ', DOBPCR                            
c         WRITE(*,*) ' EZ_330 DOBPVL = ', DOBPVL                            
          IF ( JRC(1) .GT. 100 ) GOTO 9100                                 
C                                      > ECLIPTIC -> EQUATORIAL        <
C                                                                       
          CALL R_3400( DOBPCR, DOBPVL, DEQACR, DEQAVL )                 
c         WRITE(*,*) ' EZ_330 DEQACR = ', DEQACR                            
c         WRITE(*,*) ' EZ_330 DEQAVL = ', DEQAVL                            
C                                                                       
C                                      > HELIOCENTRIC  -> BARYCENTRIC  <
C                                                                       
C         WRITE(*,*) ' EZ_330 LIGHT-TIME CORRECTED ', DXEMTM                
          CALL R_3500( DTSTJD, DEQACR, DEQAVL, JRC(1) )                    
C         write(*,*)'EI_330  JRC(1) OF R_3500=', JRC(1)
          IF ( JRC(1) .GT. 100 ) GOTO 9000                                 
C                                      > DERIVE LIGHT-TIME             <
          CALL R_3200( CESTNM )                                         
c         WRITE(*,*) ' EZ_330 LIGHT-TIME CORRECTED ', DXEMTM                
c         WRITE(*,*) ' EZ_330 DPLDIS, DMBUF = ', DPLDIS, DMBUF              
C                                                                       
          IF ( DABS( DPLDIS - DMBUF ) / DPLDIS .LT. DTOLE ) THEN        
            CALL R_4400( DPLUNT , DWR , JRC )                           
            DXOBPS(1) = DWR(1)                                          
            DXOBPS(2) = DWR(2)                                          
c           write(*,*)'EI_330  DXOBPS=', DXOBPS
C           WRITE(*,*) 'EZ_330 COMMET DEBUG DOBPCR =  ', DOBPCR              
C           WRITE(*,*) 'EZ_330 COMMET DEBUG DOBPVL =  ', DOBPVL              
C           WRITE(*,*) 'EZ_330 COMMET DEBUG DEPSFE =  ', DEPSFE              
            GOTO 100                                                    
          ENDIF                                                         
C                                                                       
 3000   CONTINUE                                                        
C ５回で収束しなかった場合
        GOTO 9200                                                       
      ENDIF
C
C 観測天体が惑星の場合
      DO 2000 JC = 1 , 5 , 1
        DMBUF = DPLDIS
C 太陽の位置、地球の日心位置を求める
        CALL R_3100( DTSTJD , JST2, JRC(2) )
C エラーの場合
        IF ( JRC(2) .NE. 0 ) GOTO 9000
C 光行差算出
        CALL R_3200( CESTNM )
C 収束したかどうかの判定
        IF ( DABS( DPLDIS - DMBUF ) / DPLDIS .LT. DRTOLE ) THEN         
C 収束した場合、惑星の球面座標値を求める
          CALL R_4400( DPLUNT , DWR , JRC )
          DXOBPS(1) = DWR(1)
          DXOBPS(2) = DWR(2)
          GOTO 100
        ENDIF
 2000 CONTINUE
C ５回で収束しなかった場合
      GOTO 9100
C
  100 CONTINUE
C                                                                       
      IF ( DCCDST .NE. -1.0D0 ) THEN
        DPLDIS = DCCDST
      ENDIF
      RETURN
C----------------------------------------------------------------------C
C ERROR ENTRY                                                          C
C----------------------------------------------------------------------C
C
 9000 CONTINUE
      IF( ( JRC(1) .NE. 0 ) .OR. ( JRC(2) .NE. 0 ) ) THEN
        JERCOD(1) = 4
        CALL O_MSG2( IMID00, CEL, CM(1), IRC )
      ENDIF
      GOTO 9999
C
 9100 CONTINUE
      JERCOD(2) = 1
      CALL O_MSG2( IMID00, CEL, CM(2), IRC )
      GOTO 9999
C                                                                       
 9200 CONTINUE                                                          
      CALL O_MSG2( IMID00, CEL, CM(3), IRC )
      JERCOD(2) = 1
      GOTO 9999
C----------------------------------------------------------------------C
C EXIT                                                                 C
C----------------------------------------------------------------------C
 9999 CONTINUE
      RETURN
      END
