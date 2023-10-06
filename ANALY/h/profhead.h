#include <astro.h>
/*	struct difinitions for COST60 data file header

T.OKA	1992/3/29
1992/4/16 T.Handa UNIX MiniSTAR
1992/8/26 T.Handa add for STAR system
1992/12/19 T.Handa
1993/9/29 T.Handa add for code definitions and PRF_HEADER
*/

#define MAX_CH		2048
#define	MAX_FLAG_CH	256	/* MAX_CH/8 */
#define	MAX_HISTORY_STEP	64
#define	MAX_FILE_NAME_LENGTH	128	/* see also in starlib.h */

#define MAX_HISTORY_LINES_PER_STEP	96

/* format ID */
#define SPECTRUM	1		/* １次元周波数データ */
#define	SCAN_FORMAT_VERSION	2	/*	Scaling Factor 追加	*/

/* data type */
#define RAW_BE	0	/* 分光器の生出力 */
#define ONOFF	1	/* アンテナ温度スケールon-offデータ */
#define FREQ_SW_RAW 2	/* 復調前周波数スイッチデータ */
#define FREQ_SW	3	/* 復調後周波数スイッチデータ */

/* coordinate system code */
#define COOD_LB		1
#define COOD_RADEC	2
#define COOD_RADEC_APP	10
#define COOD_AZEL	101

/* sideband code */
#define LSB_SSB 4
#define LSB_DSB 5
#define USB_SSB 6
#define USB_DSB 7
#define DSB 0x01
#define SSB 0x00
#define USB 0x02
#define LSB 0x00



typedef struct	{	unsigned long		l_file;		/* ファイル長 */
			unsigned short		l_head;		/* ヘッダー長 */
			unsigned char		format_id;	/* 形式識別 */
			unsigned char		version;	/* 形式版数 */
			unsigned char		data_type;	/* データ種別 */
			unsigned char		hist_step;	/* ヒストリィステップ数 */
			unsigned char		hist_line;	/* ヒストリィ総行数 */
			unsigned long		scan;		/* スキャン通番 */
			unsigned char		backup;		/* 直前のバックアップ番号 */
			unsigned char 		num_scan;	/* 構成している生スキャン数 */
		} SYS_HEAD;	/* profile system header */

typedef struct	{	char			project[17];	/* プロジェクト名 */
			char			scd_file[9];	/* 観測スケジューラ名 */
			unsigned short		line_numb;
			unsigned char		sw_mode;	/* スイッチングモード */
			unsigned long		mjd;		/* 観測日の修正ユリウス日 */
			long			start;		/* 観測開始時UNIX標準時刻 */
			long			stop;		/* 観測終了時UNIX標準時刻 */
			unsigned short		on_time;	/* on点積分時間 */
			unsigned short		off_time;	/* off点積分時間 */
			char			observer1[9];	/* 観測者氏名-1 */
			char			observer2[9];	/* 観測者氏名-2 */
			char			observer3[9];	/* 観測者氏名-3 */
			char			observer4[9];	/* 観測者氏名-4 */
		} OBS_PRM;	/* profile observation parameters */

typedef struct	{	char		name[16];	/* 天体名 */
			unsigned char	cood_sys;	/* 座標コード */
			XY_POS		on_lb;		/* on点銀経銀緯 */
			XY_POS		on_radec;	/* on点赤経赤緯(B1950.0) */
			XY_POS		on_azel;	/* on点積分開始時AzEl */
			XY_POS		off_lb;		/* off点銀経銀緯 */
			XY_POS		off_radec;	/* off点赤経赤緯(B1950.0) */
			XY_POS		orig_xy;	/* マップ原点の局所座標位置 */
			double		pa_xy;		/* 座標軸位置角 */
			XY_POS		on_xy;		/* on点局所座標位置 */
		} OBJECT;	/* profile object parameters */

typedef struct	{	char		tele[9];	/* 望遠鏡名 */
			float		ap_eff;		/* 開口能率 */
			float		mb_eff;		/* 主ビーム能率 */
			float		fss_eff;	/* 前方散乱能率 */
			float		hpbw;		/* ビーム半値幅 */
		} ANT_STATUS;	/* profile antenna parameters */

typedef struct	{	char		name[8];	/* 受信機名 */
			unsigned char	sb_mode;	/* サイドバンドモード */
			double		v_lsr;		/* LSR速度 */
			double		rest_freq;	/* 静止周波数 */
			double		obs_freq;	/* 観測周波数 */
			double		lo_freq1;	/* 第１局部発振周波数-1 */
			double		if_freq1;	/* 第１中間周波数-1 */
			double		lo_freq2;	/* 第１局部発振周波数-2 */
			double		if_freq2;	/* 第１中間周波数-2 */
			double		lo_freq3;	/* 第１局部発振周波数-3 */
			double		if_freq3;	/* 第１中間周波数-3 */
			unsigned short	mltplx1;	/* 逓倍数-1 */
			unsigned short	mltplx2;	/* 逓倍数-2 */
			unsigned short	mltplx3;	/* 逓倍数-3 */
			float		tsys;		/* システム雑音温度 */
			float		trx;		/* 受信機雑音温度 */
			float		rms;		/* プロファイルrmsレベル */
			float		calib;		/* 較正用黒体温度 */
			float		weight;		/* データの重み係数 */
			float		scale;		/* Scaling factor */
		} RX_STATUS;	/* profile receiver parameters */

typedef struct	{	char		name[9];	/* バックエンド名 */
			unsigned short	be_ch;		/* バックエンドチャンネル数 */
			unsigned short	basis_ch;	/* バックエンド規準チャンネル位置 */
			char		sign;		/* RF周波数-チャンネル番号増減方向 */
			unsigned short	data_ch;	/* プロファイルデータチャンネル数 */
			double		center_freq;		/* 規準AOS周波数(MHz) */
			double		disp0;		/* ０次分散 */
			double		disp1;		/* １次分散 */
			double		disp2;		/* ２次分散 */
			double		disp3;		/* ３次分散 */
			double		disp4;		/* ４次分散 */
/* freq_on_be(MHz)=disp0+disp1*ch+disp2*ch^2+disp3*ch^3+disp4*ch^4 */
			double		freq_res;	/* 周波数分解能 */
			float		ch_width;	/* binningしたch幅 */
			float		ch_orig;	/* binningした原点 */
		} BE_STATUS;	/* profile backend parameters */

typedef struct {
			float tmp; 	/* 大気温度(degree C) */
			float water; 	/* 水蒸気分圧(mbar) */
			float press; 	/* 地上 1.5m における大気圧(mbar) */
	       } F_METEO_PRM;

typedef struct	{	char			site_name[9];	/* 観測地名 */
			F_METEO_PRM		meteo;		/* 気象パラメータ */
			float			tau0;		/* 大気の光学的厚さ */
			char			memo[65];	/* メモ */
			char			comment[65];	/* コメント */
		} OBS_LOG;	/* profile observation logs */

typedef struct	{	unsigned char	size;	/* lines in this history */
			char		step[8];
			long		time;
			char		prm[MAX_HISTORY_LINES_PER_STEP][17]; /* Max 16Byte x "MAX_HISTORY_LINES_PER_STEP"line */
		} HISTORY;	/* profile process history */

typedef struct	{	char 	file_id[10];
			char	file_name[MAX_FILE_NAME_LENGTH];
			SYS_HEAD	sys_head;
			OBS_PRM		obs_prm;
			OBJECT		object;
			ANT_STATUS	ant_status;
			RX_STATUS	rx_status;
			BE_STATUS	be_status;
			OBS_LOG		obs_log;
			HISTORY		history[MAX_HISTORY_STEP];
		} PRF_HEADER;	/* profile header without data */


typedef struct	{	char	file_id[10];
			char	file_name[MAX_FILE_NAME_LENGTH];
			SYS_HEAD	sys_head;
			OBS_PRM		obs_prm;
			OBJECT		object;
			ANT_STATUS	ant_status;
			RX_STATUS	rx_status;
			BE_STATUS	be_status;
			OBS_LOG		obs_log;
			HISTORY		history[MAX_HISTORY_STEP];
			unsigned char	spurious[MAX_FLAG_CH];
			unsigned char	baseline[MAX_FLAG_CH];
			float		data[MAX_CH];
		} PROFILE;	/* profile with the header */

/* extern definition */

#ifdef _NO_PROTO
extern int	profile_in();
extern int	profile_out();

extern int	profile_load();
extern int	profile_save();

extern int	prfhead_load();

extern int	read_ant_status();
extern int 	read_baseline();
extern int 	read_be_status();
extern int 	read_history();
extern int 	read_object();
extern int 	read_obs_log();
extern int 	read_obs_prm();
extern int 	read_prof();
extern int 	read_rx_status();
extern int	read_spurious();
extern int 	read_sys_head();

extern int	write_ant_status();
extern int 	write_baseline();
extern int 	write_be_status();
extern int 	write_history();
extern int 	write_object();
extern int 	write_obs_log();
extern int 	write_obs_prm();
extern int 	write_prof();
extern int 	write_rx_status();
extern int	write_spurious();
extern int 	write_sys_head();

#else /* undef _NO_PROTO */
extern int	profile_in(PROFILE *);
extern int	profile_out(PROFILE *);

extern int	profile_load(FILE *, PROFILE *);
extern int	profile_save(FILE *, PROFILE *);

extern int	prfhead_load(FILE *, PRF_HEADER *);

extern int	read_ant_status(FILE *, ANT_STATUS *);
extern int 	read_baseline(FILE *, unsigned int, unsigned char *);
extern int 	read_be_status(FILE *, BE_STATUS *);
extern int 	read_history(FILE *, HISTORY *);
extern int 	read_object(FILE *, OBJECT *);
extern int 	read_obs_log(FILE *, OBS_LOG *);
extern int 	read_obs_prm(FILE *, OBS_PRM *);
extern int 	read_prof(FILE *, unsigned int, float *);
extern int 	read_rx_status(FILE *, RX_STATUS *, unsigned char);
extern int	read_spurious(FILE *, unsigned int, unsigned char *);
extern int 	read_sys_head(FILE *, SYS_HEAD *);

extern int	write_ant_status(FILE *, ANT_STATUS *);
extern int 	write_baseline(FILE *, unsigned int, unsigned char *);
extern int 	write_be_status(FILE *, BE_STATUS *);
extern int 	write_history(FILE *, HISTORY *);
extern int 	write_object(FILE *, OBJECT *);
extern int 	write_obs_log(FILE *, OBS_LOG *);
extern int 	write_obs_prm(FILE *, OBS_PRM *);
extern int 	write_prof(FILE *, unsigned int, float *);
extern int 	write_rx_status(FILE *, RX_STATUS *, unsigned char);
extern int	write_spurious(FILE *, unsigned int, unsigned char *);
extern int 	write_sys_head(FILE *, SYS_HEAD *);

#endif /* endif of define _NO_PROTO */
