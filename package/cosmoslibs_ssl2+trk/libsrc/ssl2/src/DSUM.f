      SUBROUTINE DSUM ( a, b, n, ia, ib, sum)
C
C      sum of products (dot product) of two vectors 
C
      INTEGER  n, ia, ib, j, k
      REAL*8   a(ia*n), b(ib*n), sum

C
      sum = 0
C
      do 20 i=1, n
C
           j = 1 + ia*(i-1)
           k = 1 + ib*(i-1)
C
           sum = sum + a(j)*b(k)
C
 20   continue
C
      return
      end
