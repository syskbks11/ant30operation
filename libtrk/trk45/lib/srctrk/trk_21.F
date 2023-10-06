C*--------------------------------------------------------------------*/
C*                                                                    */
C*  関数名   : trk_21                                                 */
C*                                                                    */
C*  和名     : ＦＯＲＴＲＡＮの共通領域設定                           */
C*                                                                    */
C*  機能概要 : ＦＯＲＴＲＡＮの共通領域へ値を設定する。               */
C*                                                                    */
C*  戻り値   : =   0: 正常終了                                        */
C*                                                                    */
C*  注意事項 : なし                                                   */
C*                                                                    */
C*  作成者   : FUJITSU LIMITED 1996.06.10                             */
C*                                                                    */
C*  メモ     :                                                        */
C*             MODIFY 1997.04.03 ( FLUKE_FREQ  FNS )                  */
C*             MODIFY 1997.09.30 ( ATMOSPHERIC FNS )                  */
C*                                                                    */
C*--------------------------------------------------------------------*/
      SUBROUTINE trk_21( DMJD,   DPOFST, DSCNST, DSCNED,
     +                   DOBSFQ, DTRNFQ, DINTRL, DDOFST, DFOFST,
     +                   DLCLFQ, ITNSCN, INOSCN, IRPFLG, IAPFLG,
     +                   IRSFLG, IASFLG, IDSBFG, IDLFLG, IULSEL,
     +                   IROTDR, INLOCL, INFLCK, ITRKFQ, DAZEL,
     +                   DTAUG,  DPHAS,  DDPHAS, DFLKFQ, DUVW,
     +                   DHOURA, IRET,   IVREF,  IVDEF,  DPOFAZ,
     +                   FRSTFG, DCOMET, DCMSRC, DCMDST, DCMDLT )
C 宣言
#include	"__DFTF"
C コモン領域
#include        "EI_LATR"
#include        "EI_TTME"
#include        "XDFWRK"
#include        "XSTINF"
#include        "XTMERT"
#include	"XTRK00"
C for test
#include        "XEPHEM"
#include        "XIERST"
#include        "XMATHC"
#include        "XOBSEI"
#include        "XPRCNT"
#include        "XVWORK"
#include        "XDUMMY"
#include        "XOTHER"
#include        "XANTEN"
#include        "XCOMET"
C
      common /dopvel/ dopant
C 引数
      REAL*8            DMJD            ! 計算する時刻(MJD[sec])
      REAL*8            DPOFST(2)       ! ポインティング [RADEC,LB]
      REAL*8            DSCNST(2)       ! スキャン開始位置
      REAL*8            DSCNED(2)       ! スキャン終了位置
      REAL*8            DOBSFQ          ! 観測周波数
      REAL*8            DTRNFQ(2)       ! 周波数変換係数
      REAL*8            DINTRL          ! フリンジローテータ制御間隔
      REAL*8            DDOFST          ! 遅延オフセット
      REAL*8            DFOFST          ! 周波数オフセット
      REAL*8            DLCLFQ          ! 第一ローカル周波数
      INTEGER*4         ITNSCN          ! スキャン点数
      INTEGER*4         INOSCN          ! スキャン点
      INTEGER*4         IRPFLG          ! ポインティング実角フラグ
      INTEGER*4         IAPFLG          ! ポインティング角フラグ
      INTEGER*4         IRSFLG          ! スキャン実角フラグ
      INTEGER*4         IASFLG          ! スキャン角フラグ
      INTEGER*4         IDSBFG          ! ＤＳＢフラグ
      INTEGER*4         IDLFLG          ! 遅延追尾フラグ
      INTEGER*4         IULSEL          ! Ｕ／Ｌフラグ
      INTEGER*4         IROTDR          ! フリンジ回転方向
      INTEGER*4         INLOCL          ! 第一ローカル発信器逓倍次数
      INTEGER*4         INFLCK          ! フルークシンセサイザ逓倍次数
      INTEGER*4         ITRKFQ          ! トラッキング周波数種別
      REAL*8            DAZEL(2)        ! アンテナ指令値
      REAL*8            DTAUG(3)        ! 遅延時間
      REAL*8            DPHAS(2)        ! 初期位相
      REAL*8            DDPHAS(2)       ! 位相変化量
      REAL*8            DFLKFQ          ! 受信周波数
      REAL*8            DUVW(3)         ! ＵＶＷ
      REAL*8            DHOURA          ! アワーアングル
      REAL*8            DELAST          ! 地方恒星時
      INTEGER*4         IRET            ! リターンコード
      INTEGER*2         JRET            ! リターンコード
      INTEGER*2         JERCOD(2)       ! リターンコード
      INTEGER*4         IWRK
      INTEGER*4         IVREF           ! 視線速度の基準系フラグ
      INTEGER*4         IVDEF           ! 視線速度の定義系フラグ
      REAL*8            DPOFAZ(2)       ! ポインティング [AZEL]
      INTEGER*4         FRSTFG          ! 初回フラグ
      REAL*8            DCOMET(6)       ! 旧彗星）彗星軌道６要素
      REAL*8            DCMSRC(2)       ! 新彗星）天体座標 (RADEC)
      REAL*8            DCMDST          ! 新彗星）地心からの天体の距離
      REAL*8            DCMDLT(3)       ! 新彗星）デカルト成分での変化量
C
      REAL*8            DILOCL          ! 地方恒星時
      REAL*8            DPINT1, DPINT2  ! 観測者の経緯度
      REAL*8            DMPRD1, DMPRD2  ! マップセンター座標
      REAL*8            DMPLB1, DMPLB2  ! マップセンター座標
      REAL*8            DMPAE1, DMPAE2  ! マップセンター座標
      REAL*8            DSNRD1, DSNRD2  ! 観測位置座標
      REAL*8            DSNLB1, DSNLB2  ! 観測位置座標
      REAL*8            DSNAE1, DSNAE2  ! 観測位置座標
      REAL*8            DXTCRA, DXTCDE  ! 測心視位置[RADEC]
C
      REAL*8            DIPPLF          ! 受信周波数
      REAL*8            dopant          ! アンテナから見た視線速度
      INTEGER*2         UPTIME
C
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
      JRET = 0
      JERCOD(1) = 0
      JERCOD(2) = 0
      IWRK = INFLCK
      IWRK = INLOCL
      UPTIME = FRSTFG
C
      IF( UPTIME .EQ. 1 ) THEN
        DPINT1  = 0.0D0
        DPINT2  = 0.0D0
        DMPRD1  = 0.0D0
        DMPRD2  = 0.0D0
        DMPLB1  = 0.0D0
        DMPLB2  = 0.0D0
        DMPAE1  = 0.0D0
        DMPAE2  = 0.0D0
        DSNRD1  = 0.0D0
        DSNRD2  = 0.0D0
        DSNLB1  = 0.0D0
        DSNLB2  = 0.0D0
        DSNAE1  = 0.0D0
        DSNAE2  = 0.0D0
        DXTCRA  = 0.0D0
        DXTCDE  = 0.0D0
        DILOCL  = 0.0D0
        DIPPLF  = 0.0D0
        dopant  = 0.0D0
      ENDIF
C
      DCCSRC(1) = DCMSRC(1)
      DCCSRC(2) = DCMSRC(2)
      DCCDST    = DCMDST
      DCCDLT(1) = DCMDLT(1)
      DCCDLT(2) = DCMDLT(2)
      DCCDLT(3) = DCMDLT(3)
C
C trk_00 フラグチェック
      IF ( ITRF00 .NE. 1 ) THEN
        IRET = -1
        GOTO 9000
      ENDIF
C
C trk_10 フラグチェック
      IF ( ITRF10 .NE. 1 ) THEN
        IRET = -1
        GOTO 9000
      ENDIF
C
C フリンジトラッキング処理の場合
      IF ( ITRF20 .EQ. 2 ) THEN
C サイドバンド選択フラグ
        IF ( ( IULSEL .LT. 1 ) .OR. ( IULSEL .GT. 2 ) ) THEN
          IRET = -2
          GOTO 9000
        ENDIF
C フリンジ回転方向フラグ
        IF ( ( IROTDR .LT. 0 ) .OR. ( IROTDR .GT. 3 ) ) THEN
          IRET = -2
          GOTO 9000
        ENDIF
      ENDIF
C 出力変数の初期化
      DAZEL(1) = 0.0D0
      DAZEL(2) = 0.0D0
      DTAUG(1) = 0.0D0
      DTAUG(2) = 0.0D0
      DTAUG(3) = 0.0D0
      DPHAS(1) = 0.0D0
      DPHAS(2) = 0.0D0
      DDPHAS(1) = 0.0D0
      DDPHAS(2) = 0.0D0
      DFLKFQ = 0.0D0
      DUVW(1) = 0.0D0
      DUVW(2) = 0.0D0
      DUVW(3) = 0.0D0
      DHOURA = 0.0D0
C
C 時刻の補正
#ifdef DEBUG
      write(*,*)'trk_21:MJDs         (DMJD)=',DMJD
#endif
C 遅延時間、位相計算の場合、計算時刻をずらす
      IF( ( ITRF20 .EQ. 1 ) .OR. ( ITRF20 .EQ. 2 ) )THEN
        DMJD = DMJD - DINTRL
      ENDIF
#ifdef DEBUG
      write(*,*)'trk_21:MJDe         (DMJD)=',DMJD
#endif
C
C 時系の変換
      CALL EI_320( DMJD )
#ifdef DEBUG
      write(*,*)'trk_21:UTC         (DTMUJD)=',DTMUJD
      write(*,*)'trk_21:TAI-UTC     (ILEAPT)=',ILEAPT
      write(*,*)'trk_21:UT1-TAI     (DCORPR)=',DCORPR
      write(*,*)'trk_21:J century   (DCENTR)=',DCENTR
      write(*,*)'trk_21:nutation tlo(DNTLON)=',DNTLON
      write(*,*)'trk_21:nutation Mla(DMNOBL)=',DMNOBL
      write(*,*)'trk_21:nutation tla(DNTOBL)=',DNTOBL
#endif
C
C スキャンの設定
      ICTOTA = ITNSCN                  ! スキャン総点数
      ICEPOC = INOSCN                  ! スキャン点数
C
C 天体の位置
      IF( JTATMD .EQ. 1 ) THEN
C
C スキャン量、ポインティングオフセット量が赤道座標、銀河座標で指定され
C た場合
#ifdef DEBUG
      write(*,*)'trk_21:input star p(DEPSFE)=',DEPSFE
#endif
C
C 惑星の場合光行差まで補正する
C 太陽系外天体の場合Ｊ２０００年元期に於けるαδを算出
C その他の場合何もしない
        CALL EI_330( JERCOD, DCOMET )
        IF( JERCOD(1) .NE. 0 ) THEN
          IRET = -3
          GOTO 9000
        ENDIF
        IF ( DCCDST .NE. -1.0D0 ) THEN
          DXOBPS(1) = DCCSRC(1)
          DXOBPS(2) = DCCSRC(2)
          CALL R_4500( DXOBPS, DPLUNT )
        ENDIF
#ifdef DEBUG
      write(*,*)'trk_21:observing dr(DXOBPS)=',DXOBPS
#endif
C
        IF( ( ITRKAX .EQ. 0 ) .OR. ( ITRKAX .EQ. 1 ) ) THEN
          CALL trk_22rd( DSCNST, DSCNED, DPOFST, IRSFLG, IASFLG, IRPFLG,
     +                   IAPFLG, JRET )
        ELSE
          CALL trk_22lb( DSCNST, DSCNED, DPOFST, IRSFLG, IASFLG, IRPFLG,
     +                   IAPFLG, JRET )
        ENDIF
        IF( JRET .NE. 0 ) THEN
          IRET = -2
          GOTO 9000
        ENDIF
#ifdef DEBUG
      write(*,*)'trk_21:offset prt1 (DEPWRK)=',DEPWRK
#endif
C
C 地心視位置を求める
        CALL EI_350( JESTCR, JRET )
        IF( JRET .NE. 0 ) THEN
          IRET = -4
          GOTO 9000
        ENDIF
#ifdef DEBUG
      write(*,*)'trk_21:Earth veract(DOPBVL)=',DOPBVL
      write(*,*)'trk_21:observing dr(DXSTPS)=',DXSTPS
      write(*,*)'trk_21:after prop m(DPRPMT)=',DPRPMT
      write(*,*)'trk_21:after grvt t(DLTDFL)=',DLTDFL
      write(*,*)'trk_21:after lorz t(DANABR)=',DANABR
      write(*,*)'trk_21:after nut pt(DPRNUT)=',DPRNUT
#endif
C
C 測心視位置を求める
        CALL EI_360( DELAST, JESTCR, JERCOD )
#ifdef DEBUG
      write(*,*)'trk_21:phase center(DXTCPS)=',
     +            DXTCPS(1,1),DXTCPS(2,1),DXTCPS(3,1)
      write(*,*)'trk_21:phase antenn(DXTCPS)=',
     +          DXTCPS(1,2),DXTCPS(2,2),DXTCPS(3,2)
      write(*,*)'trk_21:local sidere(DXLAST)=',DXLAST
#endif
        IF( JERCOD(1) .NE. 0 ) THEN
          IRET = -5
          GOTO 9000
        ENDIF
C
C 直接、方位角と仰角を指定された場合
      ELSE
        DAPSAE(1,1) = DEPSFE(1) 
        DAPSAE(2,1) = DEPSFE(2) 
        DAPSAE(1,2) = DEPSFE(1) 
        DAPSAE(2,2) = DEPSFE(2) 
      ENDIF
C
#ifdef DEBUG
      write(*,*)'trk_21:ANTENNA rpos(DXTCPS)=',DXTCPS(1,1),
     +                             DXTCPS(2,1),DXTCPS(3,1)
      write(*,*)'trk_21:ANTENNA opos(DXTCPS)=',DXTCPS(1,2),
     +                             DXTCPS(2,2),DXTCPS(3,2)
      write(*,*)'trk_21:ANTENNA AZEL(DAPSAE)=',DAPSAE(1,1),DAPSAE(2,1)
      write(*,*)'trk_21:ANTENNA AZEL(DAPSAE)=',DAPSAE(1,2),DAPSAE(2,2)
      write(*,*)'trk_21:Antenna LOLA(DGDINF)=',DGDINF(1,1),DGDINF(2,1)
      write(*,*)'trk_21:Antenna LOLA(DGDINF)=',DGDINF(1,2),DGDINF(2,2)
      write(*,*)'trk_21:GAST        (DXGAST)=',DXGAST
      write(*,*)'trk_21:long period (DNPLXY)=',DNPLXY
#endif
C
C 一回目の位相中心アワーアングルをテンポラリ領域へコピー
      IF ( ( ( ITRF20 .EQ. 1 ) .OR. ( ITRF20 .EQ. 2 ) ) .AND.
     +     ( IRET .EQ. 0 ) ) THEN
        DTMPHA = DICTRD(3)
        IRET = 200
#ifdef DEBUG
      write(*,*)'trk_21:TMP IRET    (DTMPHA)=',IRET,DTMPHA
#endif
        GOTO 9000
      ENDIF
C
C 方位角と仰角でオフセットを与えられた場合のスキャン位置補正
      CALL trk_23( DSCNST, DSCNED, DPOFAZ, IRSFLG, IASFLG, IRPFLG,
     +             JRET )
      IF( JRET .NE. 0 ) THEN
        IRET = -6
        GOTO 9000
      ENDIF
C
C 動作種別毎に計算をする
CC
CC アンテナ制御
CC
      IF ( ITRF20 .EQ. 0 ) THEN
C
C 測心視位置（赤経赤緯）算出
        CALL EI_371( JRET )
        IF( JRET .NE. 0 ) THEN
          IRET = -11
          GOTO 9000
        ENDIF
C
C 器差補正
C       CALL EI_375( JESTCR )
        CALL EI_375()
#ifdef DEBUG
      write(*,*)'trk_21:end of AZEL (DISTCR)=',DISTCR(1,1),DISTCR(2,1)
#endif
C
C 計算結果の複写
        DAZEL(1) = DISTCR(1,1)
        DAZEL(2) = DISTCR(2,1)
C
        IRET = 100
CC
CC フルークの受信周波数、遅延、位相トラッキングの場合
CC
      ELSEIF( ( ITRF20 .EQ. 1 ) .OR.
     +        ( ITRF20 .EQ. 2 ) .OR.
     +        ( ITRF20 .EQ. 3 ) ) THEN
C 受信周波数算出
        CALL TRK_24( DOBSFQ, IVREF, IVDEF )
        IF( ITRKFQ .EQ. 0 )THEN
          DIPPLF = 0.0D0
        ELSE
          DIPPLF = DTFRCR
        ENDIF
C
#ifdef DEBUG
      write(*,*)'trk_21:obs veracity(DVELCR)=',DVELCR
      write(*,*)'trk_21:ant veracity(DSTRVL)=',DSTRVL
      write(*,*)'trk_21:input freque(DOBSFQ)=',DOBSFQ
      write(*,*)'trk_21:output frequ(DTFRCR)=',DTFRCR
#endif
C
C フルーク周波数
        IF( ITRF20 .EQ. 3 )THEN
          DFLKFQ = DTFRCR
          IRET = 1
          GOTO 9000
        ELSE
C         静止周波数でトラッキングする場合
          IF( ITRKFQ .EQ. 0 )THEN
            DTFRCR = DOBSFQ
          ENDIF
        ENDIF
C
C 中間周波数のセット
        DTIFFR = DLCLFQ
C
C 遅延オフセット
        DTDLOF = DDOFST
C
C 初期位相、位相変化量、遅延時間の物理量計算
        CALL EI_381( DINTRL, DTAUG )
        IF( IDLFLG .EQ. 0 ) THEN
          DTAUG(1) = DTDLOF(1)
          DTAUG(2) = 0.0D0
          DTAUG(3) = 0.0D0
        ENDIF
#ifdef DEBUG
      write(*,*)'trk_21:Base line vv(DBASEL)=',
     +          DBASEL(1,1),DBASEL(2,1),DBASEL(3,1)
      write(*,*)'trk_21:Base line tv(DBASEV)=',
     +          DBASEV(1,1),DBASEV(2,1),DBASEV(3,1)
      write(*,*)'trk_21:IF frequency(DTIFFR)=',DTIFFR
      write(*,*)'trk_21:delay offset(DTDLOF)=',DTDLOF(1)
      write(*,*)'trk_21:sidereal HA (DICTRD)=',DICTRD(3)
      write(*,*)'trk_21:sidereal Dec(DICTRD)=',DICTRD(2)
      write(*,*)'trk_21:alpha       (DALPHA)=',DALPHA(1)
      write(*,*)'trk_21:beta        (DFBETA)=',DFBETA(1)
      write(*,*)'trk_21:taug        (DTAUG )=',DTAUG(1)
      write(*,*)'trk_21:taug        (DTAUG )=',DTAUG(2)
      write(*,*)'trk_21:taug        (DTAUG )=',DTAUG(3)
#endif
        IF( ITRF20 .EQ.1 ) THEN
          IRET = 200
          GOTO 9000
        ENDIF
C
C バンドの方向
        IF( IULSEL .EQ. 1 )THEN
          IBAND = 1
        ELSE
          IBAND = -1
        ENDIF
C
C フリンジ回転方向のセット
        IF( IROTDR .EQ. 0 )THEN
          JTFRRD(1) = 1
          JTFRRD(2) = 1
        ELSE IF( IROTDR .EQ. 1 )THEN
          JTFRRD(1) = 1
          JTFRRD(2) = -1
        ELSE IF( IROTDR .EQ. 2 )THEN
          JTFRRD(1) = -1
          JTFRRD(2) = 1
        ELSE IF( IROTDR .EQ. 3 )THEN
          JTFRRD(1) = -1
          JTFRRD(2) = -1
        ENDIF
C
C 周波数変換計数のセット
        DTMLPA = DTRNFQ(1)
        DTMLPA = DTRNFQ(2)
C
C フリンジ周波数オフセット
        IF( IDSBFG .EQ. 0 ) THEN
          RTFROF(1,1) = DFOFST
        ELSE
          RTFROF(1,1) = DFOFST
          RTFROF(1,2) = DFOFST
        ENDIF
C
C 初期位相、位相変化量計算
        CALL EI_382( IBAND, IDSBFG )
        IF( ITRF20 .EQ. 1 ) THEN
          IRET = 200
          GOTO 9000
        ENDIF
C
C 計算結果を出力領域へコピー
        DPHAS(1) = DFRIPH(1,1)
        DPHAS(2) = DFRIPH(1,2)
        DDPHAS(1) = DFRGFR(1,1)
        DDPHAS(2) = DFRGFR(1,2)
        IRET = 300
CC
CC ＵＶＷ計算
CC
      ELSE IF( ITRF20 .EQ. 4 ) THEN
        CALL TRK_26( DUVW, DHOURA )
#ifdef DEBUG
      write(*,*)'trk_21:UVW         (DUVW)=  ',DUVW
      write(*,*)'trk_21:Hour angle(DHOURA)=  ',DHOURA
#endif
        IRET = 4
      ELSE
        IRET = -1
        GOTO 9000
      ENDIF
C
      IF( UPTIME .EQ. 1 ) THEN
        DILOCL = DXLAST
      ENDIF
C
      RETURN
C
C----------------------------------------------------------------------C
C ERROR ENTRY                                                          C
C----------------------------------------------------------------------C
 9000 CONTINUE
C----------------------------------------------------------------------C
C EXIT                                                                 C
C----------------------------------------------------------------------C
      RETURN
      END
