	/* ���դ�Ĺ�� */
#define		LDATE	32
	/* ������Ĺ��[sec] */
#define		DAYSEC	86400.0
	/* J2000�����Ǥ�MJD[day] */
#define		D_J2000	51544.5
	/* �ʣӣԤλ��ﺹ[sec] */
#define		MJD_JST	32400.0
	/* �ͣʣĤ��饫�����ǡ��ؤ��Ѵ������ϰ�[day] */
#define		MJDMAX	51544.0		/* 2000/1/1 */
#define		MJDMIN	15110.0		/* 1900/4/1 */
	/* �ԣɣͣťե�����쥳����Ĺ */
#define		RECSIZE_TIME	43
#define		RECSIZE_EPH	707
	/* ����ƥʿ� */
#define		IPB		6

	/* �����ǡ����κ����Ǽ�� (CMTMAX) �ȳ�Ǽ�ѥơ��֥� */
#define		CMTMAX		86402
struct {
  int  index;			/* ��������Ƭ index */
  int  imax;			/* ���������ǡ����� */
  double ldmjd[CMTMAX];		/* MJD [sec] */
  double ldrad[CMTMAX][2];	/* ŷ�κ�ɸ (RADEC) [2] [rad] */
  double lddkm[CMTMAX];		/* �Ͽ������ŷ�Τε�Υ [m] */
} t_cmt;
