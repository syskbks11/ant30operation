C      SUBROUTINE DKBIF3(X,N,M,C,XT,ISW,V,I,F,VW,ICON)
      SUBROUTINE DBIF3(X,N,M,C,XT,ISW,V,I,F,VW,ICON)
C CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
C  (1) Purpose
C         Computes the indefinite integral
C                  the interpolated value or
C                  the isw-th derivative at a given point,
C                  based on the interpolating spline of type-3.
C             So,before bif3 is used, the interpolating spline
C             must have been obtained by the subroutine bic3.
C
C  (2) Author
C         version 1.0   by KOTA Nishiyama(*JST) 1999-10-26
C
C      *JST : Japan Science and Technology Corporation
C
C ---------------------------------------------------------------
C
C CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
c
      REAL*8   X(N),C(N),XT(N-M+1),V,F,VW(M*N+2)
      REAL*8   GZAI(20),XX(4),S(4),b(4,4),beta(4),deta
      INTEGER  N ,M, ISW, ICON, I, indx(4)
c KKK 
      ICON = 0
c KKKc
c KKK 
C      write(6,*)'*** start DKBIF3 ***'
c KKK
C
C *** CHECK PARAMETER ***
C
      IF((V.LT.X(1)).OR.(V.GT.X(N)).OR.(ISW.LT.-1).OR.(ISW.GT.M)) THEN
        ICON = 30000
        write(6,*)'ERR KBIK3 : ICON =',ICON
        STOP
      ENDIF 
C
      IF(V.EQ.X(N)) THEN
        IB = I
        I  = N-1
        IF(I.NE.IB) ICON = 10000
      ELSE
        DO 30 NC=1, N-1
          IF(V.GE.X(NC).AND.V.LT.X(NC+1)) THEN
            IB = I
            I  = NC
            IF(I.NE.IB) ICON = 10000
          ENDIF
30      CONTINUE
      ENDIF
C
C *** SET parameter ***  
C
      VW(1) = 0.    
      NOD   = N
      IOR   = M + 1
      M1    = NOD
      NKNOT = NOD + IOR
c
c *** CALC GZAI ***    
c
      DO 20 NC=1, NKNOT
        IF (NC.LE.IOR) THEN
          GZAI(NC) = XT(1)
        ELSE IF(NC.LE.NOD) THEN
          GZAI(NC) = XT(NC-M)
        ELSE IF(NC.LE.NKNOT) THEN
          GZAI(NC) = XT(N+1-M)
        ENDIF
20    CONTINUE
C
c *** CALC C ***
C
      IF(ISW.EQ.0) THEN
        XX(1) = V
        NN    = 1
        M2 = 1
        CALL IFUNCT(NKNOT, GZAI, IOR, C, XX, NN, S, M1, M2,NOD,RINTEG)
         F = S(1) 
      ELSE IF(ISW.EQ.1) THEN
        DO 40 NC=1,N-1
          IF(V.GE.X(NC).AND.V.LE.X(NC+1)) then
            XX(1) = X(NC)
            XX(2) = X(NC) + ABS(X(NC)-X(NC+1))/3.0d0
            XX(3) = X(NC) + ABS(X(NC)-X(NC+1))*2.0d0/3.0d0
            XX(4) = X(NC+1)
          ENDIF
40      CONTINUE        
        NN    = 4
        M2 = 4
        CALL IFUNCT(NKNOT, GZAI, IOR, C, XX, NN, S, M1, M2,NOD,RINTEG)
C        DO 50, I=1,4
        DO 50, K=1,4
          DO 60, J=1,4 
C            b(I,J)= XX(I)**(4-J)
            b(K,J)= XX(K)**(4-J)
60        CONTINUE
C          beta(I)=S(I)
          beta(K)=S(K)
50      CONTINUE
c
        CALL sludcmp(b,4,indx,deta)
        CALL slubksb(b,4,indx,beta)
c
        F=3.0d0*beta(1)*V**2 + 2.0d0*beta(2)*V + beta(3)
c
      ELSE
        ICON = 30000 
        write(6,*)'ERR KBIK3 :CHECK ISW: ICON =',ICON
        STOP
      ENDIF
c
c **************
c
      RETURN
      END
