      SUBROUTINE HOUSE(X,N,K)
C     CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C     
C     (1) Purpose
C     Householder transformation
C     
C     (2) Author
C     version 1.0   by KOTA Nishiyama(*JST) 1999-10-26 
C     
C     *JST : Japan Science and Technology Corporation
C     
C     -------------------------------------------------------------
C     Inputs :
C     X(N,K+2)  : Originnal matrix + solusion + working area
C     X(N,1-K) : original matrix:IN/OUT
C     X(N,K+1) : solusion:IN/OUT
C     X(N,K+2) : working area
C     D(N)      : Working area
C     N         : Number of rows of X:IN
C     K         : Number of columns of X:IN
C     Outoput:
C     X(I,J):  Householder reduced form (upper triangular form)
C     
C     CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
      IMPLICIT  REAL*4(A-H,O-Z)
      DIMENSION X(N,K+2)
      DIMENSION D(4096)
      TOL = 1.0e-30
c     
      DO II=1,K
         H=0.0e0
         DO I=1, N
            D(I) = 0.0e0
         END DO
         DO I=II, N
            D(I) = X(I,II)
            H    = H + D(I)**2
         END DO
         IF(H.LE.TOL) THEN
            G = 0.0e0
            CYCLE
         ENDIF
c     
         G = SQRT(H)
         F = X(II,II)
         IF(F.GE.0.0e0) G = -G
         D(II) = F - G
         H = H - F*G
c     
         DO I=II+1, N
            X(I,II) = 0.0e0
         END DO
c     
c     *** calc X ***
c     
         DO J=II+1,K
            SS  = 0.0e0
            DO I=II,N
               SS  = SS + D(I)*X(I,J)
            END DO
            SS  = SS/H
            DO I=II,N
               X(I,J) = X(I,J) - D(I)*SS
            END DO
         END DO
c     
         H=0.0e0
         DO NC=II, N
            H = H + D(NC)**2
         END DO
c     
c     *** calc Y ***
c     
c     DO 75 J=1, N
c     DD=0.0e0
c     DO 70 I=1,N
c     IF (I.EQ.J) then
c     DD = DD + (1.0d0 - 2e0*D(I)*D(J)/H)*X(I,K+1)
c     ELSE
c     DD = DD - (2e0*D(J)*D(I)/H)*X(I,K+1)
c     ENDIF
c     70        CONTINUE
c     X(J,K+2) = DD
c     75      CONTINUE
c     DO 80 NC=1,N
c     X(NC,K+1) = X(NC,K+2)
c     80      CONTINUE   
c     
         DD = 0.0d0
         DO I=1,N
            DD = DD + D(I) * X(I,K+1)
         END DO
         DO J=1,N
            X(J,K+1) = -2e0 * D(J)/H * DD + X(J,K+1)
         END DO
c     
c     ***    ***     
c     
         X(II,II) = G
      END DO

      RETURN
      END   
