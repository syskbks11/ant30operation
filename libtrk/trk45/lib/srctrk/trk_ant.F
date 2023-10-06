C*--------------------------------------------------------------------*/
C*                                                                    */
C*  関数名   : trk_ant                                                */
C*                                                                    */
C*  機能概要 : 追尾計算時に算出する値の取得（アンテナローカル用）     */ 
C*                                                                    */
C*  注意事項 : なし                                                   */
C*                                                                    */
C*  作成者   : FUJITSU LIMITED 1997.09.30                             */ 
C*                                                                    */
C*  メモ     :                                                        */
C*                                                                    */
C*--------------------------------------------------------------------*/
      SUBROUTINE trk_ant( DPOOUT, DMAPRD, DMAPLB, DMAPAE, DSCNRD,
     +                    DSCNLB, DSCNAE, DXRDEC, DLOCAL )
C
#include        "XANTEN"
C
      REAL*8            DPOOUT(2)       ! 観測者の経緯度
      REAL*8            DMAPRD(2)       ! マップセンター座標 (RADEC)
      REAL*8            DMAPLB(2)       ! マップセンター座標 (LB   )
      REAL*8            DMAPAE(2)       ! マップセンター座標 (AZEL )
      REAL*8            DSCNRD(2)       ! 観測位置座標       (RADEC)
      REAL*8            DSCNLB(2)       ! 観測位置座標       (LB   )
      REAL*8            DSCNAE(2)       ! 観測位置座標       (AZEL )
      REAL*8            DXRDEC(2)       ! 測心視位置         (RADEC)
      REAL*8            DLOCAL          ! 地方恒星時 
C
      REAL*8            DPINT1,  DPINT2
      REAL*8            DMPRD1,  DMPRD2
      REAL*8            DMPLB1,  DMPLB2
      REAL*8            DMPAE1,  DMPAE2
      REAL*8            DSNRD1,  DSNRD2
      REAL*8            DSNLB1,  DSNLB2
      REAL*8            DSNAE1,  DSNAE2
      REAL*8            DXTCRA,  DXTCDE
      REAL*8            DILOCL
C
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
      DPOOUT(1) = DPINT1
      DPOOUT(2) = DPINT2
C
      DMAPRD(1) = DMPRD1 
      DMAPRD(2) = DMPRD2 
      DMAPLB(1) = DMPLB1 
      DMAPLB(2) = DMPLB2 
      DMAPAE(1) = DMPAE1 
      DMAPAE(2) = DMPAE2 
      DSCNRD(1) = DSNRD1 
      DSCNRD(2) = DSNRD2 
      DSCNLB(1) = DSNLB1 
      DSCNLB(2) = DSNLB2 
      DSCNAE(1) = DSNAE1 
      DSCNAE(2) = DSNAE2 
C
      DXRDEC(1) = DXTCRA
      DXRDEC(2) = DXTCDE
      DLOCAL    = DILOCL
C
C----------------------------------------------------------------------C
C EXIT                                                                 C
C----------------------------------------------------------------------C
      RETURN
      END
