C*--------------------------------------------------------------------*/
C*                                                                    */
C*  関数名   : trk_24                                                 */
C*                                                                    */
C*  和名     : ドップラー効果による受信周波数の補正                   */
C*                                                                    */
C*  機能概要 : 観測天体の視線速度、観測者の天体に対する速度によるドッ */
C*             プラー効果を補正し、観測局における受信周波数を求める   */
C*                                                                    */
C*  戻り値   : =   0 : 正常終了                                       */
C*                                                                    */
C*  注意事項 : なし                                                   */
C*                                                                    */
C*  作成者   : FUJITSU LIMITED 1996.07.08                             */
C*                                                                    */
C*  メモ     :                                                        */
C*             MODIFY 1997.04.03 ( FLUKE_FREQ  FNS )                  */
C*                                                                    */
C*--------------------------------------------------------------------*/
      SUBROUTINE trk_24( DEOBFR, IVREF, IVDEF )
C 宣言
#include      "__DFTF"
C コモン領域
#include      "EI_TTME"
#include      "XDUMMY"
#include      "XIERST"
#include      "XSTINF"
#include      "XTRK00"

C
C 引数
      REAL*8        DEOBFR              ! 静止周波数
      INTEGER*4     IVREF               ! 視線速度の基準系フラグ
      INTEGER*4     IVDEF               ! 視線速度の定義系フラグ
C
C                                       * WORK VARIABLE DEFINITION     *
C
      REAL*8        DFRLSR
      REAL*8        DUMMY
      REAL*8        DCHGVE
      INTEGER*2     ICHGST
      INTEGER*2     ICHGDE
      INTEGER*2     JRC
C
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C
C 計算開始
C
C 計算する天体が太陽外の天体の場合
C ドップラー効果を考慮した天体の周波数
      IF ( JESTCR .EQ. 1 ) THEN
        DCHGVE = DVELCR
C OPTICAL 定義処理
        ICHGST = IVDEF
        IF( ICHGST .EQ. 2 ) THEN
          DCHGVE = DVELCR / ( 1D0 + DVELCR / DVLLGT )
        ENDIF
C
        DFRLSR = DEOBFR * ( 1D0 - DCHGVE / DVLLGT )
C
C 上記以外の場合、観測天体の運動速度は無視する
      ELSE
        DFRLSR = DEOBFR
      ENDIF
C
C 絶対座標系に対する観測者の運動速度
      ICHGDE = IVREF
      CALL R_8100( DTSTJD, DUMMY, ICHGDE, JRC )
C
C 受信周波数
      DTFRCR = DFRLSR / ( 1D0 + DSTRVL / DVLLGT )
c     write(*,*)'trk_24 DSTRVL=', DSTRVL
C
      RETURN
      END
