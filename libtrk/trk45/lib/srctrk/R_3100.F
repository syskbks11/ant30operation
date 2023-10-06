      SUBROUTINE R_3100 ( DTSTJD, JSTCOD, JERCOD )                      
C----------------------------------------------------------------------C
C     MODULE NAME : R_3100                                             C
C                                                                      C
C     FUNCTION    : DERIVATION OF THE SUN'S AND A PLANET'S POSITIONS   C
C                   AND VELOCITIES.                                    C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1990.09.13                      C
C                                                                      C
C     -NOTE-                                                           C
C                                                                      C
C      NONE                                                            C
C                                                                      C
C----------------------------------------------------------------------C
C                                       *------------------------------*
C                                       * DECLARE DIVITION             *
C                                       *------------------------------*
#include      "__DFTF"
C                                       * ARGUMENT VARIABLE DEFINITION *
      REAL*8        DTSTJD(8)
      INTEGER*2     JSTCOD, JERCOD
C                                       * COMMON AREA OF INSTRUCTION   *
#include      "XTMERT"
#include      "XEPHEM"
#include      "XCOMET"
C
C                                       * WORK VARIABLE DEFINITION     *
C
      REAL*8        DCF(8), DXT(6), DPV, VW(8)
      INTEGER*4     ICON, IDPOS
C                                       * INITIALIZE DATA              *
      DATA    IDPOS  / 2 /
C
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C                                      *-------------------------------*
C リターンコード等の初期化
      JERCOD    = 0
      DPV       = 0D0
C
C 太陽、地球の位置ベクトル場合
C 太陽（太陽系中心座標）
C 地球（太陽中心座標）
      IF ( JSTCOD .EQ. 1 ) THEN
C
C 太陽
C Ｘ、Ｙ、Ｚ成分の３回ループ
        DO 1000 IC1 = 1, 3
C スプライン補間係数をコピーする
          DO 1100 IC2 = 1, 8
             DCF(IC2) = DINTBR(IC2, IC1, 1)
             IF ( IC2 .LE. 6)  DXT(IC2) = DBRNOD(IC2, IC1, 1)
 1100     CONTINUE
C
C スプライン補間を行なう
          CALL DBIF3(DTSTJD, 8, 3, DCF, DXT, 0, DTDBJD, IDPOS,
     +               DPV, VW, ICON )
          IF ( ICON .EQ. 30000 ) GOTO 9000
          DSLPOS(IC1) = DPV
C
C 地球
C スプライン補間係数をコピーする
          DO 1200 IC2 = 1, 8
            DCF(IC2) = DINTHL(IC2, IC1, 1)
            IF ( IC2 .LE. 6 )  DXT(IC2) = DHLNOD( IC2, IC1, 1)
 1200     CONTINUE 
C
C スプライン補間を行なう
          CALL DBIF3(DTSTJD, 8, 3, DCF, DXT, 0, DTDBJD, IDPOS,
     +               DPV, VW, ICON)
          IF ( ICON .EQ. 30000 ) GOTO 9000
          DOPHEL(IC1) = DPV
C
 1000   CONTINUE
      ENDIF
C
C 太陽、地球以外の惑星
C 太陽系中心座標における位置、速度ベクトル
      IF ( JSTCOD .EQ. 2 ) THEN
        IF ( DCCDST .NE. -1.0D0 ) THEN
          DPLPOS(1) = DCCPLP(1)
          DPLPOS(2) = DCCPLP(2)
          DPLPOS(3) = DCCPLP(3)
          DPLBVL(1) = 0.0D0
          DPLBVL(2) = 0.0D0
          DPLBVL(3) = 0.0D0
        ELSE
C
C Ｘ、Ｙ、Ｚ成分の３回ループ
          DO 2000 IC1 = 1, 3
C
C スプライン補間係数をコピーする
            DO 2100 IC2 = 1, 8
              DCF(IC2) = DINTBR(IC2, IC1, 3)
              IF ( IC2 .LE. 6 )  DXT(IC2) = DBRNOD(IC2, IC1, 3)
 2100       CONTINUE
C
C 位置ベクトルをスプライン補間して求める
            CALL DBIF3(DTSTJD, 8, 3, DCF, DXT, 0, DXEMTM, IDPOS,
     +                 DPV, VW, ICON )
            IF ( ICON .EQ. 30000 ) GOTO 9000 
            DPLPOS(IC1) = DPV
C
C 速度ベクトルをスプライン補間して求める
            CALL DBIF3(DTSTJD, 8, 3, DCF, DXT, 1, DXEMTM, IDPOS,
     +                 DPV, VW, ICON )
            IF ( ICON .EQ. 30000 ) GOTO 9000
C
C 速度の単位を[m/day]から[m/sec]へ変換する
            DPLBVL(IC1) = DPV / 86400D0
 2000     CONTINUE
        ENDIF
      ENDIF
C
C 地球の速度ベクトルを求める場合
      IF ( JSTCOD .EQ. 3 ) THEN
C
C Ｘ、Ｙ、Ｚ成分の３回ループ
        DO 3000 IC1 = 1, 3
C
C スプライン補間係数をコピーする
          DO 3100 IC2 = 1, 8
            DCF(IC2) = DINTBR(IC2, IC1, 2)
            IF ( IC2 .LE. 6 )  DXT(IC2) = DBRNOD(IC2, IC1, 2)
 3100     CONTINUE
C
C 速度ベクトルをスプライン補間して求める
          CALL DBIF3(DTSTJD, 8, 3, DCF, DXT, 1, DTDBJD, IDPOS,
     +               DPV, VW, ICON)
          IF ( ICON .EQ. 30000 ) GOTO 9000
C
C 速度の単位を[m/day]から[m/sec]へ変換する
          DOPBVL(IC1) = DPV / 86400D0
 3000   CONTINUE
      ENDIF
C
      RETURN
C----------------------------------------------------------------------C
C ERROR ENTRY                                                          C
C----------------------------------------------------------------------C
C
 9000 CONTINUE
      JERCOD = 4
C----------------------------------------------------------------------C
C EXIT                                                                 C
C----------------------------------------------------------------------C
      RETURN
      END
