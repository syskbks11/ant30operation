      SUBROUTINE bspl(X, NKNOT, GZAI, IOR, IGZ, RN)
C     
C     EVALUATION OF A NORMALIZED B-SPLINE RN AT X
C     
      real*8 X,GZAI(NKNOT), RM(11,10),RN(10)
c     
      IGZ = NKNOT - IOR + 1
c     
      do I = 1, NKNOT
         if (X .LT. GZAI(I)) then
            IGZ = I
            exit
         end if
      end do
c     
      IORM1 = IOR - 1
c     
      do I = 1, IOR + 1
         do J = 1, IOR
            RM(I,J) = 0.0
         end do
      end do
c     
      RM(2,1) = 1.0/(GZAI(IGZ)-GZAI(IGZ-1))
      do J=2, IORM1
         JP1 = J + 1
         do I=2, JP1
            K = IGZ + I - 2 - J
            L = IGZ + I - 2  
            RM(I,J) = ((X-GZAI(K))*RM(I-1,J-1)+(GZAI(L)-X)*RM(I,J-1))/
     *           (GZAI(L)-GZAI(K))
         end do
      end do
      IORP1 = IOR + 1
      do I=2, IORP1
         K=IGZ+I-2-IOR
         L=IGZ+I-2
         RN(I-1) = (X-GZAI(K))*RM(I-1,IORM1)+(GZAI(L)-X)*RM(I,IORM1)
      end do
      do I=1, IOR
         IF (RN(I).LT.1.0E-10) RN(I)=0.0
      end do
c     
      return
      end
