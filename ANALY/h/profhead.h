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
#define SPECTRUM	1		/* �P�������g���f�[�^ */
#define	SCAN_FORMAT_VERSION	2	/*	Scaling Factor �ǉ�	*/

/* data type */
#define RAW_BE	0	/* ������̐��o�� */
#define ONOFF	1	/* �A���e�i���x�X�P�[��on-off�f�[�^ */
#define FREQ_SW_RAW 2	/* �����O���g���X�C�b�`�f�[�^ */
#define FREQ_SW	3	/* ��������g���X�C�b�`�f�[�^ */

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



typedef struct	{	unsigned long		l_file;		/* �t�@�C���� */
			unsigned short		l_head;		/* �w�b�_�[�� */
			unsigned char		format_id;	/* �`������ */
			unsigned char		version;	/* �`���Ő� */
			unsigned char		data_type;	/* �f�[�^��� */
			unsigned char		hist_step;	/* �q�X�g���B�X�e�b�v�� */
			unsigned char		hist_line;	/* �q�X�g���B���s�� */
			unsigned long		scan;		/* �X�L�����ʔ� */
			unsigned char		backup;		/* ���O�̃o�b�N�A�b�v�ԍ� */
			unsigned char 		num_scan;	/* �\�����Ă��鐶�X�L������ */
		} SYS_HEAD;	/* profile system header */

typedef struct	{	char			project[17];	/* �v���W�F�N�g�� */
			char			scd_file[9];	/* �ϑ��X�P�W���[���� */
			unsigned short		line_numb;
			unsigned char		sw_mode;	/* �X�C�b�`���O���[�h */
			unsigned long		mjd;		/* �ϑ����̏C�������E�X�� */
			long			start;		/* �ϑ��J�n��UNIX�W������ */
			long			stop;		/* �ϑ��I����UNIX�W������ */
			unsigned short		on_time;	/* on�_�ϕ����� */
			unsigned short		off_time;	/* off�_�ϕ����� */
			char			observer1[9];	/* �ϑ��Ҏ���-1 */
			char			observer2[9];	/* �ϑ��Ҏ���-2 */
			char			observer3[9];	/* �ϑ��Ҏ���-3 */
			char			observer4[9];	/* �ϑ��Ҏ���-4 */
		} OBS_PRM;	/* profile observation parameters */

typedef struct	{	char		name[16];	/* �V�̖� */
			unsigned char	cood_sys;	/* ���W�R�[�h */
			XY_POS		on_lb;		/* on�_��o��� */
			XY_POS		on_radec;	/* on�_�Ԍo�Ԉ�(B1950.0) */
			XY_POS		on_azel;	/* on�_�ϕ��J�n��AzEl */
			XY_POS		off_lb;		/* off�_��o��� */
			XY_POS		off_radec;	/* off�_�Ԍo�Ԉ�(B1950.0) */
			XY_POS		orig_xy;	/* �}�b�v���_�̋Ǐ����W�ʒu */
			double		pa_xy;		/* ���W���ʒu�p */
			XY_POS		on_xy;		/* on�_�Ǐ����W�ʒu */
		} OBJECT;	/* profile object parameters */

typedef struct	{	char		tele[9];	/* �]������ */
			float		ap_eff;		/* �J���\�� */
			float		mb_eff;		/* ��r�[���\�� */
			float		fss_eff;	/* �O���U���\�� */
			float		hpbw;		/* �r�[�����l�� */
		} ANT_STATUS;	/* profile antenna parameters */

typedef struct	{	char		name[8];	/* ��M�@�� */
			unsigned char	sb_mode;	/* �T�C�h�o���h���[�h */
			double		v_lsr;		/* LSR���x */
			double		rest_freq;	/* �Î~���g�� */
			double		obs_freq;	/* �ϑ����g�� */
			double		lo_freq1;	/* ��P�Ǖ����U���g��-1 */
			double		if_freq1;	/* ��P���Ԏ��g��-1 */
			double		lo_freq2;	/* ��P�Ǖ����U���g��-2 */
			double		if_freq2;	/* ��P���Ԏ��g��-2 */
			double		lo_freq3;	/* ��P�Ǖ����U���g��-3 */
			double		if_freq3;	/* ��P���Ԏ��g��-3 */
			unsigned short	mltplx1;	/* ���{��-1 */
			unsigned short	mltplx2;	/* ���{��-2 */
			unsigned short	mltplx3;	/* ���{��-3 */
			float		tsys;		/* �V�X�e���G�����x */
			float		trx;		/* ��M�@�G�����x */
			float		rms;		/* �v���t�@�C��rms���x�� */
			float		calib;		/* �r���p���̉��x */
			float		weight;		/* �f�[�^�̏d�݌W�� */
			float		scale;		/* Scaling factor */
		} RX_STATUS;	/* profile receiver parameters */

typedef struct	{	char		name[9];	/* �o�b�N�G���h�� */
			unsigned short	be_ch;		/* �o�b�N�G���h�`�����l���� */
			unsigned short	basis_ch;	/* �o�b�N�G���h�K���`�����l���ʒu */
			char		sign;		/* RF���g��-�`�����l���ԍ��������� */
			unsigned short	data_ch;	/* �v���t�@�C���f�[�^�`�����l���� */
			double		center_freq;		/* �K��AOS���g��(MHz) */
			double		disp0;		/* �O�����U */
			double		disp1;		/* �P�����U */
			double		disp2;		/* �Q�����U */
			double		disp3;		/* �R�����U */
			double		disp4;		/* �S�����U */
/* freq_on_be(MHz)=disp0+disp1*ch+disp2*ch^2+disp3*ch^3+disp4*ch^4 */
			double		freq_res;	/* ���g������\ */
			float		ch_width;	/* binning����ch�� */
			float		ch_orig;	/* binning�������_ */
		} BE_STATUS;	/* profile backend parameters */

typedef struct {
			float tmp; 	/* ��C���x(degree C) */
			float water; 	/* �����C����(mbar) */
			float press; 	/* �n�� 1.5m �ɂ������C��(mbar) */
	       } F_METEO_PRM;

typedef struct	{	char			site_name[9];	/* �ϑ��n�� */
			F_METEO_PRM		meteo;		/* �C�ۃp�����[�^ */
			float			tau0;		/* ��C�̌��w�I���� */
			char			memo[65];	/* ���� */
			char			comment[65];	/* �R�����g */
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
