      SUBROUTINE DLAX(A, K, N, B, EPSZ, ISW, VW, IP, ICON)
C***  For three unknowns by Maekawa 2012/11/04 ***
      REAL*8 A(3,3), B(3), EPSZ, VW(3), IP(3)
      INTEGER*4 K, N, ISW, ICON
C     
      REAL*8 DET, AINV(3,3), X(3)
C     
      IF (K .NE. 3 .OR. N .NE. 3) THEN
         ICON = 10000
         RETURN
      ENDIF
C     
      DET = A(1,1)*A(2,2)*A(3,3) + A(2,1)*A(3,2)*A(1,3)
     *     + A(3,1)*A(1,2)*A(2,3) - A(1,1)*A(3,2)*A(2,3)
     *     - A(3,1)*A(2,2)*A(1,3) - A(2,1)*A(1,2)*A(3,3)
C     
      IF (DET .EQ. 0.0) THEN
         ICON = 200000
         RETURN
      ENDIF
C     
      AINV(1,1) = (A(2,2)*A(3,3) - A(2,3)*A(3,2)) / DET
      AINV(1,2) = (A(1,3)*A(3,2) - A(1,2)*A(3,3)) / DET
      AINV(1,3) = (A(1,2)*A(2,3) - A(1,3)*A(2,2)) / DET
      AINV(2,1) = (A(2,3)*A(3,1) - A(2,1)*A(3,3)) / DET
      AINV(2,2) = (A(1,1)*A(3,3) - A(1,3)*A(3,1)) / DET
      AINV(2,3) = (A(1,3)*A(2,1) - A(1,1)*A(2,3)) / DET
      AINV(3,1) = (A(2,1)*A(3,2) - A(2,2)*A(3,1)) / DET
      AINV(3,2) = (A(1,2)*A(3,1) - A(1,1)*A(3,2)) / DET
      AINV(3,3) = (A(1,1)*A(2,2) - A(1,2)*A(2,1)) / DET
C     
      X(1) = AINV(1,1)*B(1) + AINV(1,2)*B(2) + AINV(1,3)*B(3)
      X(2) = AINV(2,1)*B(1) + AINV(2,2)*B(2) + AINV(2,3)*B(3)
      X(3) = AINV(3,1)*B(1) + AINV(3,2)*B(2) + AINV(3,3)*B(3)
C     
      B(1) = X(1)
      B(2) = X(2)
      B(3) = X(3)
      ICON = 0
      RETURN
      END
