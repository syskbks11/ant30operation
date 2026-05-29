      subroutine GAUSEL(N, A, X, B, M)
C
C     Solving a linear algebraic equation by gaussian elimination
C        with partial pivoting
C
      real*8 A(M,M), X(M), B(M), S, AMAX, WORK
      integer   N, M
C
      NM1 = N - 1
      do 10 K=1, NM1
        KP1 = K + 1
        AMAX = ABS(A(K,K))
        MAX = K
        do 12 I=KP1, N
          if(AMAX.ge.ABS(A(I,K))) go to 12
          AMAX = ABS(A(I,K))
          MAX = I
12      continue
        if(MAX.EQ.K) go to 13
        do 14 J=K, N
          WORK = A(K,J)
          A(K,J) = A(MAX,J)
          A(MAX,J) = WORK
14      continue
        WORK = B(K)
        B(K) = B(MAX)
        B(MAX) = WORK
13      MAX = KP1
        do 15 J=KP1, N
          if (A(K,J).eq.0.0) go to 15
          A(K,J)=A(K,J)/A(K,K)
          MAX=J
15      continue
        B(K) = B(K)/A(K,K)
        do 20 I=KP1, N
          if(A(I,K).EQ.0.0) go to 20
          do 25 J = KP1, MAX
            A(I,J)=A(I,J)-A(I,K)*A(K,J)
25        continue
          B(I) = B(I)-A(I,K)*B(K)
20      continue
10    continue
      X(N)=B(N)/A(N,N)
      do 30 L=1, NM1
        I = N-L
        IP1 = I + 1
        S = B(I)
        do 35 J=IP1, N
          S = S-A(I,J)*X(J)
35      continue   
        X(I) = S
30    continue
C
      return
      end
