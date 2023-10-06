C*---------------------------------------------------------------------C
C*                                                                     C
C*  関数名   : trk_23                                                  C
C*                                                                     C
C*  和名     : オフセット処理２                                        C
C*                                                                     C
C*  機能概要 : スキャンオフセットによる観測方向の補正を行なう          C
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
      SUBROUTINE trk_23( DSCNST, DSCNED, DPOFAZ, IRSFLG, IASFLG, IRPFLG,
     +                   JRET )
C
#include      "__DFTF"
C                                       * COMMON AREA OF INSTRUCTION   *
#include      "EI_LMAI"
#include      "XSTINF"
#include      "XTMERT"
#include      "XTRK00"
#include      "XANTEN"
C                                       * WORK VARIABLE DEFINITION     *
C                                                                       
      REAL*8      DSCNST(2)             ! スキャン開始点
      REAL*8      DSCNED(2)             ! スキャン終了点
      REAL*8      DPOFAZ(2)             ! ポインティングオフセット
      INTEGER*4   IRSFLG                ! スキャン実角フラグ
      INTEGER*4   IASFLG                ! スキャン角フラグ
      INTEGER*4   IRPFLG                ! ポインティング実角フラグ
      INTEGER*2   JRET                  ! リターンコード
C
      REAL*8      DSTART(2)             ! 開始位置
      REAL*8      DROFFSET(2)           ! 実角オフセット量
      REAL*8      DIOFFSET(2)           ! 虚角オフセット量
      REAL*8      DWRK(2), DWRK1(2) ,DTMP1(2), DTMP2(2)
      REAL*8      DWF(2)
      INTEGER*4   IMSG                                                  
      INTEGER*2   JRC(2) 
      CHARACTER   CEL, CM(2)*79                                         
C
      REAL*8      DMPAE1                ! マップセンター座標
      REAL*8      DMPAE2                !
      REAL*8      DSNAE1                ! 観測位置座標
      REAL*8      DSNAE2                !
      INTEGER*2   UPTIME                ! 初回フラグ
C
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
C
C リターンコード初期化
      JRC(1) = 0
      JRC(2) = 0
C
C マップセンター座標算出処理 ( trk_ant )
C ポインティングオフセットが方位角、仰角の場合
      IF( UPTIME .EQ. 1 ) THEN
        DTMP1(1) = DAPSAE(1,1)
        DTMP1(2) = DAPSAE(2,1)
C 実角オフセット量加算
        JTSCCR = 5
        CALL R_4300( DTMP1, DPOFAZ, DTMP2, JTSCCR , JRC(1) )
        IF( JRC(1) .NE. 0 )THEN
          JRET = -1
          GOTO 9000
        ENDIF
        DMPAE1 = DTMP2(1)
        DMPAE2 = DTMP2(2)
      ENDIF
C
C 基準局と観測局についてオフセットを与える
      DO 1000 JC = 1, 2, 1
C
C オフセット量の初期化
        DROFFSET(1) = 0.0d0
        DROFFSET(2) = 0.0d0
        DIOFFSET(1) = 0.0d0
        DIOFFSET(2) = 0.0d0
C
C スキャンオフセットが方位角、仰角の場合
        IF( IASFLG .EQ. 3 )THEN
C
C 開始位置へのオフセット量計算
          IF( IRSFLG .EQ. 1 )THEN
            JTSCCR = 5
          ELSE
            JTSCCR = 0
          ENDIF
C オフセット量の加算
          DWRK1(1) = DAPSAE(1,JC)
          DWRK1(2) = DAPSAE(2,JC)
          CALL R_4300( DWRK1, DSCNST, DSTART, JTSCCR , JRC(1) )
          IF( JRC(1) .NE. 0 )THEN
            JRET = -1
            GOTO 9000
          ENDIF
C
C スキャン量計算
          DWF(1) = ( ICEPOC - 1 ) * ( DSCNED(1) - DSCNST(1) ) / ICTOTA
          DWF(2) = ( ICEPOC - 1 ) * ( DSCNED(2) - DSCNST(2) ) / ICTOTA
C 実角の場合
          IF( IRSFLG .EQ. 1 )THEN
            DROFFSET(1) = DROFFSET(1) + DWF(1)
            DROFFSET(2) = DROFFSET(2) + DWF(2)
C 虚角の場合
          ELSE
            DIOFFSET(1) = DIOFFSET(1) + DWF(1)
            DIOFFSET(2) = DIOFFSET(2) + DWF(2)
          ENDIF
        ELSE
C スキャンオフセットが方位角、仰角でない場合
          DSTART(1) = DAPSAE(1,JC)
          DSTART(2) = DAPSAE(2,JC)
        ENDIF
C
C ポインティングオフセットが方位角、仰角の場合
        IF( (DPOFAZ(1) .NE. 0.0D0) .OR. (DPOFAZ(2) .NE. 0.0D0) )THEN
C 実角の場合
          IF( IRPFLG .EQ. 1 )THEN
            DROFFSET(1) = DROFFSET(1) + DPOFAZ(1)
            DROFFSET(2) = DROFFSET(2) + DPOFAZ(2)
C 虚角の場合
          ELSE
            DIOFFSET(1) = DIOFFSET(1) + DPOFAZ(1)
            DIOFFSET(2) = DIOFFSET(2) + DPOFAZ(2)
          ENDIF
        ENDIF
C
C 虚角オフセット量加算
        JTSCCR = 0
        CALL R_4300( DSTART, DIOFFSET, DWRK, JTSCCR , JRC(1) )
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
        DAPSAE(1,JC) = DWRK1(1)
        DAPSAE(2,JC) = DWRK1(2)
C
 1000 CONTINUE
C
C スキャン座標の算出処理 ( trk_ant )
      IF( UPTIME .EQ. 1 ) THEN
        DSNAE1 = DAPSAE(1,1)
        DSNAE2 = DAPSAE(2,1)
      ENDIF
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
