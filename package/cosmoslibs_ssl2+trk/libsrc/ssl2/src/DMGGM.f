      SUBROUTINE DMGGM ( a, ka, b, kb, c, kc, m, n, l, vw, icon)
C     
C     product of two matrices
C     
      INTEGER  ka, kb, kc, m, n, l, i, j, k, icon
      REAL*8   a(ka,n), b(kb,l), c(kc,l), d(kc,l), vw(n), sum
C     
C     c = 0
      d = 0
      icon = 0
C     
      if ( (m .lt. 1) .or. (n .lt. 1) .or. (l .lt. 1)
     *     .or. (ka .lt. m) .or. (kb .lt. n) .or. (kc .lt. m) ) then
         icon = 30000
         return
      endif
C     
C     
      do i=1, m
         do j=1, l
            sum = 0
            do k=1, n
               sum = sum + a(i,k)*b(k,j)
            end do
            d(i,j) = sum 
         end do
      end do
      c = d
C     
C     error procedure
C     
      return
      end
