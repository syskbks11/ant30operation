2008.03.18 Y.Koide
・M1の扇野 氏のためのlibtkbの技術資料。研究室のための技術資料ではない。
また、著作権を放棄してはいない。
・本ライブラリのライセンス形態はGNU LESSER GENERAL PUBLIC LICENSE(LGPL)とし、
何者もこのlibtkbライブラリを用いたプログラムをソースコードの公開無しで譲渡配布することを禁ずる。
ただし、libtkbの動的な外部ライブラリとしての使用に限り上記の限りではない。
詳細はLICENSE.TXTを参照。
・Makefileを用いてFFC用とgcc用(動作未確認)ライブラリのコンパイルが行える。
・libtkbのlibtkb.slnでVC++用ライブラリのコンパイルが行える

2008.03.01 Y.Koide
・rs232c, rs232c2のInit関数の引数parityBitの定義を修正。
今まで0:none 1:evenParity 2:oddParityだったのを0:none 1:oddParity 2:evenParityに直した。

2007.10.25 Y.Koide
・libtkb01を再構築
conf,conf2,netcl,netsv,netu,rs232c,tm,uを含む。

2007.10.21 Y.Koide
・confにてValの"の定義を変更。
ValはKeyと空文字を挟んで接しており、Val自体に空文字を含むことも可能とする。
よって、今まではValに空文字を含む場合に""で囲っていたのを廃止する。
同様にValの中に"を含んでいても""と変換して記す必要はない。

2007.10.18 Y.Koide
・ライブラリ関連用のリポジトリlibtkb01を作成。
・./src/{conf,netcl,netsv,netu,u,rs232c,tm}を作成
・Makefileを作成
% make
で./export/にスタティックライブラリが生成される。
・全ライブラリを集約した./export/libtkb.aを生成するようにした。
インクルードファ@イルは各ライブラリのを使用し、ライブラリのみlibtkb.aを使用する
こと
・使い方
% FCC -I/usr/FFC/include -I./myinclude/ libtkb.a xxx.cpp

以上
