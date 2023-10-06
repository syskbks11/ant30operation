C*---------------------------------------------------------------------C
C*                                                                     C
C*  関数名   : O_MSG2                                                  C
C*                                                                     C
C*  和名     : エラーメッセージ出力                                    C
C*                                                                     C
C*  機能概要 : エラーメッセージを出力する。                            C
C*                                                                     C
C*  戻り値   :                                                         C
C*                                                                     C
C*  注意事項 : なし                                                    C
C*                                                                     C
C*  作成者   : FUJITSU LIMITED  1996.06.21                             C
C*                                                                     C
C*  メモ     :                                                         C
C*                                                                     C
C*---------------------------------------------------------------------C
      SUBROUTINE O_MSG2( IMID, CEL, CM, IRET )

      INTEGER*4  IMID                         ! メッセージＩＤ
      INTEGER*4  IRET                         ! リターンコード
      CHARACTER  CEL                          ! エラー識別子
      CHARACTER  CM*79                        ! エラーメッセージ

      IMID = IMID
      IRET = IRET

C
C メッセージ出力
C
      WRITE( 0, * ) CEL, CM
C
C 終了
C
      RETURN
      END
