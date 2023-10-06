C*---------------------------------------------------------------------C
C*                                                                     C
C*  関数名   : trk_22rd                                                C
C*                                                                     C
C*  和名     : オフセット処理１                                        C
C*                                                                     C
C*  機能概要 : スキャンオフセット、ポインティングオフセットによる観測方C
C*             向の補正を行なう                                        C
C*                                                                     C
C*  戻り値   : =   0 : 正常終了                                        C
C*                                                                     C
C*  注意事項 : なし                                                    C
C*                                                                     C
C*  作成者   : FUJITSU LIMITED 1996.07.03                              C
C*                                                                     C
C*  メモ     : EI_340からの改造                                        C
C*                                                                     C
C*---------------------------------------------------------------------C
      SUBROUTINE trk_22rd( DSCNST, DSCNED, DPOFST, IRSFLG, IASFLG,
     +                     IRPFLG, IAPFLG, JRET )
C
#include      "__DFTF"
C                                       * COMMON AREA OF INSTRUCTION   *
#include      "EI_LMAI"                                               
#include      "XTRK00"
#include      "XTMERT"
#include      "XSTINF"
#include      "XANTEN"
C                                       * WORK VARIABLE DEFINITION     *
C                                                                       
      REAL*8      DSCNST(2)             ! スキャン開始点
      REAL*8      DSCNED(2)             ! スキャン終了点
      REAL*8      DPOFST(2)             ! ポインティングオフセット
      INTEGER*4   IRSFLG                ! スキャン実角フラグ
      INTEGER*4   IASFLG                ! スキャン角フラグ
      INTEGER*4   IRPFLG                ! ポインティング実角フラグ
      INTEGER*4   IAPFLG                ! ポインティング角フラグ
      INTEGER*2   JRET                  ! リターンコード
C
      REAL*8      DROFFSET(2)           ! 実角オフセット量
      REAL*8      DIOFFSET(2)           ! 虚角オフセット量
      CHARACTER*6 CESCFE(2)             ! 分点名
      REAL*8      DWRK(2), DWRK1(2)
      REAL*8      DTMP(2), DTMP1(2), DTMP2(2)
      REAL*8      DWF(2)
      INTEGER*4   IMSG                                                  
      INTEGER*2   JRC(2)
      CHARACTER   CEL, CM(2)*79                                         
C
      REAL*8      DMPRD1                ! マップセンター座標
      REAL*8      DMPRD2                !
      REAL*8      DMPLB1                ! 
      REAL*8      DMPLB2                !
      REAL*8      DSNRD1                ! 観測位置座標
      REAL*8      DSNRD2                !
      REAL*8      DSNLB1                ! 
      REAL*8      DSNLB2                !
      INTEGER*2   UPTIME                ! 初回フラグ
C                                       * CONSTANT DATA DEFINITION     *
      DATA  IMSG / 0 /                                          
      DATA  CEL / 'W' /                                                 
      DATA  CM(1)( 1:40) / 'XEI307W EI_340  IRREGULAR OCCURS IN SCAN' / 
      DATA  CM(1)(41:79) / ' OPERATION. IT''S PURGED.' /                
      DATA  CM(2)( 1:40) / 'XEI308W EI_340  TOO LARGE SCAN OFFSET. S' / 
      DATA  CM(2)(41:79) / 'CAN OPERATION CONTINUED.' /                 
C                                                                       
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C リターンコード初期化
      JRC(1) = 0
      JRC(2) = 0
C
C オフセット量の初期化
      DROFFSET(1) = 0.0d0
      DROFFSET(2) = 0.0d0
      DIOFFSET(1) = 0.0d0
      DIOFFSET(2) = 0.0d0
C
C 天体の位置
      DEPWRK(1) = DXOBPS(1)
      DEPWRK(2) = DXOBPS(2)
C
C オフセット量が銀河座標、赤道座標で与えられている場合
C
C 分点名の初期化
      CESCFE(1) = 'B1950'
      CESCFE(2) = 'J2000'
C
      IF( UPTIME .EQ. 1 ) THEN
        DMPRD1 = DEPWRK(1)
        DMPRD2 = DEPWRK(2)
        DSNRD1 = DEPWRK(1)
        DSNRD2 = DEPWRK(2)
C
        CALL R_4100( DEPWRK, DTMP, CESCFE(ITRKBJ) )
        DMPLB1 = DTMP(1)
        DMPLB2 = DTMP(2)
        DSNLB1 = DTMP(1)
        DSNLB2 = DTMP(2)
      ENDIF
C
C スキャン量計算
      DWF(1) = ( DSCNED(1) - DSCNST(1) ) * ( ICEPOC - 1 ) / ICTOTA
      DWF(2) = ( DSCNED(2) - DSCNST(2) ) * ( ICEPOC - 1 ) / ICTOTA
C
C 観測天体が惑星または天体位置が赤道座標で指定されている場合
      IF( ( ITRKAX .EQ. 0 ) .OR. ( ITRKAX .EQ. 1 ) ) THEN
C
C マップセンター座標の算出処理 (trk_ant )
C ポインティングオフセットが赤道座標の場合
        IF( ( IAPFLG .EQ. 1 ) .AND. ( UPTIME .EQ. 1 ) ) THEN
          DTMP1(1) = DEPWRK(1)
          DTMP1(2) = DEPWRK(2)
          JTSCCR = 5
          CALL R_4300( DTMP1, DPOFST, DTMP2, JTSCCR , JRC(1) )
          IF( JRC(1) .NE. 0 ) THEN
            JRET = -1
            GOTO 9000
          ENDIF
C
          DMPRD1 = DTMP2(1)
          DMPRD2 = DTMP2(2)
          CALL R_4100( DTMP2, DTMP1, CESCFE(ITRKBJ) )
          DMPLB1 = DTMP1(1)
          DMPLB2 = DTMP1(2)
        ENDIF
C
C ポインティングオフセットが銀河座標の場合
        IF( ( IAPFLG .EQ. 2 ) .AND. ( UPTIME .EQ. 1 ) ) THEN
          DTMP1(1) = DMPLB1
          DTMP1(2) = DMPLB2
          JTSCCR = 5
          CALL R_4300( DTMP1, DPOFST, DTMP2, JTSCCR , JRC(1) )
          IF( JRC(1) .NE. 0 ) THEN
            JRET = -1
            GOTO 9000
          ENDIF
C
          DMPLB1 = DTMP2(1)
          DMPLB2 = DTMP2(2)
          CALL R_4200( DTMP2, DTMP1, CESCFE(ITRKBJ) )
          DMPRD1 = DTMP1(1)
          DMPRD2 = DTMP1(2)
        ENDIF
C
C スキャンオフセットが赤道座標の場合
        IF( IASFLG .EQ. 1 )THEN
C 実角の場合
          IF( IRSFLG .EQ. 1 )THEN
            JTSCCR = 5
            DROFFSET(1) = DROFFSET(1) + DWF(1)
            DROFFSET(2) = DROFFSET(2) + DWF(2)
C 虚角の場合
          ELSE
            JTSCCR = 0
            DIOFFSET(1) = DIOFFSET(1) + DWF(1)
            DIOFFSET(2) = DIOFFSET(2) + DWF(2)
          ENDIF
C 開始位置
          CALL R_4300( DXOBPS, DSCNST, DEPWRK, JTSCCR , JRC(1) )
          IF( JRC(1) .NE. 0 )THEN
            JRET = -1
            GOTO 9000
          ENDIF
        ENDIF
C
C ポインティングオフセットが赤道座標の場合
        IF( IAPFLG .EQ. 1 )THEN
C 実角の場合
          IF( IRPFLG .EQ. 1 )THEN
            DROFFSET(1) = DROFFSET(1) + DPOFST(1)
            DROFFSET(2) = DROFFSET(2) + DPOFST(2)
C 虚角の場合
          ELSE
            DIOFFSET(1) = DIOFFSET(1) + DPOFST(1)
            DIOFFSET(2) = DIOFFSET(2) + DPOFST(2)
          ENDIF
        ENDIF
C
C 虚角オフセット量加算
        JTSCCR = 0
        CALL R_4300( DEPWRK, DIOFFSET, DWRK, JTSCCR , JRC(1) )
        IF( JRC(1) .NE. 0 )THEN
          JRET = -1
          GOTO 9000
        ENDIF
C
C 実角オフセット量加算
        JTSCCR = 5
        CALL R_4300( DWRK, DROFFSET, DWRK1, JTSCCR , JRC(1) )
        IF( JRC(1) .NE. 0 )THEN
          JRET = -1
          GOTO 9000
        ENDIF
C
C 赤道座標から銀河座標へ変換する
        CALL R_4100( DWRK1, DEPWRK, CESCFE(ITRKBJ) )
      ENDIF
C
C 赤道座標から銀河座標へ変換された場合、または銀河座標で天体の位置を 
C 指定された場合
      IF( ( ITRKAX .EQ. 0 ) .OR.
     +    ( ITRKAX .EQ. 1 ) .OR. ( ITRKAX .EQ. 2 ) ) THEN
C
        DROFFSET(1) = 0.0d0
        DROFFSET(2) = 0.0d0
        DIOFFSET(1) = 0.0d0
        DIOFFSET(2) = 0.0d0
C
C スキャンオフセットが銀河座標の場合
        IF( IASFLG .EQ. 2 )THEN
C 実角の場合
          IF( IRSFLG .EQ. 1 )THEN
            JTSCCR = 5
            DROFFSET(1) = DROFFSET(1) + DWF(1)
            DROFFSET(2) = DROFFSET(2) + DWF(2)
C 虚角の場合
          ELSE
            JTSCCR = 0
            DIOFFSET(1) = DIOFFSET(1) + DWF(1)
            DIOFFSET(2) = DIOFFSET(2) + DWF(2)
          ENDIF
C 開始位置
          CALL R_4300( DEPWRK, DSCNST, DWRK1, JTSCCR , JRC(1) )
          IF( JRC(1) .NE. 0 )THEN
            JRET = -1
            GOTO 9000
          ENDIF
          DEPWRK(1) = DWRK1(1)
          DEPWRK(2) = DWRK1(2)
        ENDIF
C
C ポインティングオフセットが銀河座標の場合
        IF( IAPFLG .EQ. 2 )THEN
C 実角の場合
          IF( IRPFLG .EQ. 1 )THEN
            DROFFSET(1) = DROFFSET(1) + DPOFST(1)
            DROFFSET(2) = DROFFSET(2) + DPOFST(2)
C 虚角の場合
          ELSE
            DIOFFSET(1) = DIOFFSET(1) + DPOFST(1)
            DIOFFSET(2) = DIOFFSET(2) + DPOFST(2)
          ENDIF
        ENDIF
C
C 虚角オフセット量加算
        JTSCCR = 0
        CALL R_4300( DEPWRK, DIOFFSET, DWRK, JTSCCR , JRC(1) )
        IF( JRC(1) .NE. 0 )THEN
          JRET = -2
          GOTO 9000
        ENDIF
C
C 実角オフセット量加算
        JTSCCR = 5
        CALL R_4300( DWRK, DROFFSET, DWRK1, JTSCCR , JRC(1) )
        IF( JRC(1) .NE. 0 )THEN
          JRET = -2
          GOTO 9000
        ENDIF
C
C スキャン座標の算出処理 ( trk_ant )
        IF( UPTIME .EQ. 1 ) THEN
          DSNLB1 = DWRK1(1)
          DSNLB2 = DWRK1(2)
          CALL R_4200( DWRK1, DEPWRK, CESCFE(ITRKBJ) )
          DSNRD1 = DEPWRK(1)
          DSNRD2 = DEPWRK(2)
        ELSE
          CALL R_4200( DWRK1, DEPWRK, CESCFE(ITRKBJ) )
        ENDIF
C
C 分点をＪ２０００へ変換する
        IF( ITRKBJ .EQ. 1 )THEN
          CALL R_4600( DEPWRK, DWRK )
          DEPWRK(1) = DWRK(1)
          DEPWRK(2) = DWRK(2)
        ENDIF

      ENDIF
C                                                                       
      DXOBPS(1) = DEPWRK(1)
      DXOBPS(2) = DEPWRK(2)
C
C----------------------------------------------------------------------C
C ERROR ENTRY                                                          C
C----------------------------------------------------------------------C
C
 9000 CONTINUE
      IF ( IMSG .EQ. 1 ) THEN
        IF( ( JRC(1) .EQ. 4 ) .OR. ( JRC(2) .EQ. 4 ) ) THEN
          IMSG = 0
          CALL O_MSG2( IMID00, CEL, CM(1), IRC )
        ELSE IF( ( JRC(1) .EQ. 1 ) .OR. ( JRC(2) .EQ. 1 ) ) THEN
          IMSG = 0
          CALL O_MSG2( IMID00, CEL, CM(2), IRC )
        ENDIF
      ENDIF
C
C----------------------------------------------------------------------C
C EXIT                                                                 C
C----------------------------------------------------------------------C
      RETURN
      END
