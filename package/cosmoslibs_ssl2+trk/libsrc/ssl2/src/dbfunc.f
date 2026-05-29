      SUBROUTINE dbfunc ( x, afunc, ma)
c
      REAL*8   x, afunc(51)
      INTEGER  ma
C
      do 20 i=1, ma
C
           afunc(i) = x**dble(i-1)
C
 20   continue
C
      return
      end
