C*--------------------------------------------------------------------*/
C*                                                                    */
C*  関数名   : trk_01                                                 */
C*                                                                    */
C*  和名     : ＦＯＲＴＲＡＮの共通領域設定                           */
C*                                                                    */
C*  機能概要 : ＦＯＲＴＲＡＮの共通領域へ値を設定する。               */
C*                                                                    */
C*  戻り値   : =   0: 正常終了                                        */
C*                                                                    */
c*  注意事項 : なし                                                   */
C*                                                                    */
C*  作成者   : FUJITSU LIMITED 1996.06.10                             */
C*                                                                    */
C*  メモ     :                                                        */
C*                                                                    */
C*--------------------------------------------------------------------*/
      SUBROUTINE trk_01( DMJDJST, DEPH, DTIME, IRET )
C 宣言
#include	"__DFTF"
C コモン領域
#include	"XTRK00"
#include	"XTMERT"
#include	"EI_LMAI"
#include        "EI_TTME"
C
C 引数
      REAL*8		DMJDJST         ! 計算開始日本ＭＪＤ[sec]
      REAL*8            DEPH(248)       ! ＥＰＨデータ
      REAL*8            DTIME(3)        ! ＴＩＭＥデータ
      REAL*8 		DEJSTD		! 計算開始日本ＭＪＤ[day]
      INTEGER*4		IRET		! リターンコード
      REAL*8 		DELAST		! 地方恒星時
      CHARACTER		CEL, CM*79      ! エラーメッセージ
      INTEGER*4         I, J1, J2, K
C
C データ
      DATA   CEL, CM / 'E', 'XEI300E trk_01  TRACKING ABNORMAL END.' /  

C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C 日本標準時におけるＭＪＤ[day]
      DEJSTD = DMJDJST
C
C アンテナの駆動限界値を設定
      CALL EI_305( DELAST )                             
C
C ＥＰＨ／ＴＩＭＥデータのテンポラリー領域へデータを設定する
C
      K = 1
      DO 1000 I = 1, 8
        DTSTJD(I) = DEPH(K)           ! データの時刻(MJD[day] at TDB )
        K = K + 1
        DO 1100 J2 = 1, 10
          DO 1110 J1 = 1, 3
            DTPLPS(I,J1,J2) = DEPH(K) ! 惑星の位置
            K = K + 1
 1110     CONTINUE
 1100   CONTINUE
 1000 CONTINUE
C
C 惑星の出現順は以下の通り
C   水星  金星  地球  火星  木星  土星  天王星  海王星  冥王星  月
C
      DCORPR = DTIME(2)               ! UT1-TAI
C for db
C      DCORPR = -30.072999954223626
      ILEAPT = DTIME(3)               ! TAI-UTC
      DNPLXY(1) = 0.0d0               ! Polar motion X
      DNPLXY(2) = 0.0d0               ! Polar motion Y
C
C 開始時刻に於けるＵＴ１、ＴＡＩ、ＴＤＢ、ＧＡＳＴ等を求める
      CALL EI_320( DEJSTD )                             
C                                                                       
C 静的設定項目完了フラグの更新
C
      IRET = 0
      ITRF00 = 1
C
      RETURN
C
C----------------------------------------------------------------------C
C ERROR ENTRY                                                          C
C----------------------------------------------------------------------C
C
 9000 CONTINUE
      CALL O_MSG2( IMID00, CEL, CM, IRC ) 
C                                                                       
C----------------------------------------------------------------------C
C EXIT                                                                 C
C----------------------------------------------------------------------C
C
      RETURN
      END
