/* struct definitions
for COST60cm telescope driver

T.Handa 1991/12/18
T.Handa 1992/08/26 Reduce for STAR system
*/

typedef struct {
		double l; 	/* Ｘ軸方向の方向余弦 */
		double m; 	/* Ｙ軸方向の方向余弦 */
		double n; 	/* Ｚ軸方向の方向余弦 */
	} DIRECT_COS;

typedef struct {
		double d_psi; 		/* 黄経の章動 ⊿ψ(rad) */
		double d_epsilon; 	/* 黄道傾角の章動 ⊿ε(rad) */
		double epsilon; 	/* 平均黄道傾角 ε(rad) */
	} NUTATION_PRM;

typedef struct {
		double x; 	/* 観測地点経度(degree):東経が正 */
		double y; 	/* 観測地点緯度(degree):北緯が正 */
		double z; 	/* 観測地点海抜(m) */
	} SITE_POS;

typedef struct {
		double tmp; 	/* 大気温度(degree C) */
		double water; 	/* 水蒸気分圧(mbar) */
		double press; 	/* 地上 1.5m における大気圧(mbar) */
	} METEO_PRM;

typedef struct {
		double rest; 	/* 静止周波数(GHz) */
		double ifreq; 	/* 第１中間周波数(GHz) */
		double v_lsr; 	/* 天体のＬＳＲ速度(km/s) */
		double v_trk; 	/* 追尾速度(km/s) */
		double sw;		/* スイッチング速度(Hz) */
		double offset;	/* 周波数スイッチングオフセット周波数(GHz) */
		short  sb_mode; /* サイドバンドモード 0x0004:LSB-SSB,0x0005:LSB-DSB,0x0006:USB-SSB,0x0007:USB-DSB */
	} FREQ;

typedef struct {
		double d; 		/* 1899/12/31 12:00(ET)から現在までの経過暦表時（日） */
		double t; 		/* 1899/12/31 12:00(ET)から現在までの経過暦表時（ユリウス世紀） */
		double t50; 	/* B1950.0からの経過時（ベッセル世紀） */
	} EP_TIME;

typedef struct {
		double x_rad; 	/* 天体経度(rad) */
		double y_rad; 	/* 天体緯度(rad) */
	} XY_POS_RAD;

