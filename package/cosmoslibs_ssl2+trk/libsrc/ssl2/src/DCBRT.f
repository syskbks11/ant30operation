      REAL*8 FUNCTION DCBRT ( x )
C
C      cubic root of real x
C
      REAL*8   x, w
C
C
C
      if ( x .ne. 0 ) then
         w = dsign( abs( x )**(1.0/3.0), x ) 
         dcbrt = .5*(w+3.*x/(2.0*w*w+x/w ))
      else
         dcbrt = 0.0
      endif   
C
      return
      end
