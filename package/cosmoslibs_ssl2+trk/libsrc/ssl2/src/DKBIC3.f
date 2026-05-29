C     SUBROUTINE DKBIC3(X,Y,N,M,C,XT,VW,ICON)
      SUBROUTINE DBIC3(X,Y,N,M,C,XT,VW,ICON)
C     CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C     
C     (1) Purpose
C     Computes the coefficients of Type-3 interpolating 
C     splines expanded by normalized b-splines
C     (2) Author
C     version 1.0   by KOTA Nishiyama(*JST) 1999-10-26
C     
C     *JST : Japan Science and Technology Corporation
C     
C     ---------------------------------------------------------------
C     
C     CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
c     
      REAL*8   X(N),Y(N),C(N),XT(N-M+1),VW(M*N+2), GZAI(20)
      REAL*8   A(10,10), YY(10)
      INTEGER  N ,M, ICON
      INTEGER  M1
C     KKK
      ICON = 0
C     KKK
C     
C     KKK
C     write(6,*)'*** start DKBIC3 ***'
C     KKK    
C     
C     *** CHECK PARAMETER ***
C     
      IF((M.NE.INT(2*(M/2.))).OR.(N.LT.M+2).OR.(M.LT.3)) THEN 
         ICON = 30000
         write(6,*)'ERR ICON=',ICON
         STOP
      ENDIF
      DO NC = 1, N-1
         IF(X(NC).GE.X(NC+1)) THEN
            ICON = 30000
            write(6,*)'ERR ICON=',ICON
            STOP
         ENDIF 
      END DO
C     
C     *** SET parameter ***  
C     
      VW(1) = 0.    
      NOD   = N
      IOR   = M + 1
      M1    = NOD
      NKNOT = NOD + IOR
      DO NC=1, N
         YY(NC)=Y(NC)
      END DO
c     
c     *** CALC XT ***
c     
      DO NC=1, N
         IF(NC.EQ.1) THEN
            XT(NC)=X(NC)
         ELSE IF(NC.GE.2.AND.NC.LE.N-M) THEN
            XT(NC)=X(NC+INT((M-1)/2))
         ELSE IF(NC.EQ.N) THEN
            XT(N-M+1)=X(N)
         ENDIF
      END DO
c     
c     *** CALC GZAI ***    
c     
      DO NC=1, NKNOT
         IF (NC.LE.IOR) THEN
            GZAI(NC) = XT(1)
         ELSE IF(NC.LE.NOD) THEN
            GZAI(NC) = XT(NC-M)
         ELSE IF(NC.LE.NKNOT) THEN
            GZAI(NC) = XT(N+1-M)
         ENDIF
      END DO
C     
c     *** CALC C ***
C     
      CALL LAEQ(X, NOD, NKNOT, GZAI, IOR, M1, A)
      CALL GAUSEL(NOD, A, C, YY, M1)
c     
c     **************
c     
      RETURN
      END
