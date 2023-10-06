C*--------------------------------------------------------------------*/
C*                                                                    */
C*  関数名   : trk_11                                                 */
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
C*                                                                    */
C*--------------------------------------------------------------------*/
      SUBROUTINE trk_11( ANTCD, IOFLG, IPFLG, IAFLG, IBFLG, DSOURC,
     +                   DRDLV, IRET )
C 宣言
#include	"__DFTF"
C コモン領域
#include        "EI_LATR"
#include        "EI_TTME"
#include	"XTRK00"
#include        "XDUMMY"
#include        "XVWORK"
#include        "XEPHEM"

C 引数
      REAL*8            ANTCD(3)        ! アンテナ位置ベクトル
      INTEGER*4		IOFLG		! 天体フラグ
      INTEGER*4		IPFLG		! 動作ローカル種別フラグ
      INTEGER*4		IAFLG		! 天体位置角フラグ
      INTEGER*4		IBFLG		! 分点フラグ
      REAL*8		DSOURC(2)	! 観測天体位置
      REAL*8		DRDLV		! 観測天体の視線測度
      INTEGER*4		IRET		! リターンコード
C
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C
C     trk_00 フラグチェックの
      IRET = 0
      IF ( ITRF00 .NE. 1 ) THEN
        IRET = 1
        GOTO 9000
      ENDIF
C
C 惑星、観測アンテナ情報の設定
      CALL trk_12( ANTCD, IOFLG, IRET )
      IF( IRET .NE. 0 ) THEN
        IRET = 12
        GOTO 9000
      ENDIF
C
C 観測天体の情報をコモンへ設定
      ITRKBJ = IBFLG
      ITRKAX = IAFLG
      DEPSFE(1) = DSOURC(1)
      DEPSFE(2) = DSOURC(2)
      DVELCR = DRDLV
C initialize common variable
      DPLDIS = 0.0D0
      DOPBAR(1) = 0.0D0
      DOPBAR(2) = 0.0D0
      DOPBAR(3) = 0.0D0
      DPRPCR(1) = 0.0D0
      DPRPCR(2) = 0.0D0
      DANCRC = 0.0D0
      DTMLPB = 0.0D0
C 方位角、仰角を直接入力された場合追尾はしない
      IF( IAFLG .EQ. 3 )THEN
        JTATMD = 0
      ELSE
        JTATMD = 1
      ENDIF
C
#ifdef DEBUG
      write(*,*)'TRK_11:OBJECT FLG=',IOFLG,'(',CESTNM,')'
      if( IBFLG .eq. 1 )then
        write(*,*)'TRK_11:EPOCH FLG =',IBFLG,'(B1950.0)'
      else
        write(*,*)'TRK_11:EPOCH FLG =',IBFLG,'(J2000.0)'
      endif
      if( IAFLG .eq. 1 )then
        write(*,*)'TRK_11:CODE FLAG =',IAFLG,'(R.A.,DEC.)'
      else if( IAFLG .eq. 2 )then
        write(*,*)'TRK_11:CODE FLAG =',IAFLG,'(LB)'
      else
        write(*,*)'TRK_11:CODE FLAG =',IAFLG,'(AZ,EL)'
      endif
      write(*,*)'TRK_11:SOURCE POS=',DSOURC
      write(*,*)'TRK_11:RADIAL VER=',DRDLV
      if( IPFLG .eq. 0 )then
        write(*,*)'TRK_11:PROCESS FL=',IPFLG,'(ANTENNA AZ,EL)'
      else if( IPFLG .eq. 1 )then
        write(*,*)'TRK_11:PROCESS FL=',IPFLG,'(DELAY)'
      else if( IPFLG .eq. 2 )then
        write(*,*)'TRK_11:PROCESS FL=',IPFLG,'(PHASE)'
      else if( IPFLG .eq. 3 )then
        write(*,*)'TRK_11:PROCESS FL=',IPFLG,'(FREQUENCY)'
      else
        write(*,*)'TRK_11:CODE FLAG =',IPFLG,'(UVW)'
      endif
#endif
C
C     静的設定項目完了フラグの更新
      IRET = 0
      ITRF10 = 1
      ITRF20 = IPFLG
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
