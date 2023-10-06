C----------------------------------------------------------------------C
C     MODULE NAME : R_1300                                             C
C                                                                      C
C     FUNCTION    : COORDINATE TRANSFORMATION OF EPHEMERIS DATA        C
C                   FROM HELIOCENTRIC TO BARYCENTRIC COORDINATES       C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1990.09.12                      C
C                                                                      C
C     -NOTE-                                                           C
C                                                                      C
C      DISTANCE EXPRESSED IN THE ASTRONOMICAL UNIT IS CONVERTED TO     C
C      SI UNIT ( METER )                                               C
C                                                                      C
C----------------------------------------------------------------------C
C                                       *------------------------------*
C                                       * DECLARE DIVITION             *
C                                       *------------------------------*
      SUBROUTINE R_1300( DTPLPS, JSTCOD, CESTNM, JERCOD )
C
#include      "__DFTF"
C                                       * COMMON AREA OF INSTRUCTION   *
#include      "XEPHEM"
#include      "XIERST"
C                         
      REAL*8        DTPLPS(8,3,10)      ! ＥＰＨファイルデータ
      INTEGER*2     JSTCOD              ! 惑星フラグ
      INTEGER*2     JERCOD              ! リターンコード
      CHARACTER*16  CESTNM              ! 惑星名
C                                                                       
C                                       * WORK VARIABLE DEFINITION     *
      REAL*8        DPCNTR(8,3), DMASST, DCRDNT, DWK
      INTEGER*2     JCOORD, JDSNUM, JPLTNO, JC1
      CHARACTER*16  CPLTNM(11)
C 惑星名
      DATA   CPLTNM /'MERCURY', 'VENUS', 'EARTH', 'MARS', 'JUPITER',
     +              'SATURN', 'URANUS', 'NEPTUNE', 'PLUTO',
     +              'MOON' , 'SUN' /
C                                                                       
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C
C リターンコード、惑星番号の初期化
      JERCOD = 0
      JPLTNO = 0
C                                                                       
C                                      *-------------------------------*
C                                      * DERIVE HELIOCENTRIC POSITION  *
C                                      * OF SOLAR SYSTEM BARYCENTER    *
C                                      *-------------------------------*
C 太陽系の総質量算出
      DMASST = 1D0
      DO 500 JC1 = 1 , 9
        DMASST = DMASST + 1D0 / DMRPPS( JC1 )
  500 CONTINUE
      DMASST = DMASST + DMRLPE / DMRPPS(3)
C
C 太陽系中心ベクトル算出
      DO 600 JCOORD = 1 , 3
        DO 610 JDSNUM = 1 , 8
          DCRDNT = 0D0
          DO 611 JC1 = 1 , 9
            DCRDNT = DCRDNT + DTPLPS( JDSNUM , JCOORD , JC1 ) /
     +                        DMRPPS( JC1 )
  611     CONTINUE
          DCRDNT = DCRDNT + DTPLPS( JDSNUM , JCOORD , 10 ) *
     +                      DMRLPE / DMRPPS(3)
          DPCNTR( JDSNUM , JCOORD ) = DCRDNT / DMASST
  610   CONTINUE
  600 CONTINUE
C                                                                       
C 太陽系中心、太陽からのベクトルへ変換
      DO 700 JCOORD = 1 ,3
        DO 710 JDSNUM = 1 ,8
C 太陽系中心からの太陽
          DBARAG( JDSNUM, JCOORD, 1 ) = - DPCNTR( JDSNUM, JCOORD )
     +                                  * DASUNT
C 太陽系中心からの地球
          DBARAG( JDSNUM, JCOORD, 2 ) = (  DTPLPS( JDSNUM, JCOORD, 3 )
     +                                   - DPCNTR( JDSNUM, JCOORD ) )
     +                                  * DASUNT
C
C 太陽からの地球
          DHELAG( JDSNUM, JCOORD, 1 ) = DTPLPS( JDSNUM, JCOORD, 3 )
     +                                  * DASUNT
  710   CONTINUE
  700 CONTINUE
C                                                                       
C 指定された天体が惑星の場合のベクトルを求める
      IF ( JSTCOD .EQ. 3 ) THEN
C 指定された惑星を探す
        DO 800 JC1 = 1 , 11
          IF ( CESTNM .EQ. CPLTNM( JC1 ) ) THEN
            JPLTNO = JC1               ! 惑星番号
            GOTO 3000                  ! ループを抜ける
          ENDIF
  800   CONTINUE
C 見つからなかった
        GOTO 9000
C                                                                       
 3000   CONTINUE
C 指定された惑星についてベクトルの計算をする
        DO 900 JCOORD = 1 , 3
          DO 910 JDSNUM = 1 , 8
C 太陽の場合
            IF ( JPLTNO .EQ. 11 ) THEN
              DWK = 0D0
C 太陽以外の場合
            ELSE
              DWK = DTPLPS( JDSNUM, JCOORD, JPLTNO )
            ENDIF
C 太陽系中心からの位置ベクトル
            DBARAG( JDSNUM , JCOORD , 3 )
     +      = ( DWK - DPCNTR( JDSNUM, JCOORD ) ) * DASUNT
C 太陽からの位置ベクトル
            DHELAG( JDSNUM, JCOORD, 2 ) = DWK * DASUNT
C                                                                       
  910     CONTINUE
  900   CONTINUE
C
      ENDIF
C
C
      RETURN
C----------------------------------------------------------------------C
C ERROR ENTRY                                                          C
C----------------------------------------------------------------------C
C
 9000 CONTINUE
      JERCOD = 1
C
C----------------------------------------------------------------------C
C EXIT                                                                 C
C----------------------------------------------------------------------C
      RETURN
      END
