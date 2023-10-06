C*--------------------------------------------------------------------*/
C*                                                                    */
C*  関数名   : trk_12                                                 */
C*                                                                    */
C*  和名     : 惑星、観測アンテナ情報の設定                           */
C*                                                                    */
C*  機能概要 : 共通領域へ惑星、観測アンテナの情報を設定する。         */
C*                                                                    */
C*  戻り値   : =    0: 正常終了                                       */
C*             =    1: フラグチェックでエラー終了                     */
C*                                                                    */
C*  注意事項 : なし                                                   */
C*                                                                    */
C*  作成者   : FUJITSU LIMITED 1996.06.28                             */
C*                                                                    */
C*  メモ     :                                                        */
C*                                                                    */
C*--------------------------------------------------------------------*/
      SUBROUTINE trk_12( ANTCD, IOFLG, IERCOD )
C                                       *------------------------------*
C                                       * DECLARE DIVITION             *
C                                       *------------------------------*
#include      "__DFTF"
C                                       * ARGUMENT VARIABLE DEFINITION *
      REAL*8        ANTCD(3)            ! アンテナ位置ベクトル
      INTEGER*4     IOFLG               ! 天体フラグ
      INTEGER*4     IERCOD              ! リターンコード
C コモン
#include      "EI_TTME"
#include      "EI_LMAI"
#include      "XDFWRK"
#include      "XEPHEM"
#include      "XIERST"
#include      "XTMERT"
#include      "XOBSEI"
#include      "XTRK00"
C
C                                       * WORK VARIABLE DEFINITION     *
C
      REAL*8        DXDBA1(8), DXDBA2(8), DXDBA3(6), VW(26)
      INTEGER*2     JECODE, JC1, JC2, JC3
      INTEGER*4     IRC , ICON
      CHARACTER     CEL , CM*79
      INTEGER*4     ICOMET
C
C                                       * CONSTANT DATA DEFINITION     *
      DATA   CEL    / 'W' /
      DATA   CM     /'XEI302W trk_12  INCONSISTENT STATION NUMBER.' /
      DATA   ICOMET / 12 /
C
C リターンコードの初期化
      IERCOD = 0
C
C フラグのチェック
      IF( ITRF00 .NE. 1 ) THEN
        IERCOD = 1
        GOTO 9000
      ENDIF
C 
C ＶＬＢＩ座標系における基準局位置ベクトル
C                       観測局位置ベクトル
C                       ベースラインベクトルをコモンへ設定する
      DO 1000 JC1 = 1, 3
        DVLBIP( JC1, 1 ) = ANTCD( JC1 )
        DVLBIP( JC1, 2 ) = ANTCD( JC1 )
        DBASEL( JC1, 1 ) = 0.0D0
 1000 CONTINUE
C
C 天体フラグから天体名を割り当てる
      IF( IOFLG .EQ. 0 ) THEN
        JESTCR = 1
        CESTNM = 'STAR'             ! 惑星以外の天体
      ELSEIF( IOFLG .EQ. 1 ) THEN
        JESTCR = 3
        CESTNM = 'MERCURY'          ! 水星
      ELSEIF( IOFLG .EQ. 2 ) THEN
        JESTCR = 3
        CESTNM = 'VENUS'            ! 金星
      ELSEIF( IOFLG .EQ. 3 ) THEN
        JESTCR = 3
        CESTNM = 'EARTH'            ! 地球
      ELSEIF( IOFLG .EQ. 4 ) THEN
        JESTCR = 3
        CESTNM = 'MARS'             ! 火星
      ELSEIF( IOFLG .EQ. 5 ) THEN
        JESTCR = 3
        CESTNM = 'JUPITER'          ! 木星
      ELSEIF( IOFLG .EQ. 6 ) THEN
        JESTCR = 3
        CESTNM = 'SATURN'           ! 土星
      ELSEIF( IOFLG .EQ. 7 ) THEN
        JESTCR = 3
        CESTNM = 'URANUS'           ! 天王星
      ELSEIF( IOFLG .EQ. 8 ) THEN
        JESTCR = 3
        CESTNM = 'NEPTUNE'          ! 海王星
      ELSEIF( IOFLG .EQ. 9 ) THEN
        JESTCR = 3
        CESTNM = 'PLUTO'            ! 冥王星
      ELSEIF( IOFLG .EQ. 10 ) THEN
        JESTCR = 3
        CESTNM = 'MOON'             ! 月
      ELSEIF( IOFLG .EQ. 11 ) THEN
        JESTCR = 3
        CESTNM = 'SUN'              ! 太陽
      ELSEIF( IOFLG .EQ. ICOMET ) THEN
        JESTCR = 3
        CESTNM = 'SUN'              ! 彗星
      ELSE
        JESTCR = 2
        CESTNM = 'OTHER'            ! その他
      ENDIF  
C                                                                       
C ＥＰＨ読みだし時刻（ＭＪＤ、ＴＤＢ）
      DXEMTM = DTDBJD                                               
C
C 太陽、地球、惑星のＥＰＨデータをそれぞれ太陽系中心、太陽中心の
C ベクトルへ変換する
      CALL R_1300( DTPLPS , JESTCR , CESTNM , JECODE )              
      IF (IOFLG .EQ. ICOMET) CESTNM = 'COMET';
C
C リターンコードのチェック
      IF( JECODE .NE. 0 ) THEN
        IERCOD = 2
        GOTO 9000
      ENDIF
C 
C                                                                       
C     内挿係数作成
      DO 2000 JC3 = 1 , 3 , 1                                       
        DO 2100 JC2 = 1 , 3 , 1                                     
C 太陽系中心座標
          DO 2110 JC1 = 1 , 8 , 1                                   
            DXDBA1( JC1 ) = DBARAG( JC1 , JC2 , JC3 )               
 2110     CONTINUE                                                  
C スプライン補間
          CALL DBIC3( DTSTJD, DXDBA1, 8, 3, DXDBA2, DXDBA3, VW, ICON )
C スプライン補間係数
          DO 2120 JC1 = 1 , 8 , 1                                   
            DINTBR( JC1 , JC2 , JC3 ) = DXDBA2( JC1 )               
            IF ( JC1 .LE. 6 ) THEN                                  
              DBRNOD( JC1 , JC2 , JC3 ) = DXDBA3( JC1 )             
            ENDIF                                                   
 2120     CONTINUE                                                  
C                                                                       
C 太陽中心座標
          IF ( JC3 .LE. 2 ) THEN                                    
            DO 2130 JC1 = 1 , 8 , 1                                 
              DXDBA1( JC1 ) = DHELAG( JC1 , JC2 , JC3 )             
 2130       CONTINUE                                                
C スプライン補間
            CALL DBIC3( DTSTJD, DXDBA1, 8, 3, DXDBA2, DXDBA3, VW, ICON )
C スプライン補間係数
            DO 2140 JC1 = 1 , 8 , 1                                 
              DINTHL( JC1 , JC2 , JC3 ) = DXDBA2( JC1 )             
              IF ( JC1 .LE. 6 ) THEN                                
                DHLNOD( JC1 , JC2 , JC3 ) = DXDBA3( JC1 )           
              ENDIF                                                 
 2140       CONTINUE                                                
          ENDIF                                                     
C                                                                       
 2100   CONTINUE                                                    
 2000 CONTINUE                                                      
C
C エラーメッセージ出力
 9000 CONTINUE
      IF( IERCOD .NE. 0 )THEN
        CALL O_MSG2( IMID00, CEL, CM, IRC )
      ENDIF
C
C
      RETURN
      END
