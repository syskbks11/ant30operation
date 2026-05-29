      SUBROUTINE DMAV ( a, k, m, n, x, y, icon)
C     
C     product of a matrix and a vector 
C     
      INTEGER  k, m, n, icon, i, j
      REAL*8   a(k,n), x(n), y(m), z(m), sum
C     
C     
      z = 0
      icon = 0
C     
      if ( (m .lt. 1) .or. (n .eq. 0) .or. (k .lt. m) ) then
         icon = 30000
         return
      endif
C     

      do i=1, m
         sum = 0
         do j=1, n
            sum = sum + a(i,j)*x(j)
         end do
         z(i) = sum 
      end do
      y = z
C     
C     error procedure
C     
      return
      end
