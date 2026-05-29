      SUBROUTINE ludcmp(a,n,indx,d)
C     
      INTEGER   n,indx(2048)
      REAL*8    d,a(51,51),TINY
      PARAMETER (TINY=1.0e-20)
      INTEGER   i,imax,j,k
      REAL*8    aamax,dum,sum,vv(51)
C     
      d=1.
      do i=1,n
         aamax=0.
         do j=1,n
            if(abs(a(i,j)).gt.aamax) aamax=abs(a(i,j))
         end do
         if (aamax.eq.0) then
            print *, 'singular matrix '
            call flush(6)
            read(*,*)
         end if
         vv(i)=1./aamax
      end do
      do j=1,n
         do i=1,j-1
            sum=a(i,j)
            do k=1,i-1
               sum=sum-a(i,k)*a(k,j)
            end do
            a(i,j)=sum
         end do
         aamax=0.
         do i=j,n
            sum=a(i,j)
            do k=1,j-1
               sum=sum-a(i,k)*a(k,j)
            end do
            a(i,j)=sum
            dum=vv(i)*abs(sum)
            if (dum.ge.aamax) then
               imax=i
               aamax=dum
            endif
         end do
         if (j.ne.imax) then
            do k=1,n
               dum=a(imax,k)
               a(imax,k)=a(j,k)
               a(j,k)=dum
            end do
            d=-d
            vv(imax)=vv(j)
         endif
         indx(j)=imax
         if(a(j,j).eq.0.) a(j,j)=TINY
         if(j.ne.n) then
            dum=1./a(j,j)
            do i=j+1,n
               a(i,j)=a(i,j)*dum
            end do
         endif
      end do
      return
      END
