      SUBROUTINE R_4500 ( DSPHRC, DCARTC )
C----------------------------------------------------------------------C
C     MODULE NAME : R_4500                                             C
C                                                                      C
C     FUNCTION    : COORDINATES TRANSFORMATION ; SPHERICAL TO CARTESIANC
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
#include      "__DFTF"
C                                       * ARGUMENT VARIABLE DEFINITION *
      REAL*8      DSPHRC(2)             ! 球面座標値
      REAL*8      DCARTC(3)             ! デカルト座標値
C
C                                       * WORK VARIABLE DEFINITION     *
      REAL*8      DNRM                  ! ベクトルの絶対値
C                                       * CONSTANT DATA DEFINITION     *
      DATA  DNRM  / 1D0 /
C
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C
C 変換開始
      DCARTC(1) = DNRM * DCOS( DSPHRC(2) ) * DCOS( DSPHRC(1) )
      DCARTC(2) = DNRM * DCOS( DSPHRC(2) ) * DSIN( DSPHRC(1) )
      DCARTC(3) = DNRM * DSIN( DSPHRC(2) )
C
      RETURN
      END
