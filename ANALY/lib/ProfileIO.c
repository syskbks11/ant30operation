#include	<time.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

#include	<profhead.h>
#include	<starlib.h>

int	profile_in(profile)
/*
profile from the pipeline
return
0 : normal
1 : garbage profile data
101 : too many history steps
102 : too many lines in a history step
111 : not "END" nor "end" for the last history step
999 : invalid format data

1992/12/19 T.Handa
*/
PROFILE	*profile;
{
	int	err_code,
		l_file_name;
	char	*ret_char;

/* get data idnetifier */
	ret_char=fgets(&((*profile).file_id[0]),10,stdin);
	if(ret_char==NULL)
	{
		(*profile).sys_head.l_file=0;
		return(1);
	}
	(*profile).file_id[8]=NULL;
	if(strcmp((*profile).file_id,"STAR.scn")!=0)
		return(999);

/* get file name */
	ret_char=fgets(&((*profile).file_name[0]),MAX_FILE_NAME_LENGTH,stdin);
	if(ret_char==NULL) return(999);
	l_file_name=strlen((*profile).file_name)-1;
	(*profile).file_name[l_file_name]=NULL;

/* get profile itself */
	err_code=profile_load(stdin,profile);

	return(err_code);
}

int	profile_out(profile)
/*
profile into the pipeline
return
0 : normal
1 : garbage profile data
999 : invalid format data

1992/12/19 T.Handa
*/
PROFILE	*profile;
{
	int	err_code;
	char	*ret_char;

/* put data idnetifier */
	if((*profile).sys_head.l_file<=0)
	{
		printf("\n");
		return(1);
	}
	if(strcmp((*profile).file_id,"STAR.scn")!=0)
	{
		printf("\n");
		return(999);
	}
	printf("%s\n",(*profile).file_id);

/* put file name */
	printf("%s\n",(*profile).file_name);

/* put profile itself */
	err_code=profile_save(stdout,profile);

	return(err_code);
}


int 	profile_load(file_p,profile)
/*
１つのプロファイルをファイルからロードする。

return
0 : normal
1 : garbage profile data
101 : too many history steps
102 : too many lines in a history step
111 : not "END" nor "end" for the last history step
999 : invalid format data


1992/2/20 T.Oka
1992/4/16 T.Handa UNIX MiniSTAR
1992/5/1  T.Handa garbage data treatment : return(1)
1992/8/19 T.Oka	  history step のループの訂正(hist_line-->hist_step)
1992/8/26 T.Handa structurize
1992/8/27 T.OKA		history step ループ "END" 認識の訂正
1992/12/7 T.Handa debug for ASTROS ver.1.31,1.32
1995/2/23 Tomo OKA	Scaling factor の付加。以後 sys_head.version=2。
*/
FILE		*file_p;	/* データ格納ファイルのポインタ */
PROFILE		*profile; 	/* プロファイル */
{
	unsigned int	ii;
	int		err_code; 	/* エラーコード */

	unsigned short	history_steps;

/* read STAR file system header */
	(*profile).sys_head.l_file=0L;
	err_code=read_sys_head(file_p,&((*profile).sys_head));
	if(err_code!=0)
	{
		fprintf(stderr,"invalid format\n");
		return(999);
	}

	if((*profile).sys_head.l_file==0L)
	{
		return(1);	/* garbage data */
	}
	history_steps=(unsigned short)(*profile).sys_head.hist_step;
	if(history_steps>MAX_HISTORY_STEP)
	{
		return(101);	/* too large history */
	}

	if((*profile).sys_head.version==1)	(*profile).rx_status.scale=1.0;

/* read scan file header */
	err_code=read_obs_prm(file_p,&((*profile).obs_prm));
	err_code=read_object(file_p,&((*profile).object));
	err_code=read_ant_status(file_p,&((*profile).ant_status));
	err_code=read_rx_status(file_p,&((*profile).rx_status),(*profile).sys_head.version);
	err_code=read_be_status(file_p,&((*profile).be_status));
	err_code=read_obs_log(file_p,&((*profile).obs_log));
/* readヒストリィー */
	for(ii=0;ii<=history_steps;ii++)
	{
		err_code=read_history(file_p,&((*profile).history[ii]));
		if(err_code!=0)
		{
			return(102);	/* too many lines in a history step */
		}
	}
/*	if(strncmp((*profile).history[history_steps].step,"END",3)!=0
	&& strncmp((*profile).history[history_steps].step,"end",3)!=0)
	{
		return(111);
	}
*/

/* readスプリアスビットマップ */
	err_code=read_spurious(file_p,(*profile).be_status.data_ch,(*profile).spurious);
/* readベースラインビットマップ */
	err_code=read_baseline(file_p,(*profile).be_status.data_ch,(*profile).baseline);
/* readプロファイルデータ */
	err_code=read_prof(file_p,(*profile).be_status.data_ch,(*profile).data);/* ASTROS 1.31,1.32のバグ対応 */
	if(strncmp((*profile).history[history_steps].step,"END",3)!=0
	&& strncmp((*profile).history[history_steps].step,"end",3)!=0)
	{
		strcpy((*profile).history[history_steps].step,"END");
		return(111);
	}
	if((*profile).history[history_steps].step[3]!=NULL)
	{
		(*profile).history[history_steps].step[3]=NULL;
	}

	return(0);
}

int prfhead_load(file_p,prfhead)
/*
プロファイル（.scnデータ）のヘッダ部分のみをロードする

return
0 : normal
1 : garbage profile data
101 : too many history steps
102 : too many lines in a history step
111 : not "END" nor "end" for the last history step
999 : invalid format data

1993/9/29 T.Handa
*/
FILE		*file_p;	/* データ格納ファイルのポインタ */
PRF_HEADER	*prfhead; 	/* プロファイルのヘッダ */
{
	unsigned int	ii;
	int		err_code; 	/* エラーコード */

	unsigned short	history_steps;

/* read STAR file system header */
	(*prfhead).sys_head.l_file=0L;
	err_code=read_sys_head(file_p,&((*prfhead).sys_head));
	if(err_code!=0)
	{
		fprintf(stderr,"invalid format\n");
		return(999);
	}

	if((*prfhead).sys_head.l_file==0L)
	{
		return(1);	/* garbage data */
	}
	history_steps=(unsigned short)(*prfhead).sys_head.hist_step;
	if(history_steps>MAX_HISTORY_STEP)
	{
		return(101);	/* too large history */
	}

	if((*prfhead).sys_head.version==1)	(*prfhead).rx_status.scale=1.0;

/* read scan file header */
	err_code=read_obs_prm(file_p,&((*prfhead).obs_prm));
	err_code=read_object(file_p,&((*prfhead).object));
	err_code=read_ant_status(file_p,&((*prfhead).ant_status));
	err_code=read_rx_status(file_p,&((*prfhead).rx_status),(*prfhead).sys_head.version);
	err_code=read_be_status(file_p,&((*prfhead).be_status));
	err_code=read_obs_log(file_p,&((*prfhead).obs_log));
/* readヒストリィー */
	for(ii=0;ii<=history_steps;ii++)
	{
		err_code=read_history(file_p,&((*prfhead).history[ii]));
		if(err_code!=0)
		{
			return(102);	/* too many lines in a history step */
		}
	}
/*	if(strncmp((*prfhead).history[history_steps].step,"END",3)!=0
	&& strncmp((*prfhead).history[history_steps].step,"end",3)!=0)
	{
		return(111);
	}
*/
	return(0);
}


int 	profile_save(file_p,profile)
/*
１つのプロファイルをファイルに格納する。

1992/2/20 T.Oka
1992/4/16 T.Handa UNIX MiniSTAR
1992/5/1  T.Handa darbage data treatment : return(1)
1992/8/19 T.Oka	  history step のループの訂正(hist_line-->hist_step)
1992/8/26 T.Handa structurize
1995/2/23 Tomo OKA	Scaling factor の付加。以後 sys_head.version=2。
*/
FILE		*file_p;	/* データ格納ファイルのポインタ */
PROFILE		*profile; 	/* プロファイル */
{
	unsigned int	ii;
	int		err_code; 	/* エラーコード */

	unsigned short	history_steps;

/* write STAR file system header */
	err_code=write_sys_head(file_p,&((*profile).sys_head));
	if(err_code!=0)
	{
		fprintf(stderr,"invalid format\n");
		return(999);
	}

	if((*profile).sys_head.l_file==0L)
	{
		return(1);	/* garbage data */
	}
	history_steps=(unsigned short)(*profile).sys_head.hist_step;
/* write scan file header */
	err_code=write_obs_prm(file_p,&((*profile).obs_prm));
	err_code=write_object(file_p,&((*profile).object));
	err_code=write_ant_status(file_p,&((*profile).ant_status));
	err_code=write_rx_status(file_p,&((*profile).rx_status),(*profile).sys_head.version);
	err_code=write_be_status(file_p,&((*profile).be_status));
	err_code=write_obs_log(file_p,&((*profile).obs_log));
/* ヒストリィーの書き込み */
	for(ii=0;ii<=history_steps;ii++)
	{
		err_code=write_history(file_p,&((*profile).history[ii]));
	}

/* スプリアスビットマップの書き込み */
	err_code=write_spurious(file_p,(*profile).be_status.data_ch,(*profile).spurious);
/* ベースラインビットマップの書き込み */
	err_code=write_baseline(file_p,(*profile).be_status.data_ch,(*profile).baseline);
/* プロファイルデータの書き込み */
	err_code=write_prof(file_p,(*profile).be_status.data_ch,(*profile).data);

	return(0);
}


/*------ part for profile_load and profile_save ------*/

int 	read_sys_head(fp,sys_head)
/*
	機能：　ファイルヘッダのシステムヘッダー部分を読み込む (1)．

	※　注意　	１）ファイルはバイナリモードでオープンされていること．　
1992/4/16 T.Handa for UNIX MiniSTAR
1992/9/19 T.Handa for UNIX STAR
*/
FILE		*fp;
SYS_HEAD	*sys_head;
{
	char	dummy[1],id_char[5];

/*
	fread(&id_char,sizeof(id_char[0]),5,fp);
	id_char[4]=NULL;
	if(strcmp(id_char,"STAR")!=0) rewind(fp);
*/
	fread(&(*sys_head).l_file,sizeof((*sys_head).l_file),1,fp);
	fread(&(*sys_head).l_head,sizeof((*sys_head).l_head),1,fp);
	fread(&(*sys_head).format_id,sizeof((*sys_head).format_id),1,fp);
	fread(&(*sys_head).version,sizeof((*sys_head).version),1,fp);
	fread(&(*sys_head).data_type,sizeof((*sys_head).data_type),1,fp);
	fread(&(*sys_head).hist_step,sizeof((*sys_head).hist_step),1,fp);
	fread(&(*sys_head).hist_line,sizeof((*sys_head).hist_line),1,fp);
	fread(&(*sys_head).scan,sizeof((*sys_head).scan),1,fp);
	fread(&(*sys_head).backup,sizeof((*sys_head).backup),1,fp);
	fread(&(*sys_head).num_scan,sizeof((*sys_head).num_scan),1,fp);
	fread(dummy,sizeof(dummy[0]),1,fp);

	return(0);
}


int 	write_sys_head(fp,sys_head)
/*
	機能：　ファイルヘッダのシステムヘッダー部分を書き出す (1)．

	※　注意　	１）ファイルはバイナリモードでオープンされていること．　
1992/4/16 T.Handa for UNIX MiniSTAR
1992/9/19 T.Handa for UNIX STAR
*/
FILE		*fp;
SYS_HEAD	*sys_head;
{
	static char	dummy[1]={0x00},id_char[]="STAR";

	if((*sys_head).version==1){
		(*sys_head).version=SCAN_FORMAT_VERSION;
		(*sys_head).l_file=(*sys_head).l_file+4;
		(*sys_head).l_head=(*sys_head).l_head+4;
	}
	fwrite(&(*sys_head).l_file,sizeof((*sys_head).l_file),1,fp);
	fwrite(&(*sys_head).l_head,sizeof((*sys_head).l_head),1,fp);
	fwrite(&(*sys_head).format_id,sizeof((*sys_head).format_id),1,fp);
	fwrite(&(*sys_head).version,sizeof((*sys_head).version),1,fp);
	fwrite(&(*sys_head).data_type,sizeof((*sys_head).data_type),1,fp);
	fwrite(&(*sys_head).hist_step,sizeof((*sys_head).hist_step),1,fp);
	fwrite(&(*sys_head).hist_line,sizeof((*sys_head).hist_line),1,fp);
	fwrite(&(*sys_head).scan,sizeof((*sys_head).scan),1,fp);
	fwrite(&(*sys_head).backup,sizeof((*sys_head).backup),1,fp);
	fwrite(&(*sys_head).num_scan,sizeof((*sys_head).num_scan),1,fp);
	fwrite(dummy,sizeof(dummy[0]),1,fp);

	return(0);
}


int 	read_obs_prm(fp,obs_prm)
/*
	機能：　ファイルヘッダの観測パラメータ部分を読み込む (2)．

	※　注意　	１）ファイルはバイナリモードでオープンされていること．　
1992/4/16 T.Handa for UNIX MiniSTAR
1992/9/19 T.Handa for UNIX STAR
1992/12/7 T.Handa 文字列データのc-string化のためのNULL追加
*/
FILE		*fp;
OBS_PRM		*obs_prm;
{
	char dummy[1];

	fread(&(*obs_prm).project[0],sizeof(char),16,fp);
	(*obs_prm).project[16]=NULL;
	fread(&(*obs_prm).scd_file[0],sizeof(char),8,fp);
	(*obs_prm).scd_file[8]=NULL;
	fread(&(*obs_prm).line_numb,sizeof((*obs_prm).line_numb),1,fp);
	fread(&(*obs_prm).sw_mode,sizeof(char),1,fp);
	fread(dummy,sizeof(dummy[0]),1,fp);
	fread(&(*obs_prm).mjd,sizeof((*obs_prm).mjd),1,fp);
	fread(&(*obs_prm).start,sizeof((*obs_prm).start),1,fp);
	fread(&(*obs_prm).stop,sizeof((*obs_prm).stop),1,fp);
	fread(&(*obs_prm).on_time,sizeof((*obs_prm).on_time),1,fp);
	fread(&(*obs_prm).off_time,sizeof((*obs_prm).off_time),1,fp);
	fread(&(*obs_prm).observer1[0],sizeof(char),8,fp);
	fread(&(*obs_prm).observer2[0],sizeof(char),8,fp);
	fread(&(*obs_prm).observer3[0],sizeof(char),8,fp);
	fread(&(*obs_prm).observer4[0],sizeof(char),8,fp);
	(*obs_prm).observer1[8]=NULL;
	(*obs_prm).observer2[8]=NULL;
	(*obs_prm).observer3[8]=NULL;
	(*obs_prm).observer4[8]=NULL;

	return(0);
}


int 	write_obs_prm(fp,obs_prm)
/*
	機能：　ファイルヘッダの観測パラメータ部分を書き出す (2)．

	※　注意　	１）ファイルはバイナリモードでオープンされていること．　
1992/4/16 T.Handa for UNIX MiniSTAR
*/
FILE		*fp;
OBS_PRM		*obs_prm;
{
	static char dummy[1]={0x00};

	fwrite(&(*obs_prm).project[0],sizeof(char),16,fp);
	fwrite(&(*obs_prm).scd_file[0],sizeof(char),8,fp);
	fwrite(&(*obs_prm).line_numb,sizeof((*obs_prm).line_numb),1,fp);
	fwrite(&(*obs_prm).sw_mode,sizeof(char),1,fp);
	fwrite(dummy,sizeof(dummy[0]),1,fp);
	fwrite(&(*obs_prm).mjd,sizeof((*obs_prm).mjd),1,fp);
	fwrite(&(*obs_prm).start,sizeof((*obs_prm).start),1,fp);
	fwrite(&(*obs_prm).stop,sizeof((*obs_prm).stop),1,fp);
	fwrite(&(*obs_prm).on_time,sizeof((*obs_prm).on_time),1,fp);
	fwrite(&(*obs_prm).off_time,sizeof((*obs_prm).off_time),1,fp);
	fwrite(&(*obs_prm).observer1[0],sizeof(char),8,fp);
	fwrite(&(*obs_prm).observer2[0],sizeof(char),8,fp);
	fwrite(&(*obs_prm).observer3[0],sizeof(char),8,fp);
	fwrite(&(*obs_prm).observer4[0],sizeof(char),8,fp);

	return(0);
}


int 	read_object(fp,object)
/*
	機能：　ファイルヘッダの観測対象表示部分を読み込む (3)．

	※　注意　１）ファイルはバイナリモードでオープンされていること．

1992/2/22 T.Oka
1992/4/16 T.Handa UNIX MiniSTAR
1992/9/19 T.Handa for UNIX STAR
1992/12/7 T.Handa 文字列データのc-string化のためのNULL追加
*/
FILE		*fp;
OBJECT		*object;
{
	char dummy[16];

	fread(&(*object).name[0],sizeof(char),15,fp);
	(*object).name[15]=NULL;
	fread(&(*object).cood_sys,sizeof((*object).cood_sys),1,fp);
	fread(&(*object).on_lb.x,sizeof((*object).on_lb.x),1,fp);
	fread(&(*object).on_lb.y,sizeof((*object).on_lb.y),1,fp);
	fread(&(*object).on_radec.x,sizeof((*object).on_radec.x),1,fp);
	fread(&(*object).on_radec.y,sizeof((*object).on_radec.y),1,fp);
	fread(&(*object).on_azel.x,sizeof((*object).on_azel.x),1,fp);
	fread(&(*object).on_azel.y,sizeof((*object).on_azel.y),1,fp);
	fread(&(*object).off_lb.x,sizeof((*object).off_lb.x),1,fp);
	fread(&(*object).off_lb.y,sizeof((*object).off_lb.y),1,fp);
	fread(&(*object).off_radec.x,sizeof((*object).off_radec.x),1,fp);
	fread(&(*object).off_radec.y,sizeof((*object).off_radec.y),1,fp);
	fread(&(*object).orig_xy.x,sizeof((*object).orig_xy.x),1,fp);
	fread(&(*object).orig_xy.y,sizeof((*object).orig_xy.y),1,fp);
	fread(&(*object).pa_xy,sizeof((*object).pa_xy),1,fp);
	fread(dummy,sizeof(dummy[0]),8,fp);
	fread(&(*object).on_xy.x,sizeof((*object).on_xy.x),1,fp);
	fread(&(*object).on_xy.y,sizeof((*object).on_xy.y),1,fp);
	fread(dummy,sizeof(dummy[0]),16,fp);

	return(0);	
}


int 	write_object(fp,object)
/*
	機能：　ファイルヘッダの観測対象表示部分を書き出す (3)．

	※　注意　	１）ファイルはバイナリモードでオープンされていること．　

1992/2/22 T.Oka
1992/4/16 T.Handa UNIX MiniSTAR
*/
FILE		*fp;
OBJECT		*object;
{
	static char dummy[16]= {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

	fwrite(&(*object).name[0],sizeof(char),15,fp);
	fwrite(&(*object).cood_sys,sizeof((*object).cood_sys),1,fp);
	fwrite(&(*object).on_lb.x,sizeof((*object).on_lb.x),1,fp);
	fwrite(&(*object).on_lb.y,sizeof((*object).on_lb.y),1,fp);
	fwrite(&(*object).on_radec.x,sizeof((*object).on_radec.x),1,fp);
	fwrite(&(*object).on_radec.y,sizeof((*object).on_radec.y),1,fp);
	fwrite(&(*object).on_azel.x,sizeof((*object).on_azel.x),1,fp);
	fwrite(&(*object).on_azel.y,sizeof((*object).on_azel.y),1,fp);
	fwrite(&(*object).off_lb.x,sizeof((*object).off_lb.x),1,fp);
	fwrite(&(*object).off_lb.y,sizeof((*object).off_lb.y),1,fp);
	fwrite(&(*object).off_radec.x,sizeof((*object).off_radec.x),1,fp);
	fwrite(&(*object).off_radec.y,sizeof((*object).off_radec.y),1,fp);
	fwrite(&(*object).orig_xy.x,sizeof((*object).orig_xy.x),1,fp);
	fwrite(&(*object).orig_xy.y,sizeof((*object).orig_xy.y),1,fp);
	fwrite(&(*object).pa_xy,sizeof((*object).pa_xy),1,fp);
	fwrite(dummy,sizeof(dummy[0]),8,fp);
	fwrite(&(*object).on_xy.x,sizeof((*object).on_xy.x),1,fp);
	fwrite(&(*object).on_xy.y,sizeof((*object).on_xy.y),1,fp);
	fwrite(dummy,sizeof(dummy[0]),16,fp);

	return(0);	
}


int 	read_ant_status(fp,ant_status)
/*
	機能：　ファイルヘッダのアンテナステータス部分を読み込む (4)．

	※　注意　１）ファイルはバイナリモードでオープンされていること．

1992/2/22　岡　朋治
1992/4/16 T.Handa UNIX MiniSTAR
1992/9/19 T.Handa for UNIX STAR
1992/12/7 T.Handa 文字列データのc-string化のためのNULL追加
*/
FILE		*fp;
ANT_STATUS	*ant_status;
{
	char dummy[8];

	fread(&(*ant_status).tele[0],sizeof(char),8,fp);
	(*ant_status).tele[8]=NULL;
	fread(&(*ant_status).ap_eff,sizeof((*ant_status).ap_eff),1,fp);
	fread(&(*ant_status).mb_eff,sizeof((*ant_status).mb_eff),1,fp);
	fread(&(*ant_status).fss_eff,sizeof((*ant_status).fss_eff),1,fp);
	fread(&(*ant_status).hpbw,sizeof((*ant_status).hpbw),1,fp);
	fread(dummy,sizeof(dummy[0]),8,fp);

	return(0);
}


int 	write_ant_status(fp,ant_status)
/*
	機能：　ファイルヘッダのアンテナステータス部分を書き出す (4)．

	※　注意　	１）ファイルはバイナリモードでオープンされていること．　

1992/2/22　岡　朋治
1992/4/16 T.Handa UNIX MiniSTAR
*/

FILE		*fp;
ANT_STATUS	*ant_status;
{
	static char	dummy[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

	fwrite(&(*ant_status).tele[0],sizeof(char),8,fp);
	fwrite(&(*ant_status).ap_eff,sizeof((*ant_status).ap_eff),1,fp);
	fwrite(&(*ant_status).mb_eff,sizeof((*ant_status).mb_eff),1,fp);
	fwrite(&(*ant_status).fss_eff,sizeof((*ant_status).fss_eff),1,fp);
	fwrite(&(*ant_status).hpbw,sizeof((*ant_status).hpbw),1,fp);
	fwrite(dummy,sizeof(dummy[0]),8,fp);

	return(0);
}


int 	read_rx_status(fp,rx_status,version)
/*
	機能：　ファイルヘッダの受信機ステータス部分を読み込む (5)．

	※　注意　	１）ファイルはバイナリモードでオープンされていること．　
1992/4/16 T.Handa UNIX MiniSTAR
1992/9/19 T.Handa for UNIX STAR
1992/12/7 T.Handa 文字列データのc-string化のためのNULL追加
*/
FILE		*fp;
RX_STATUS	*rx_status;
unsigned char	version;
{
	char dummy[4];

	fread(&(*rx_status).name[0],sizeof(char),7,fp);
	(*rx_status).name[7]=NULL;
	fread(&(*rx_status).sb_mode,sizeof((*rx_status).sb_mode),1,fp);
	fread(&(*rx_status).v_lsr,sizeof((*rx_status).v_lsr),1,fp);
	fread(&(*rx_status).rest_freq,sizeof((*rx_status).rest_freq),1,fp);
	fread(&(*rx_status).obs_freq,sizeof((*rx_status).obs_freq),1,fp);
	fread(&(*rx_status).lo_freq1,sizeof((*rx_status).lo_freq1),1,fp);
	fread(&(*rx_status).if_freq1,sizeof((*rx_status).if_freq1),1,fp);
	fread(&(*rx_status).lo_freq2,sizeof((*rx_status).lo_freq2),1,fp);
	fread(&(*rx_status).if_freq2,sizeof((*rx_status).if_freq2),1,fp);
	fread(&(*rx_status).lo_freq3,sizeof((*rx_status).lo_freq3),1,fp);
	fread(&(*rx_status).if_freq3,sizeof((*rx_status).if_freq3),1,fp);
	fread(&(*rx_status).mltplx1,sizeof((*rx_status).mltplx1),1,fp);
	fread(&(*rx_status).mltplx2,sizeof((*rx_status).mltplx2),1,fp);
	fread(&(*rx_status).mltplx3,sizeof((*rx_status).mltplx3),1,fp);
	fread(dummy,sizeof(dummy[0]),1,fp);
	fread(&(*rx_status).tsys,sizeof((*rx_status).tsys),1,fp);
	fread(&(*rx_status).trx,sizeof((*rx_status).trx),1,fp);
	fread(&(*rx_status).rms,sizeof((*rx_status).rms),1,fp);
	fread(dummy,sizeof(dummy[0]),4,fp);
	fread(&(*rx_status).calib,sizeof((*rx_status).calib),1,fp);
	fread(&(*rx_status).weight,sizeof((*rx_status).weight),1,fp);
	if(version>1){
	fread(&(*rx_status).scale,sizeof((*rx_status).scale),1,fp);
	}
	return(0);
}


int 	write_rx_status(fp,rx_status,version)
/*
	機能：　ファイルヘッダの受信機ステータス部分を書き出す (5)．

	※　注意　	１）ファイルはバイナリモードでオープンされていること．　

*/
FILE		*fp;
RX_STATUS	*rx_status;
unsigned char	version;
{
	static char dummy[4]={0x00,0x00,0x00,0x00};

	fwrite(&(*rx_status).name[0],sizeof(char),7,fp);
	fwrite(&(*rx_status).sb_mode,sizeof((*rx_status).sb_mode),1,fp);
	fwrite(&(*rx_status).v_lsr,sizeof((*rx_status).v_lsr),1,fp);
	fwrite(&(*rx_status).rest_freq,sizeof((*rx_status).rest_freq),1,fp);
	fwrite(&(*rx_status).obs_freq,sizeof((*rx_status).obs_freq),1,fp);
	fwrite(&(*rx_status).lo_freq1,sizeof((*rx_status).lo_freq1),1,fp);
	fwrite(&(*rx_status).if_freq1,sizeof((*rx_status).if_freq1),1,fp);
	fwrite(&(*rx_status).lo_freq2,sizeof((*rx_status).lo_freq2),1,fp);
	fwrite(&(*rx_status).if_freq2,sizeof((*rx_status).if_freq2),1,fp);
	fwrite(&(*rx_status).lo_freq3,sizeof((*rx_status).lo_freq3),1,fp);
	fwrite(&(*rx_status).if_freq3,sizeof((*rx_status).if_freq3),1,fp);
	fwrite(&(*rx_status).mltplx1,sizeof((*rx_status).mltplx1),1,fp);
	fwrite(&(*rx_status).mltplx2,sizeof((*rx_status).mltplx2),1,fp);
	fwrite(&(*rx_status).mltplx3,sizeof((*rx_status).mltplx3),1,fp);
	fwrite(dummy,sizeof(dummy[0]),1,fp);
	fwrite(&(*rx_status).tsys,sizeof((*rx_status).tsys),1,fp);
	fwrite(&(*rx_status).trx,sizeof((*rx_status).trx),1,fp);
	fwrite(&(*rx_status).rms,sizeof((*rx_status).rms),1,fp);
	fwrite(dummy,sizeof(dummy[0]),4,fp);
	fwrite(&(*rx_status).calib,sizeof((*rx_status).calib),1,fp);
	fwrite(&(*rx_status).weight,sizeof((*rx_status).weight),1,fp);
	if(version>1){
	fwrite(&(*rx_status).scale,sizeof((*rx_status).scale),1,fp);
	}
	return(0);
}


int 	read_be_status(fp,be_status)
/*
	機能：　ファイルヘッダのバックエンドステータス部分を読み込む (6)．

	※　注意　	１）ファイルはバイナリモードでオープンされていること．　
1991/4/16 T.Handa UNIX MiniSTAR
1992/9/19 T.Handa for UNIX STAR
1992/12/7 T.Handa 文字列データのc-string化のためのNULL追加
*/
FILE		*fp;
BE_STATUS	*be_status;
{
	char dummy[1];

	fread(&(*be_status).name[0],sizeof(char),8,fp);
	(*be_status).name[8]=NULL;
	fread(&(*be_status).be_ch,sizeof((*be_status).be_ch),1,fp);

	if((*be_status).be_ch==0)
		(*be_status).be_ch=MAX_CH; /* debug for ASTROS 1.11 */

	fread(&(*be_status).basis_ch,sizeof((*be_status).basis_ch),1,fp);
	fread(&(*be_status).sign,sizeof((*be_status).sign),1,fp);
	fread(dummy,sizeof(dummy[0]),1,fp);
	fread(&(*be_status).data_ch,sizeof((*be_status).data_ch),1,fp);

	if((*be_status).data_ch==0)
		(*be_status).data_ch=MAX_CH; /* debug for ASTROS 1.11 */

	fread(&(*be_status).center_freq,sizeof((*be_status).center_freq),1,fp);
	fread(&(*be_status).disp0,sizeof((*be_status).disp0),1,fp);
	fread(&(*be_status).disp1,sizeof((*be_status).disp1),1,fp);
	fread(&(*be_status).disp2,sizeof((*be_status).disp2),1,fp);
	fread(&(*be_status).disp3,sizeof((*be_status).disp3),1,fp);
	fread(&(*be_status).disp4,sizeof((*be_status).disp4),1,fp);
	fread(&(*be_status).freq_res,sizeof((*be_status).freq_res),1,fp);
	fread(&(*be_status).ch_width,sizeof((*be_status).ch_width),1,fp);
	fread(&(*be_status).ch_orig,sizeof((*be_status).ch_orig),1,fp);

	return(0);
}


int 	write_be_status(fp,be_status)
/*
	機能：　ファイルヘッダのバックエンドステータス部分を書き出す (6)．

	※　注意　	１）ファイルはバイナリモードでオープンされていること．　
1991/4/16 T.Handa UNIX MiniSTAR
*/
FILE		*fp;
BE_STATUS	*be_status;
{
	static char dummy=0x00;

	fwrite(&(*be_status).name[0],sizeof(char),8,fp);
	fwrite(&(*be_status).be_ch,sizeof((*be_status).be_ch),1,fp);
	fwrite(&(*be_status).basis_ch,sizeof((*be_status).basis_ch),1,fp);
	fwrite(&(*be_status).sign,sizeof((*be_status).sign),1,fp);
	fwrite(&dummy,sizeof(dummy),1,fp);
	fwrite(&(*be_status).data_ch,sizeof((*be_status).data_ch),1,fp);
	fwrite(&(*be_status).center_freq,sizeof((*be_status).center_freq),1,fp);
	fwrite(&(*be_status).disp0,sizeof((*be_status).disp0),1,fp);
	fwrite(&(*be_status).disp1,sizeof((*be_status).disp1),1,fp);
	fwrite(&(*be_status).disp2,sizeof((*be_status).disp2),1,fp);
	fwrite(&(*be_status).disp3,sizeof((*be_status).disp3),1,fp);
	fwrite(&(*be_status).disp4,sizeof((*be_status).disp4),1,fp);
	fwrite(&(*be_status).freq_res,sizeof((*be_status).freq_res),1,fp);
	fwrite(&(*be_status).ch_width,sizeof((*be_status).ch_width),1,fp);
	fwrite(&(*be_status).ch_orig,sizeof((*be_status).ch_orig),1,fp);

	return(0);
}


int 	read_obs_log(fp,obs_log)
/*
	機能：　ファイルヘッダの観測ログ部分を読み込む (4)．

	※　注意　	１）ファイルはバイナリモードでオープンされていること．　

1992/2/22　岡　朋治
1992/4/16 T.Handa UNIX MiniSTAR
1992/9/19 T.Handa for UNIX STAR
1992/12/7 T.Handa 文字列データのc-string化のためのNULL追加
*/
FILE		*fp;
OBS_LOG		*obs_log;
{
	char dummy[8];
	float tmp,press,water;

	fread(&(*obs_log).site_name[0],sizeof(char),8,fp);
	(*obs_log).site_name[8]=NULL;
	fread(&tmp,sizeof(tmp),1,fp);
	fread(&press,sizeof(press),1,fp);
	fread(&water,sizeof(water),1,fp);
	fread(&(*obs_log).tau0,sizeof((*obs_log).tau0),1,fp);
	fread(dummy,sizeof(dummy[0]),8,fp);
	fread(&(*obs_log).memo[0],sizeof(char),64,fp);
	(*obs_log).memo[64]=NULL;
	fread(&(*obs_log).comment[0],sizeof(char),64,fp);
	(*obs_log).comment[64]=NULL;

	(*obs_log).meteo.tmp=tmp;
	(*obs_log).meteo.press=press;
	(*obs_log).meteo.water=water;

	return(0);
}


int 	write_obs_log(fp,obs_log)
/*
	機能：　ファイルヘッダの観測ログ部分を書き出す (4)．

	※　注意　	１）ファイルはバイナリモードでオープンされていること．　

1992/2/22　岡　朋治
1992/4/16 T.Handa UNIX MiniSTAR
*/
FILE		*fp;
OBS_LOG		*obs_log;
{
	static char dummy[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	float tmp,press,water;

	tmp=(*obs_log).meteo.tmp;
	press=(*obs_log).meteo.press;
	water=(*obs_log).meteo.water;

	fwrite(&(*obs_log).site_name[0],sizeof(char),8,fp);
	fwrite(&tmp,sizeof(tmp),1,fp);
	fwrite(&press,sizeof(press),1,fp);
	fwrite(&water,sizeof(water),1,fp);
	fwrite(&(*obs_log).tau0,sizeof((*obs_log).tau0),1,fp);
	fwrite(dummy,sizeof(dummy[0]),8,fp);
	fwrite(&(*obs_log).memo[0],sizeof(char),64,fp);
	fwrite(&(*obs_log).comment[0],sizeof(char),64,fp);

	return(0);
}


int 	read_history(fp,history)
/*
	機能：　データファイルの処理ヒストリー部分を読み込む (8)．

	※　注意　	１）ファイルはバイナリモードでオープンされていること．　
return
0 : normal
102 : too many lines in a history step

1992/2/22	岡　朋治
1992/4/16 T.Handa UNIX MiniSTAR
1992/8/22 T.Handa check also by "END" keyword in the history
1992/9/19 T.Handa for UNIX STAR
1992/12/7 T.Handa 文字列データのc-string化のためのNULL追加
*/
FILE		*fp;
HISTORY		*history;
{
	unsigned int ii;
	char dummy[4];

	fread(&(*history).size,sizeof((*history).size),1,fp);
	fread(&(*history).step[0],sizeof(char),7,fp);
	(*history).step[7]=NULL;
	fread(&(*history).time,sizeof((*history).time),1,fp);
	fread(dummy,sizeof(dummy[0]),4,fp);

	if((*history).size > MAX_HISTORY_LINES_PER_STEP)
		return(102);

	for(ii=0;ii<(*history).size;ii++)
	{
		fread(&(*history).prm[ii][0],16,1,fp);
		(*history).prm[ii][16]=NULL;
	}

	return(0);
}


int 	write_history(fp,history)
/*
	機能：　データファイルの処理ヒストリー部分を書き出す (8)．

	※　注意　	１）ファイルはバイナリモードでオープンされていること．　

1992/2/22	岡　朋治
1992/4/16 T.Handa UNIX MiniSTAR
1992/8/22 T.Handa check also by "END" keyword in the history
*/
FILE		*fp;
HISTORY		*history;
{
	unsigned int ii;
	static char dummy[4]={0x00,0x00,0x00,0x00};

	fwrite(&(*history).size,sizeof((*history).size),1,fp);
	fwrite(&(*history).step[0],sizeof(char),7,fp);
	fwrite(&(*history).time,sizeof((*history).time),1,fp);
	fwrite(dummy,sizeof(dummy[0]),4,fp);

	for(ii=0;ii<(*history).size;ii++)
		fwrite(&(*history).prm[ii][0],16,1,fp);

	return(0);
}


int read_spurious(fp,data_ch,spurious)
/*
	機能：　ファイルヘッダのスプリアスビットフラグ部分を読み込む (4)．

	※　注意　	１）ファイルはバイナリモードでオープンされていること．　

1992/2/22　岡　朋治
1992/4/16 T.Handa UNIX MiniSTAR
*/
FILE 		*fp;
unsigned int	data_ch;
unsigned char	*spurious;
{
	unsigned int sp_ch;

	sp_ch=(data_ch-1)/(sizeof(unsigned char)*8)+1;
	fread(spurious,sizeof(unsigned char),sp_ch,fp);

	return(0);
}


int write_spurious(fp,data_ch,spurious)
/*
	機能：　ファイルヘッダのスプリアスビットフラグ部分を書き出す (4)．

	※　注意　	１）ファイルはバイナリモードでオープンされていること．　

1992/2/22　岡　朋治
1992/4/16 T.Handa UNIX MiniSTAR
*/
FILE 		*fp;
unsigned int	data_ch;
unsigned char	*spurious;
{
	unsigned int sp_ch;

	sp_ch=(data_ch-1)/(sizeof(unsigned char)*8)+1;
	fwrite(spurious,sizeof(unsigned char),sp_ch,fp);

	return(0);
}


int read_baseline(fp,data_ch,baseline)
/*
	機能：　ファイルヘッダのベースライン部分を読み込む (4)．

	※　注意　	１）ファイルはバイナリモードでオープンされていること．　

1992/2/22　岡　朋治
1992/4/16 T.Handa UNIX MiniSTAR
*/
FILE 		*fp;
unsigned int	data_ch;
unsigned char	*baseline;
{
	unsigned int sp_ch;

	sp_ch=(data_ch-1)/(sizeof(unsigned char)*8)+1;
	fread(baseline,sizeof(unsigned char),sp_ch,fp);

	return(0);
}


int write_baseline(fp,data_ch,baseline)
/*
	機能：　ファイルヘッダのベースライン部分を書き出す (4)．

	※　注意　	１）ファイルはバイナリモードでオープンされていること．　

1992/2/22　岡　朋治
1992/4/16 T.Handa UNIX MiniSTAR
*/
FILE 		*fp;
unsigned int	data_ch;
unsigned char	*baseline;
{
	unsigned int sp_ch;

	sp_ch=(data_ch-1)/(sizeof(unsigned char)*8)+1;
	fwrite(baseline,sizeof(unsigned char),sp_ch,fp);

	return(0);
}


int read_prof(fp,data_ch,profile)
/*
	機能：	profileデータを読み込む (4)．

	※　注意　	１）ファイルはバイナリモードでオープンされていること．　

1992/2/22　岡　朋治
1992/4/16 T.Handa UNIX MiniSTAR
*/
FILE 		*fp;
unsigned int 	data_ch;
float		profile[];
{
	fread(profile,sizeof(profile[0]),data_ch,fp);

	return(0);
}


int write_prof(fp,data_ch,profile)
/*
	機能：	profileデータを書き出す (4)．

	※　注意　	１）ファイルはバイナリモードでオープンされていること．　

1992/2/22　岡　朋治
1992/4/16 T.Handa UNIX MiniSTAR
*/
FILE 		*fp;
unsigned int 	data_ch;
float		profile[];
{
	fwrite(profile,sizeof(profile[0]),data_ch,fp);

	return(0);
}
