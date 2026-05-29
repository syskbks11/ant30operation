      SUBROUTINE dlfit(x,y,ndat,ndeg,sig,a)
C     
C     
      INTEGER   ma, ndat, ndeg,indx(2048)
      REAL*8    a(51), covar(51,51), 
     *     sig(2048),
     *     x(2048), y(2048)
      EXTERNAl  dbfunc
      INTEGER   i,j,k,l,m,mfit
      REAL*8    sig2i,wt,ym,afunc(51),beta(51),deta
C     
C     write(6,*)'***** start lfit *****'
C     
C     *** set param1 *** 
C     
c     BAIC = 0.0
      ma = ndeg + 1
c     
c     *** count mfit ***
c     
      mfit = 0
      do j=1,ma
         mfit=mfit+1
      end do
      if (mfit.eq.0) then
         print *, 'lfit: no parameters to be fitted'
         call flush(6)
         read(*,*)
      end if
c     
c     *** initialize covar(mfit, mfit), beta(mfit) ***
c     
      do j=1, mfit
         do k=1, mfit
            covar(j,k)=0.0d0
         end do
         beta(j)=0.0d0
      end do
c     
      do i=1,ndat
         call dbfunc(x(i),afunc,ma)
         ym=y(i)
         sig2i=sig(i)
         j=0
         do l=1,ma
            j=j+1
            wt=afunc(l)*sig2i
            k=0
            do m=1,l
               k=k+1
               covar(j,k)=covar(j,k)+wt*afunc(m)
            end do
            beta(j)=beta(j)+ym*wt
         end do
      end do
      do j=2, mfit
         do k=1,j-1
            covar(k,j)=covar(j,k)
         end do
      end do
      call dludcmp(covar,mfit,indx,deta)
      call dlubksb(covar,mfit,indx,beta)
      j=0
      do l=1,ma
         j=j+1
         a(l)=beta(j)
      end do
C     
      return
      END
