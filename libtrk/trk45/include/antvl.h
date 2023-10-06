/*
 * �֣ң��ķ׻����Ϲ�¤��
 */
typedef	struct{
    char    cstime[24];    /* ��¬����  YYYYMMDDhhmmss                      */

    char    ctrk_type[8];  /* ��������  RADEC, LB, AZEL, SOLAR, COMET, HOLO */
    double  dsrc_pos[2];   /* ��¬ŷ�κ�ɸ  [DEG]                           */
    char    csrc_name[256];/* ����̾  MERCURY, VENUS, etc...                */
    char    cepoch[8];     /* ����  B1950, J2000                            */
    char    cvref[8];      /* ����®�٤������ LSR, HEL, GAL                */

    char    ccomet[24];    /* �������̲����  YYYYMMDDhhmmss.x              */
    double  dcomet[5];     /* [0]: ��������Υ  [AU]                         */
                           /* [1]: Υ��Ψ                                   */
                           /* [2]: ����������  [DEG]                        */
                           /* [3]: ����������  [DEG]                        */
                           /* [4]: ��ƻ���г�  [DEG]                        */

    /**** �ʲ���̤���� ****/ 
    char    cscan_cood[8]; /* ��������ɸ��  RADEC, LB, AZEL               */
    double  dscan_st[2];   /* ������󳫻ϥ��ե��å�  [DEG]                 */
    double  dscan_ed[2];   /* �������λ���ե��å�  [DEG]                 */
}XANTVL;
