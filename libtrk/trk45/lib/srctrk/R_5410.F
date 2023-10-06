      SUBROUTINE R_5410 ( DANGRT, DMATRT, JROTAX, JERCOD )
C----------------------------------------------------------------------C
C     MODULE NAME : R_5410                                             C
C                                                                      C
C     FUNCTION    : GENERATION OF A ROTATION MATRIX                    C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1990.07.31                      C
C                                                                      C
C     -NOTE-                                                           C
C                                                                      C
C      DANGRT    : ROTATION ANGLE                                      C
C      DMATRT    : ROTATION MATRIX                                     C
C      JROTAX = 1: X-AXIS   2: Y-AXIS   3: Z-AXIS                      C
C                                                                      C
C----------------------------------------------------------------------C
C                                       *------------------------------*
C                                       * DECLARE DIVITION             *
C                                       *------------------------------*
#include     "__DFTF"
C                                       * ARGUMENT VARIABLE DEFINITION *
      REAL*8      DANGRT                ! 回転角
      REAL*8      DMATRT(3,3)           ! 回転行列
      INTEGER*2   JROTAX                ! 回転軸識別コード
      INTEGER*2   JERCOD                ! リターンコード
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C
C リターンコードの初期化
      JERCOD = 0
C
C 引数チェック
      IF ( ( JROTAX .LT. 1 ) .OR. ( JROTAX .GT. 3 ) ) GOTO 9000
C
C Ｘ軸周りに回転する場合
      IF ( JROTAX .EQ. 1 ) THEN
        DMATRT(1,1) =   1D0
        DMATRT(1,2) =   0D0
        DMATRT(1,3) =   0D0
        DMATRT(2,1) =   0D0
        DMATRT(2,2) =   DCOS( DANGRT )
        DMATRT(2,3) =   DSIN( DANGRT )
        DMATRT(3,1) =   0D0
        DMATRT(3,2) = - DSIN( DANGRT )
        DMATRT(3,3) =   DCOS( DANGRT )
        GOTO 100
      ENDIF
C
C Ｙ軸周りに回転する場合
      IF ( JROTAX .EQ. 2 ) THEN
        DMATRT(1,1) =   DCOS( DANGRT )
        DMATRT(1,2) =   0D0
        DMATRT(1,3) = - DSIN( DANGRT )
        DMATRT(2,1) =   0D0
        DMATRT(2,2) =   1D0
        DMATRT(2,3) =   0D0
        DMATRT(3,1) =   DSIN( DANGRT )
        DMATRT(3,2) =   0D0
        DMATRT(3,3) =   DCOS( DANGRT )
        GOTO 100
      ENDIF
C
C Ｚ軸周りに回転する場合
      IF ( JROTAX .EQ. 3 ) THEN
        DMATRT(1,1) =   DCOS( DANGRT )
        DMATRT(1,2) =   DSIN( DANGRT )
        DMATRT(1,3) =   0D0
        DMATRT(2,1) = - DSIN( DANGRT )
        DMATRT(2,2) =   DCOS( DANGRT )
        DMATRT(2,3) =   0D0
        DMATRT(3,1) =   0D0
        DMATRT(3,2) =   0D0
        DMATRT(3,3) =   1D0
      ENDIF 
C
  100 CONTINUE
C
      RETURN
C----------------------------------------------------------------------C
C ERROR ENTRY                                                          C
C----------------------------------------------------------------------C
C
C 回転軸の指定に誤りがある場合回転しない行列を作成する
 9000 CONTINUE
      JERCOD = 1
      DMATRT(1,1) = 1D0
      DMATRT(1,2) = 0D0
      DMATRT(1,3) = 0D0
      DMATRT(2,1) = 0D0
      DMATRT(2,2) = 1D0
      DMATRT(2,3) = 0D0
      DMATRT(3,1) = 0D0
      DMATRT(3,2) = 0D0
      DMATRT(3,3) = 1D0
C 
C----------------------------------------------------------------------C
C EXIT                                                                 C
C----------------------------------------------------------------------C
      RETURN
      END
