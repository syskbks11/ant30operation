	/* 日付の長さ */
#define		LDATE	32
	/* 一日の長さ[sec] */
#define		DAYSEC	86400.0
	/* J2000元期でのMJD[day] */
#define		D_J2000	51544.5
	/* ＪＳＴの時刻差[sec] */
#define		MJD_JST	32400.0
	/* ＭＪＤからカレンダデーへの変換時間範囲[day] */
#define		MJDMAX	51544.0		/* 2000/1/1 */
#define		MJDMIN	15110.0		/* 1900/4/1 */
	/* ＴＩＭＥファイルレコード長 */
#define		RECSIZE_TIME	43
#define		RECSIZE_EPH	707
	/* アンテナ数 */
#define		IPB		6

	/* 彗星データの最大格納数 (CMTMAX) と格納用テーブル */
#define		CMTMAX		86402
struct {
  int  index;			/* 検索用先頭 index */
  int  imax;			/* 取得したデータ数 */
  double ldmjd[CMTMAX];		/* MJD [sec] */
  double ldrad[CMTMAX][2];	/* 天体座標 (RADEC) [2] [rad] */
  double lddkm[CMTMAX];		/* 地心からの天体の距離 [m] */
} t_cmt;
