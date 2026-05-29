      SUBROUTINE DLESQ1(xx,yy,ndat,ndeg,sigg,a,VW,ICON)
C CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
C  (1) Purpose
C         Obtains polynominal least-squares approximation
C  (2) Author
C         version 1.2   by KOTA Nishiyama(*JST) 2000-07-19
C                        bug fix -> initialize nd and a(*)
C                            
C         version 1.1   by KOTA Nishiyama(*JST) 2000-01-31
C                        stop -> return
C                        sig -> sigg
C         version 1.0   by KOTA Nishiyama(*JST) 1999-10-26
C
C      *JST : Japan Science and Technology Corporation
C
C ---------------------------------------------------------------
C
C CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
      INTEGER   ndat, ndeg, ICON
      REAL*8    C(51), sig(2048),
     *          x(2048), y(2048),SIGSQ,RVAL,AIC,BAIC
      REAL*8    xx(2048),yy(2048),sigg(2048),
     *          a(51),VW(7*2048)
C
C      write(6,*)'***** start KLESQ1 *****'
C
C *** CHECK param ***
C
      if ((ndat.lt.2).or.(ndeg+1.gt.ndat-1)) then
         ICON = 30000
C KKKs   stop
         return
C KKK 2000/1/31/e
      endif
      do 10 IC=1, 1, ndat
C KKKs         if(sig(IC).lt.0.0) then
         if(sigg(IC).lt.0.0) then
C KKK 2000/1/31e
           ICON = 30000
C KKKs           stop
           return
C KKK 2000/1/31e
         endif
10    continue
C
C *** set param *** 
C
      VW(1) = 0.0
      ICON  = 0.0
      BAIC  = 0.0
      IP   = 0
C
C KKK 2000/07/19S
      nd   = 0
      DO 220, I=1, ndeg + 1
          a(I)=0
220   CONTINUE
C KKK 2000/07/19E
C
      IF(ndeg.lt.0) THEN
             IP   = ABS(ndeg)
             ndeg = ABS(ndeg)
      ENDIF
      do 5 IC =1, ndat
         x(IC)=xx(IC)
         y(IC)=yy(IC)
         sig(IC)=sigg(IC)
5     continue
c 
C *******************************
c
      DO 200 NC = IP, ndeg
C
       call lfit(x,y,ndat,NC,sig,C)
C
C KKK 2000/07/19S
       IF (NC.EQ.0) a(1)=C(1)
C KKK 2000/07/19E
c
C *** CALCULATE AIC ***
C
       AIC = 0.0
       SIGSQ=0.0
       DO 100 i=1, ndat
         RVAL = 0.0
         DO 110 j=1,NC+1
           RVAL = RVAL + C(j)*(x(i)**DBLE(j-1))
110      CONTINUE
         SIGSQ = SIGSQ + sig(i)*((y(i)-RVAL)*(y(i)-RVAL))
100    CONTINUE  
       AIC = ndat * log10(SIGSQ) + 2.*NC
c       write(6,*) NC,' AIC =',AIC
C
C *** CHECK AIC ***
C
       IF ((AIC.LT.BAIC).OR.(NC.EQ.IP)) THEN
         BAIC = AIC
         DO 210 I=1, NC+1
            a(I)=C(I)
210      CONTINUE
         nd = NC
       ENDIF
C
200   CONTINUE
C
C *******************************
C
      ndeg = nd
C
      return
      END
