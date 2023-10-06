/*
 * ＶＲＡＤ計算入力構造体
 */
typedef	struct{
    char    cstime[24];    /* 観測時刻  YYYYMMDDhhmmss                      */

    char    ctrk_type[8];  /* 追尾種別  RADEC, LB, AZEL, SOLAR, COMET, HOLO */
    double  dsrc_pos[2];   /* 観測天体座標  [DEG]                           */
    char    csrc_name[256];/* 惑星名  MERCURY, VENUS, etc...                */
    char    cepoch[8];     /* 元期  B1950, J2000                            */
    char    cvref[8];      /* 視線速度の定義系 LSR, HEL, GAL                */

    char    ccomet[24];    /* 近日点通過時刻  YYYYMMDDhhmmss.x              */
    double  dcomet[5];     /* [0]: 近日点距離  [AU]                         */
                           /* [1]: 離心率                                   */
                           /* [2]: 近日点引数  [DEG]                        */
                           /* [3]: 昇降点黄経  [DEG]                        */
                           /* [4]: 軌道傾斜角  [DEG]                        */

    /**** 以下は未使用 ****/ 
    char    cscan_cood[8]; /* スキャン座標系  RADEC, LB, AZEL               */
    double  dscan_st[2];   /* スキャン開始オフセット  [DEG]                 */
    double  dscan_ed[2];   /* スキャン終了オフセット  [DEG]                 */
}XANTVL;
