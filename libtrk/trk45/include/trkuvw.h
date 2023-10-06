/* ＵＶＷ計算入力構造体 */
typedef	struct{
	double	dtime[2];	/* 観測時間 */
	double	dobsp[2];	/* 観測天体位置 */
	double	dantr[3];	/* 基準局アンテナ位置ベクトル */
	double	danto[3];	/* 観測局アンテナ位置ベクトル */
	double	dantv[9];	/* 観測局アンテナ器差補正値 */
	double	dpofst[2];	/* ポインティングオフセット */
				/* [0]: 方位角/赤経/銀経[rad] */
				/* [1]: 仰角/赤緯/銀度[rad] */
	double	dscnst[2];	/* スキャン開始位置[rad] */
				/* [0]: 方位角/赤経/銀経[rad] */
				/* [1]: 仰角/赤緯/銀度[rad] */
	double	dscned[2];	/* スキャン終了位置[rad] */
				/* [0]: 方位角/赤経/銀経[rad] */
				/* [1]: 仰角/赤緯/銀度[rad] */
	double	dobsfq;		/* 観測周波数[Hz] */
	double	dobsrv;		/* 観測天体視線速度[m/s] */
	int	ioflg;		/* 天体フラグ */
	int	iaflg;		/* 天体位置角フラグ */
	int	ibflg;		/* 天体位置分点フラグ */
	int	itnscn;		/* スキャンニング点数 */
				/* １以上の整数 */
	int	inoscn;		/* スキャンニング点 */
				/* １≦ スキャンニング点 ≦ スキャンニング点数 */
	int	irpflg;		/* ポインティング実角フラグ */
				/* ０：虚角 */
				/* １：実角 */
	int	iapflg;		/* ポインティング角フラグ */
				/* １：ＲＡＤＥＣ */
				/* ２：銀経銀緯 */
				/* ３：ＡＺＥＬ */
	int	irsflg;		/* スキャンニング実角フラグ */
				/* ０：虚角 */
				/* １：実角 */
	int	iasflg;		/* スキャンニング角フラグ */
				/* １：ＲＡＤＥＣ */
				/* ２：銀経銀緯 */
				/* ３：ＡＺＥＬ */
	int	inlocl;		/* 第一ローカル発振器逓倍次数 */
	int	inflck;		/* フルークシンセサイザ逓倍次数 */
}XUVWI;

/* 追尾計算出力構造体 */
typedef	struct{
	double	dflkfq;		/* 受信周波数[Hz] */
	double	duvw[3];	/* ＵＶＷ */
				/* [0]: Ｕ */
				/* [1]: Ｖ */
				/* [2]: ｗ */
	double	dhoura;		/* アワーアングル[rad] */
}XUVWO;
