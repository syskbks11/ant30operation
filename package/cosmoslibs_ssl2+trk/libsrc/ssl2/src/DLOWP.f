      SUBROUTINE DLOWP(A, N, Z, ICON)
C***  Makeshift version by Maekawa 2012/11/07 ***
      REAL*8 A(5)
      COMPLEX*16 Z(4)
      INTEGER*4 N, ICON
C     
      REAL*8 X, Y, XB, YB, SOL(4), EPS, X0
C***  -X0 < solutions < X0, accuracy=EPS
      PARAMETER (EPS = 0.001, X0 = 10.0)
C***  Quartic equation only
      IF (N .NE. 4) THEN
         ICON = 10000
         RETURN
      ENDIF
C***  Get real solutions
      X = -X0
      Y = A(1)*X**4+A(2)*X**3+A(3)*X**2+A(4)*X+A(5)
      LOOP = -X*2/EPS
      NSOL = 0
      DO I=1,LOOP
         XB = X
         YB = Y
         X = X + EPS
         Y = A(1)*X**4+A(2)*X**3+A(3)*X**2+A(4)*X+A(5)
         IF (YB*Y .LE. 0 .AND. NSOL .LE. 3) THEN
            NSOL = NSOL+1
            SOL(NSOL) = (XB+X)/2
         ENDIF
      END DO
C***  Real solutions
      IF (NSOL .GT. 0) THEN
         DO I=1,NSOL
            Z(I) = DCMPLX(SOL(I), 0.0D00)
         END DO
C***  Imaginary solutions are dummy
         DO I=NSOL+1,4
            Z(I) = DCMPLX(0.0D00, 0.1D00)
         END DO
         ICON = 0
C***  No solution
      ELSE
         ICON = 100000
      ENDIF
C     
      END
