/* �գ֣׷׻����Ϲ�¤�� */
typedef	struct{
	double	dtime[2];	/* ��¬���� */
	double	dobsp[2];	/* ��¬ŷ�ΰ��� */
	double	dantr[3];	/* ���ɥ���ƥʰ��֥٥��ȥ� */
	double	danto[3];	/* ��¬�ɥ���ƥʰ��֥٥��ȥ� */
	double	dantv[9];	/* ��¬�ɥ���ƥʴﺹ������ */
	double	dpofst[2];	/* �ݥ���ƥ��󥰥��ե��å� */
				/* [0]: ���̳�/�ַ�/���[rad] */
				/* [1]: �ĳ�/�ְ�/����[rad] */
	double	dscnst[2];	/* ������󳫻ϰ���[rad] */
				/* [0]: ���̳�/�ַ�/���[rad] */
				/* [1]: �ĳ�/�ְ�/����[rad] */
	double	dscned[2];	/* �������λ����[rad] */
				/* [0]: ���̳�/�ַ�/���[rad] */
				/* [1]: �ĳ�/�ְ�/����[rad] */
	double	dobsfq;		/* ��¬���ȿ�[Hz] */
	double	dobsrv;		/* ��¬ŷ�λ���®��[m/s] */
	int	ioflg;		/* ŷ�Υե饰 */
	int	iaflg;		/* ŷ�ΰ��ֳѥե饰 */
	int	ibflg;		/* ŷ�ΰ���ʬ���ե饰 */
	int	itnscn;		/* �������˥����� */
				/* ���ʾ������ */
	int	inoscn;		/* �������˥��� */
				/* ���� �������˥��� �� �������˥����� */
	int	irpflg;		/* �ݥ���ƥ��󥰼³ѥե饰 */
				/* �������� */
				/* �����³� */
	int	iapflg;		/* �ݥ���ƥ��󥰳ѥե饰 */
				/* �����ң��ģţ� */
				/* ������ж�� */
				/* �������ڣţ� */
	int	irsflg;		/* �������˥󥰼³ѥե饰 */
				/* �������� */
				/* �����³� */
	int	iasflg;		/* �������˥󥰳ѥե饰 */
				/* �����ң��ģţ� */
				/* ������ж�� */
				/* �������ڣţ� */
	int	inlocl;		/* ����������ȯ�������ܼ��� */
	int	inflck;		/* �ե롼�����󥻥��������ܼ��� */
}XUVWI;

/* �����׻����Ϲ�¤�� */
typedef	struct{
	double	dflkfq;		/* �������ȿ�[Hz] */
	double	duvw[3];	/* �գ֣� */
				/* [0]: �� */
				/* [1]: �� */
				/* [2]: �� */
	double	dhoura;		/* ������󥰥�[rad] */
}XUVWO;