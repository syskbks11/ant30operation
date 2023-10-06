C*--------------------------------------------------------------------*/
C*                                                                    */
C*  関数名   : trk_26                                                 */
C*                                                                    */
C*  和名     :  ＵＶＷ計算                                            */
C*                                                                    */
C*  機能概要 : ＵＶＷの計算をする。                                   */
C*                                                                    */
C*  戻り値   : =   0 : 正常終了                                       */
C*                                                                    */
C*  注意事項 : なし                                                   */
C*                                                                    */
C*  作成者   : FUJITSU LIMITED 1996.07.16                             */
C*                                                                    */
C*  メモ     :                                                        */
C*                                                                    */
C*--------------------------------------------------------------------*/
      SUBROUTINE trk_26( DUVW, DHOURA )
C
C 宣言
C
#include      "__DFTF"
C コモン領域
#include      "XDFWRK"
#include      "XOBSEI"
#include      "XSTINF"
#include      "XIERST"
C
C 引数
      REAL*8        DUVW(3)             ! ＵＶＷ
      REAL*8        DHOURA              ! アワーアングル
      REAL*8        DWK                 ! 位相中心の経度
      REAL*8        DRTMT(3,3)          ! 回転行列
      REAL*8        DBSL(3)             ! ベースラインベクトル
      REAL*8        DVW(3)              ! 作業用ベクトル
      INTEGER*2     JZ                  ! 回転行列軸番号
      INTEGER*2     JRS                 ! リターンコード
C
      DATA          JZ / 3 /
C
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C
C 観測者の経度
      DWK = DGDINF(1,1)
C
C 極軸回りに回転する行列作成
      CALL R_5410( DWK, DRTMT, JZ, JRS )
C
C ベースラインベクトルへ適応
      DBSL(1) = DBASEL(1,1)
      DBSL(2) = DBASEL(2,1)
      DBSL(3) = DBASEL(3,1)
C
C 回転行列とベースラインベクトルの積
      CALL DMAV(DRTMT, 3, 3, 3, DBSL, DVW, ICON)
C
C 観測点子午線基準の地球固定座標系のベースラインベクトル
      DBASEV(1,1) = DVW(1)
      DBASEV(2,1) = DVW(2)
      DBASEV(3,1) = DVW(3)
C
C 計算開始
      DUVW(1) = DBASEV(1,1) * DSIN(DICTRD(3))
     +        + DBASEV(2,1) * DCOS(DICTRD(3))
      DUVW(2) =               DSIN(DICTRD(2))
     +    * ( - DBASEV(1,1) * DCOS(DICTRD(3))
     +        + DBASEV(2,1) * DSIN(DICTRD(3)) )
     +        + DBASEV(3,1) * DCOS(DICTRD(2))
      DUVW(3) =               DCOS(DICTRD(2))
     +      * ( DBASEV(1,1) * DCOS(DICTRD(3))
     +        - DBASEV(2,1) * DSIN(DICTRD(3)) )
     +        + DBASEV(3,1) * DSIN(DICTRD(2))
C Unit transfer [sec] to [m]
      DUVW(1) = DUVW(1) * DVLLGT
      DUVW(2) = DUVW(2) * DVLLGT
      DUVW(3) = DUVW(3) * DVLLGT
      DHOURA = DICTRD(3)
C
      RETURN
      END
