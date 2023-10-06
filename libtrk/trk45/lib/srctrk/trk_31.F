C*--------------------------------------------------------------------*/
C*                                                                    */
C*  関数名   : trk_31                                                 */
C*                                                                    */
C*  和名     : ＦＯＲＴＲＡＮの共通領域設定                           */
C*                                                                    */
C*  機能概要 : ＦＯＲＴＲＡＮの共通領域へ値を設定する。               */
C*                                                                    */
C*  戻り値   : =   0: 正常終了                                        */
C*                                                                    */
C*  注意事項 : なし                                                   */
C*                                                                    */
C*  作成者   : FUJITSU LIMITED 1996.09.30                             */
C*                                                                    */
C*  メモ     :                                                        */
C*                                                                    */
C*--------------------------------------------------------------------*/
      SUBROUTINE trk_31( IDAZEL, IDWEAT, IDGDIN, IDLAST,
     +                   ODAZEL, ODRADE, IRET           )
C                                       *------------------------------*
C                                       * DECLARE DIVITION             * 
C                                       *------------------------------*
#include      "__DFTF"
C
C                                       * ARGUMENT VARIABLE DEFINITION *
C
      REAL*8        IDAZEL(2)           ! ＡＺＥＬ値
      REAL*8        IDWEAT(3)           ! 気象パラメタ
      REAL*8        IDGDIN(2)           ! 経度、緯度
      REAL*8        IDLAST              ! 瞬時の視恒星時
C
      REAL*8        ODAZEL(2)           ! 大気差補正後のＡＺＥＬ値
      REAL*8        ODRADE(2)           ! 大気差補正後の測心視位置
      INTEGER*4     IRET
C
C                                       * COMMON VARIABLE DEFINITION   *
#include      "EI_TTME"
C
C                                       * WORK VARIABLE DEFINITION     * 
C
      INTEGER*2     JRET
C
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
      IRET = 0
C
C 気象パラメタのセット
      DTWTRP(1) = IDWEAT(1)
      DTWTRP(2) = IDWEAT(2)
      DTWTRP(3) = IDWEAT(3)
C
C 大気補正
      CALL EI_370( IDAZEL, IDGDIN, IDLAST, ODAZEL, ODRADE, JRET )
      IF( JRET .NE. 0 ) THEN
        IRET = JRET
        GOTO 9000
      ENDIF
C
C----------------------------------------------------------------------C
C ERROR ENTRY                                                          C
C----------------------------------------------------------------------C
 9000 CONTINUE
C
C----------------------------------------------------------------------C
C EXIT                                                                 C
C----------------------------------------------------------------------C
      RETURN
      END
