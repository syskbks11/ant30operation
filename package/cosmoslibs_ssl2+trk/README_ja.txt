2023/9/19
************************************************************
COSMOS用追尾計算ライブラリ
************************************************************

■(1) 当該ライブラリのコンパイル方法

　$ cd libsrc
　$ make clean
　$ make
　ar: creating ../../../lib/libssl2.a
　ar: creating ../../../lib/libtrk.a

■(2) 当該ライブラリを組み込む際に必要とするファイル群

　$ cd lib
　$ ls -1
　libssl2.a ： 科学技術計算用ライブラリ
　libtrk.a  ： 追尾計算用ライブラリ
　ssl2.h    ： libssl2用関数プロトタイプ宣言ヘッダーファイル
　trk.h     ： libtrk 用関数プロトタイプ宣言ヘッダーファイル

■(3) 実行時の環境変数定義（ANTFILE, EPHFILE, TIMEFILE）

　$ cd env
　$ ls -1
　ant.dat   : アンテナ座標ファイル　※NRO用（参考）
　eph.dat   : 天体暦ファイル　　　　※2030-07-31まで
　time.dat  : MJD, TAI-UTC, UT1-UTC ※2024-08-30まで

　以下は、環境変数の設定例
　export ANTFILE=env/ant.dat
　export EPHFILE=env/eph.dat
　export TIMEFILE=env/time.dat
