      subroutine IFUNCT(NKNOT, GZAI, IOR, C, XX, NN, S,
     *     M1, M2, NOD, RINTEG)
C     
C     Calculation of the values of the interpolating funcion
C     
      real*8 GZAI(IOR+NOD), C(M1), RN(10), XX(M2), S(M2)
C     
      do KK=1, NN
         call bspl(XX(KK), NKNOT, GZAI, IOR, IGZ, RN)     
         IGMO = IGZ - IOR -1
         S(KK) = 0.0
         do I = 1, IOR
            IA = IGMO + I
            S(KK)=S(KK)+C(IA)*RN(I)
         end do
      end do
      RINTEG = 0.0
      do I = 1, NOD
         IIOR = I + IOR
         RINTEG = RINTEG + (GZAI(IIOR)-GZAI(I))*C(I)
      end do
      RINTEG = RINTEG/FLOAT(IOR)
      return
      end
