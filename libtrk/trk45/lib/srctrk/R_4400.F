C----------------------------------------------------------------------C
C     MODULE NAME : R_4400                                             C
C                                                                      C
C     FUNCTION    : COORDINATES TRANSORMATION ; CARTESIAN TO SPHERICAL C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1990.07.31                      C
C                                                                      C
C     -NOTE-                                                           C
C                                                                      C
C      NONE                                                            C
C                                                                      C
C----------------------------------------------------------------------C
C                                       *------------------------------*
C                                       * DECLARE DIVITION             *
C                                       *------------------------------*
      SUBROUTINE R_4400 ( DCARTC, DSPHRC, JERCOD )
#include      "__DFTF"
C                                       * ARGUMENT VARIABLE DEFINITION *
      REAL*8      DCARTC(3)             ! デカルト座標値
      REAL*8      DSPHRC(2)             ! 球面座標値
      INTEGER*2   JERCOD                ! リターンコード
C                                       * COMMON AREA OF INSTRUCTION   *
#include      "XMATHC"
C                                       * WORK VARIABLE DEFINITION     *
C                                                                       
      REAL*8   DNRM                                                     
C                                                                       
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C
C リターンコードの初期化
      JERCOD    = 0
C
C Ｚ軸上の特異点
      IF ( ( DCARTC(1) .EQ. 0D0 ) .AND.
     +     ( DCARTC(2) .EQ. 0D0 ) ) THEN
        GOTO 9000
      ENDIF
C
C ベクトルの絶対値を求める
      CALL DSUM( DCARTC , DCARTC , 3 , 1 , 1 , DNRM )
      DNRM = DSQRT( DNRM )
C
C ψ、θを求める
      DSPHRC(1) = DATAN( DCARTC(2) / DCARTC(1) )
      DSPHRC(2) = DASIN( DCARTC(3) / DNRM )
C
C ａｒｃｔａｎの不確定性を解く
      IF ( ( DCARTC(2) .LT. 0D0 ) .AND.
     +     ( DCARTC(1) .GT. 0D0 ) ) THEN
        DSPHRC(1) = DSPHRC(1) + DTWOPI
      ELSE IF ( DCARTC(1) .LT. 0D0 ) THEN
        DSPHRC(1) = DSPHRC(1) + DPIVAL
      ENDIF
C
      RETURN
C----------------------------------------------------------------------C
C ERROR ENTRY                                                          C
C----------------------------------------------------------------------C
C
C 特異点の場合
 9000 CONTINUE
      JERCOD = 1
      IF ( DCARTC(3) .GE. 0D0 ) THEN
        DSPHRC(2) = DHLFPI
      ELSE
        DSPHRC(2) = - DHLFPI
      ENDIF
C
C----------------------------------------------------------------------C
C EXIT                                                                 C
C----------------------------------------------------------------------C
      RETURN
      END
