      subroutine LAEQ(X, NOD, NKNOT, GZAI, IOR, M1, A)
C     
C     Making the coefficient matrix of a linear algebraic equation
C     for determining the parameters of an interpolationg function
C     
      real*8 X(M1), GZAI(NOD+IOR), A(M1,M1), RN(10)
C     
c     Check the schoenberg-whitney condition
c     
      IORP1 = IOR + 1
      do I=IORP1, NOD
         IIOR = I - IOR
         if(X(IIOR).lt.GZAI(I).and.GZAI(I).lt.X(I)) CYCLE
         write(6,*)'OHSCHOENBERG-WHITNEY CONDITION IS VIOLATED'
         write(6,*)'X(',IIOR,')   =',X(IIOR)
         write(6,*)'GZAI(',I,')=',GZAI(I)
         write(6,*)'X(',I,')   =',X(I)
         stop
      end do
      do IA = 1, NOD
         do JA = 1, NOD
            A(IA, JA) = 0.0
         end do
      end do
      do K=1, NOD
         call bspl(X(K), NKNOT, GZAI, IOR, IGZ, RN) 
         IGMO = IGZ - IOR - 1
         do I=1, IOR
            JA = IGMO + I
            A(K,JA) = RN(I)
         end do
      end do
C     
      return
      end 
