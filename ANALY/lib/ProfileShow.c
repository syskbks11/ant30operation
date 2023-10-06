#define	DEBUG	0
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <profhead.h>
#include <starlib.h>
#include <starXlib.h>

/*	�v���t�@�C�����v���b�g �֘A���C�u�����B

1992/12/21	T.Handa ver.1.01	font�̕ύX
1992/12/21	T.Handa ver.1.02	change filename display
1992/12/31	T.Handa ver.1.03	ch�ԍ��̒���
1993/ 1/ 8	T.Handa ver.1.04	velocity plot
1993/ 5/29	T.Handa ver.1.05	0 level�̐�����
1994/ 8/29	T.OKA   ver.2.01	profile_plot2, general_plot�̒ǉ�
*/


#define	NUM_FONT	"-*-helvetica-*-r-normal--12-*-*-*-p-70-iso8859-1"
			/*"helvetica12" "vri-25"*/
#define	CHAR_FONT	"-*-helvetica-*-r-normal--12-*-*-*-p-70-iso8859-1"
			/*"helvetica12" "vri-25"*/	
#define LABEL_HEIGHT	12
#define LABEL_WIDTH	8

int	profile_plot(dis,win,box_size,orig_pos,x_unit,x_range,T_range,
		profile,line_color,label_color)

/*	�v���t�@�C�����v���b�g����	

1992/8/25	T.OKA
1992/8/27	T.OKA	�\���̂̕ύX
1992/8/31	T.OKA	�������̕ύX
1992/9/6  T.Handa profile_plot=�֐��^�̕ύX�A�I�v�V�����̒ǉ�
1992/12/21 T.Handa	font�̕ύX
1992/12/31 T.Handa ver.1.03 ch�ԍ��̒���
1993/ 1/ 8 T.Handa ver.1.04 velocity plot
*/
Display	*dis;		/*�f�B�X�v���C*/
Window	win;		/*�E�B���h�E*/
XY_SIZE	box_size;	/*�{�b�N�X�̑傫���i�E�B���h�E�ɑ΂����*/
XY_POS	orig_pos;	/*�{�b�N�X�̌��_�̈ʒu�i�E�B���h�E�ɑ΂����*/
int	x_unit;		/*�\���͈͒P�ʎw��*/
RANGE	x_range,	/*�\�������͈�*/
	T_range;	/*�\�����x�͈�*/	
PROFILE	*profile;	/*�v���t�@�C���f�[�^*/
char	*line_color;	/*�v���t�@�C���\���F*/
char	*label_color;	/*���x���\���F*/

{
Window	root;
GC	gc;
Font	font;
Status	status;

int		x,y;
unsigned int	w_height, w_width;	/* �E�B���h�E�̑傫�� */
unsigned int	border, depth;		/* */
unsigned int	b_height, b_width;	/* �{�b�N�X�̑傫�� */
unsigned int	b_orig_x, b_orig_y;	/* �{�b�N�X�̌��_�̈ʒu */
BOX_RANGE	real_box;	/* �����P�ʂł̘g�͈� */
BOX_RANGE_long	pixel_box;	/* pixel�P�ʂł̘g�͈� */

int	gauge;

unsigned int	ch;

unsigned short	err_code;
int	i,j;
double	mean,rms;
double	T;
double	step;

double	tick_step_x,label_step_x;
double	tick_step_y,label_step_y;

RANGE_long	ch_range;
RANGE	x_tick_range,y_tick_range;
int	n_tick_step_x,n_label_step_x;
int	n_tick_step_y,n_label_step_y;
XY_POS		plot_pos;
XY_POS_long	plot_pix,prev_pix;
XY_SIZE_long	box_size_in_pixel;

char	string[MAX_FILE_NAME_LENGTH];
static char	ext[]=".scn";

err_code=0;

/* set range along the x-axis in the proper physical unit */
if(x_unit==X_CHANNEL){
	ch_range.min=(long)x_range.min; ch_range.max=(long)x_range.max;
}
else{
	ch_range.min=(long)STAR_VelocityCh(profile,x_range.min);
	ch_range.max=(long)(STAR_VelocityCh(profile,x_range.max)+0.9999);
}
/* �p�����[�^�̃`�F�b�N */
STAR_SortRangeLong(&ch_range);
if(ch_range.min==ch_range.max)
		err_code=11;

STAR_SortRange(&T_range);
if(T_range.min==T_range.max)
		err_code=21;

if(ch_range.min<1) ch_range.min=1;
if(ch_range.max>(*profile).be_status.data_ch)
	ch_range.max=(*profile).be_status.data_ch;
/* �E�B���h�D�̑傫�������߂� */
status=XGetGeometry(dis,win,&root,&x,&y,&w_width,&w_height,&border,&depth);

pixel_box.x.min=(long)((double)w_width*orig_pos.x);
pixel_box.x.max=(long)((double)w_width*(orig_pos.x+box_size.dx));
pixel_box.y.min=(long)((double)w_height*orig_pos.y);
pixel_box.y.max=(long)((double)w_height*(orig_pos.y+box_size.dy));
box_size_in_pixel.dx=pixel_box.x.max-pixel_box.x.min;
box_size_in_pixel.dy=pixel_box.y.max-pixel_box.y.min;

STAR_AutoScale(x_range,&x_tick_range,&n_tick_step_x,&tick_step_x);
STAR_AutoScale(T_range,&y_tick_range,&n_tick_step_y,&tick_step_y);
real_box.x.min=x_range.min;
real_box.x.max=x_range.max;
real_box.y.min=y_tick_range.min;
real_box.y.max=y_tick_range.max;

STAR_SetPixelScale(real_box,pixel_box);

gc=XCreateGC(dis,win,0,0);
font=XLoadFont(dis,CHAR_FONT);
XSetFont(dis,gc,font);
XFlush(dis);

/* �g�̕\�� */
XSetForeground(dis,gc,MyColor(dis,label_color));
XDrawRectangle(dis,win,gc,pixel_box.x.min,pixel_box.y.min,
	box_size_in_pixel.dx,box_size_in_pixel.dy);
XFlush(dis);

/* �O�j���x���̃v���b�g */
if(y_tick_range.min<0.0 && y_tick_range.max>0.0){
	plot_pos.x=1.0; plot_pos.y=0.0;
	STAR_PositionPixel(plot_pos,&plot_pix);
	XDrawLine(dis,win,gc,pixel_box.x.min,plot_pix.y,pixel_box.x.max,
			plot_pix.y);
}
XFlush(dis);

/* �X�L�������̕\�� */
XDrawString(dis,win,gc,pixel_box.x.min+box_size_in_pixel.dx/40,
	pixel_box.y.min+box_size_in_pixel.dy/40+20
	,(*profile).file_name,strlen((*profile).file_name) );

XFlush(dis);

/* x-axis name */
if(x_unit==X_CHANNEL){
	XDrawString(dis,win,gc
	,(pixel_box.x.min+pixel_box.x.max)/2-LABEL_HEIGHT*11/4
	,pixel_box.y.max+LABEL_HEIGHT*2+10
	,"channel (ch)",12 );
}
else if(x_unit==X_VELOCITY){
	XDrawString(dis,win,gc
	,(pixel_box.x.min+pixel_box.x.max)/2-LABEL_HEIGHT*19/4
	,pixel_box.y.max+LABEL_HEIGHT*2+10
	,"LSR velocity (km/s)",19 );
}
XFlush(dis);


/* x-axis���x���ƃe�B�b�N�}�[�N�̕\�� */
font=XLoadFont(dis,NUM_FONT);
XSetFont(dis,gc,font);

n_label_step_x=n_tick_step_x/2;
label_step_x=tick_step_x*2.0;
gauge=(int)((double)box_size_in_pixel.dy*0.04);
for(i=0;i<n_label_step_x;i++)
{
	plot_pos.x=x_tick_range.min+label_step_x*i;
	plot_pos.y=0.0;
	STAR_PositionPixel(plot_pos,&plot_pix);
	if((real_box.x.min<=plot_pos.x)&&(plot_pos.x<=real_box.x.max))
	{
		sprintf(string,"%d",(int)plot_pos.x);
		XDrawLine(dis,win,gc,plot_pix.x,pixel_box.y.min,plot_pix.x,
			pixel_box.y.min+gauge);
		XDrawLine(dis,win,gc,plot_pix.x,pixel_box.y.max,plot_pix.x,
			pixel_box.y.max-gauge);
		XDrawString(dis,win,gc,plot_pix.x-strlen(string)*6,
			pixel_box.y.max+LABEL_HEIGHT+5,
			string,strlen(string));
	}
}
XFlush(dis);

/* x-axis���ԃe�B�b�N�}�[�N�̕\�� */
gauge=gauge/2;
for(i=0;i<n_tick_step_x;i++)
{
	plot_pos.x=x_tick_range.min+tick_step_x*i;
	STAR_PositionPixel(plot_pos,&plot_pix);
	if((real_box.x.min<=plot_pos.x)&&(plot_pos.x<=real_box.x.max))
	{
		XDrawLine(dis,win,gc,plot_pix.x,pixel_box.y.min,
			plot_pix.x,pixel_box.y.min+gauge);
		XDrawLine(dis,win,gc,plot_pix.x,pixel_box.y.max,
			plot_pix.x,pixel_box.y.max-gauge);
	}
}
XFlush(dis);

/* ���x�����x���ƃe�B�b�N�}�[�N�̕\�� */
n_label_step_y=n_tick_step_y/2;
label_step_y=tick_step_y*2.0;

gauge=(int)((double)box_size_in_pixel.dx*0.04);
for(i=0;i<n_label_step_y;i++)
{
	plot_pos.y=y_tick_range.min+label_step_y*i;
	STAR_PositionPixel(plot_pos,&plot_pix);
	if((real_box.y.min<=plot_pos.y)&&(plot_pos.y<=real_box.y.max))
	{
		XDrawLine(dis,win,gc,pixel_box.x.min,plot_pix.y,
			pixel_box.x.min+gauge,plot_pix.y);
		XDrawLine(dis,win,gc,pixel_box.x.max,plot_pix.y,
			pixel_box.x.max-gauge,plot_pix.y);
		if(label_step_y>1.0)
			sprintf(string,"%d",(int)plot_pos.y);
		else
			sprintf(string,"%.3f",plot_pos.y);
		XDrawString(dis,win,gc
		,pixel_box.x.min-strlen(string)*LABEL_WIDTH
		-(int)(LABEL_WIDTH*0.5),plot_pix.y+LABEL_HEIGHT/2
		,string,strlen(string));
	}
}
XFlush(dis);

/* ���x�����ԃe�B�b�N�}�[�N�̕\�� */
gauge=gauge/2;
for(i=0;i<n_tick_step_y;i++)
{
	plot_pos.y=y_tick_range.min+tick_step_y*i;
	STAR_PositionPixel(plot_pos,&plot_pix);
	if((real_box.y.min<=plot_pos.y)&&(plot_pos.y<=real_box.y.max))
	{
		XDrawLine(dis,win,gc,pixel_box.x.min,plot_pix.y,
			pixel_box.x.min+gauge,plot_pix.y);
		XDrawLine(dis,win,gc,pixel_box.x.max,plot_pix.y,
			pixel_box.x.max-gauge,plot_pix.y);
	}
}
XFlush(dis);
		
/* �v���t�@�C���f�[�^�̕\�� */	
XSetForeground(dis,gc,MyColor(dis,line_color));
     if(x_unit==X_CHANNEL){
       plot_pos.x=ch_range.min;
     }
     else{
       plot_pos.x=STAR_ChVelocity(profile,(double)ch_range.min);
     }
     plot_pos.y=(*profile).data[ch_range.min-1]*(*profile).rx_status.scale;
     STAR_PositionPixel(plot_pos,&prev_pix);
     {
       /* �`�摬�x�������邽�߂� X-Window ���ɓn���f�[�^�ʂ�
	  �����������Ɍ��炷���߂ɃR�[�h�������ύX
	  1993/9/26 J.Morino                                 */

       int min_y,max_y;
       int same_x;  /*�O�񓯂�x�̒l���Ƃ����Ƃ� 1 ����ȊO 0 */
       same_x=0;
       for(ch=ch_range.min;ch<=ch_range.max;ch++)
	 {
	   if(x_unit==X_CHANNEL){
	     plot_pos.x=ch;
	   }
	   else{
	     plot_pos.x=STAR_ChVelocity(profile,(double)ch);
	   }
	   plot_pos.y=(*profile).data[ch-1]*(*profile).rx_status.scale;	/* data��ch1���琔���� */
	   STAR_PositionPixel(plot_pos,&plot_pix);

	   /* X-Window��łЂƂ̕\�������Ƃ��ɉ������Ƀs�N�Z������
	      �[���łȂ��Ƃ������̂P�s�N�Z���ɑ΂��Đ�ch�̃f�[�^���d
	      �Ȃ�B���̂悤�ȏꍇ�ɏd�Ȃ��Ă�����̂ɑ΂��čő�l��
	      �ŏ��l�݂̂�X-Window���ɓn���Ώd�ˏ�������Ԃ�̎d����
	      ����A�`�摬�x���オ��A�܂��l�b�g���[�N���o�R����ꍇ
	      ���̕��S���ւ�B
	      �����ł͐��������Ƃ��ɂ������Ƀs�N�Z�����ς��Ȃ��ꍇ
	      same_x ���P�ɃZ�b�g���ő�l�ƍŏ��l���L������B
	      �������Ƀs�N�Z�����ς�����ꍇ�A�������ɂ��̐���������
	      �Ƃ��ɁA���Ƃ̂����W�ōő�l�ƍŏ��l���Ȃ��d�ˏ�����
	      ���̂Ƃ��Ȃ���Ԃɂ���B
	      ���ׂĂ�ch���������I��������same_x  ���P�ɃZ�b�g�����
	      �����ꍇ���Ƃ̂����W�ōő�l�ƍŏ��l���Ȃ��A���ׂĂ�
	      �����������Ƃ���B

	      ��
			     * 
			     ** <-- max_y
		   ^	      **
		   |y	      ** ....... * �̓f�[�^
      		   |	      **
		   |	      *
		   +----> x   *  <-- min_y

			      ^----- ���̗�ɂ���              */


	       /* �O��Ɠ��l�ɈقȂ�x��PLOT */ 
	   if((same_x==0)&&(prev_pix.x!=plot_pix.x)){
	     XDrawLine(dis,win,gc,prev_pix.x,prev_pix.y,plot_pix.x,
		       plot_pix.y);
	   }
	   /* �O��PLOT�����Ƃ��Ɠ���x�ł������Ƃ�(2 ��ڈȏ�) */
	   if((same_x==1)&&(prev_pix.x==plot_pix.x)){
	     if(min_y>plot_pix.y){min_y=plot_pix.y;}
	     if(max_y<plot_pix.y){max_y=plot_pix.y;}
	   }
	   /* �O��PLOT�����Ƃ��Ɠ���x�ł������Ƃ�(�P���) */
	   if((same_x==0)&&(prev_pix.x==plot_pix.x)){
	     same_x=1;
	     if (prev_pix.y<plot_pix.y){
	       min_y=prev_pix.y;max_y=plot_pix.y;}
	     else{
	       max_y=prev_pix.y;min_y=plot_pix.y;}
	   }
	   /* �O��͓���x�̒l��PLOT�����ɂ��č���قȂ�x�̒l��������ꍇ*/
	   if((same_x==1)&&(prev_pix.x!=plot_pix.x)){
	     same_x=0;
	     XDrawLine(dis,win,gc,prev_pix.x,prev_pix.y,plot_pix.x,
		       plot_pix.y);
	     XDrawLine(dis,win,gc,prev_pix.x,min_y     ,prev_pix.x,max_y);
	   }
	   prev_pix.x=plot_pix.x;
	   prev_pix.y=plot_pix.y;
	 }
       /* ���ׂĂ�ch���ɂ��ďI�������̏��� */
       if(same_x==1){
	 XDrawLine(dis,win,gc,prev_pix.x,min_y,prev_pix.x,max_y);
       }
     }
XFlush(dis);
return(err_code);
}
/*-----------------------*/

void STAR_AutoScale(orig_range,ceil_range,n_step,step)
/*
���͈̔͂��܂ލŏ��̐؂�̗ǂ����l�͈͂����߂�
1992/9/7 T.Handa
*/
RANGE	orig_range,	/* ���͈̔� */
	*ceil_range;	/* �ۂ߂��͈� */
int	*n_step;	/* �ڐ��̐� */
double	*step;		/* �ڐ��̊Ԋu */
{
	double	span;

	span=orig_range.max-orig_range.min;

	*step=pow(10.0, floor( log10(span) ))/10.0;
	*n_step=floor(span/(*step));

	if((*n_step)>20)
	{ *step=(*step)*5.0; *n_step=floor(span/(*step)); }
	if((*n_step)>10)
	{ *step=(*step)*2.0; *n_step=floor(span/(*step)); }

	(*ceil_range).min=floor(orig_range.min/(*step)) * (*step);
	(*ceil_range).max=ceil(orig_range.max/(*step)) * (*step);

	*n_step=floor(((*ceil_range).max-(*ceil_range).min)/(*step))+1;	
}

/* ���W���Z�@�w�E�B���h�D��ł̍��W�ʒu�����߂� */
static XY_POS		real_origin;
static XY_POS_long	pixel_origin;
static XY_SIZE		real_span;
static XY_SIZE_long	pixel_span;

void STAR_SetPixelScale(rel_box,pix_box)
/*
���W�n���`����
1992/9/8 T.Handa
*/
BOX_RANGE	rel_box;	/* �����ʍ��W�ł̂S���̒l */
BOX_RANGE_long	pix_box;	/* pixel�l�ł̂S���̒l */
{
	real_span.dx=rel_box.x.max-rel_box.x.min;
	real_span.dy=rel_box.y.max-rel_box.y.min;

	real_origin.x=rel_box.x.min;
	real_origin.y=rel_box.y.min;

	pixel_span.dx=pix_box.x.max-pix_box.x.min;
	pixel_span.dy=pix_box.y.min-pix_box.y.max;	/* pixel�l�����Ɖ�ʏ���Ƃ̐��� */

	pixel_origin.x=pix_box.x.min;
	pixel_origin.y=pix_box.y.max;	/* pixel�l�����Ɖ�ʏ���Ƃ̐��� */
}
void STAR_PixelPosition(pix_pos,rel_pos)
/*
pixel�l���畨���ʒl�����߂�
1992/9/8 T.Handa
*/
XY_POS_long	pix_pos;	/* pixel�ʒu */
XY_POS		*rel_pos;	/* ������ */
{
	XY_POS	scale;

	scale.x=real_span.dx/((double)pixel_span.dx);
	scale.y=real_span.dy/((double)pixel_span.dy);

	(*rel_pos).x=((double)(pix_pos.x-pixel_origin.x)*scale.x)+real_origin.x;
	(*rel_pos).y=((double)(pix_pos.y-pixel_origin.y)*scale.y)+real_origin.y;
}
void STAR_PositionPixel(rel_pos,pix_pos)
/*
�����ʒl����pixel�l�����߂�
1992/9/8 T.Handa
*/
XY_POS		rel_pos;	/* ������ */
XY_POS_long	*pix_pos;	/* pixel�ʒu */
{
	XY_POS	scale;

	scale.x=((double)pixel_span.dx)/real_span.dx;
	scale.y=((double)pixel_span.dy)/real_span.dy;

	(*pix_pos).x=(long)((rel_pos.x-real_origin.x)*scale.x)+pixel_origin.x;
	(*pix_pos).y=(long)((rel_pos.y-real_origin.y)*scale.y)+pixel_origin.y;
}

unsigned long MyColor(display,color)
/*
�J���[�p���b�g���m�ۂ���
1992/9/7 T.Handa
*/
Display *display;
char *color;
{
        Colormap cmap;
        XColor c0,c1;

        cmap=DefaultColormap(display,0);

        XAllocNamedColor(display,cmap,color,&c1,&c0);

        return(c1.pixel);
}




/********************************/
/*	Postscript Output	*/
/********************************/


void	PSInitialize(fp, orig, scale, angle)
FILE	*fp;		/*	�o�̓t�@�C���̃t�@�C���|�C���^	*/
XY_POS	*orig;		/*	���[�U��ԏ�ł̌��_�̈ʒu	*/
XY_POS	*scale;		/*	���W�l�̔{��	*/
double	angle;		/*	���W�̉�]�p�x	*/
{
fprintf(fp,"%%!\n");
fprintf(fp,"%%\n");
fprintf(fp,"%%\tThis is a Postscript output file made by STAR system.\n");
fprintf(fp,"%%\n");
fprintf(fp,"%%\n");
fprintf(fp,"\n");
fprintf(fp,"%lf\t%lf\t%s\n",(*orig).x,(*orig).y,"translate");
fprintf(fp,"%lf\t%lf\t%s\n",(*scale).x,(*scale).y,"scale");
fprintf(fp,"%lf\t%s\n",angle,"rotate");
fprintf(fp,"\n");
}


void	PSDrawLine(fp, xy1, xy2, width)
FILE	*fp;		/*	�o�̓t�@�C���̃t�@�C���|�C���^	*/
XY_POS	*xy1;		/*	���[�U��ԏ�ł̎n�_�̍��W�l	*/
XY_POS	*xy2;		/*	���[�U��ԏ�ł̏I�_�̍��W�l	*/
double	width;		/*	�`�悷�����	*/
{
fprintf(fp,"%s\n","newpath");
fprintf(fp,"%lf\t%lf\t%s\n",(*xy1).x,(*xy1).y,"moveto");
fprintf(fp,"%lf\t%lf\t%s\n",(*xy2).x,(*xy2).y,"lineto");
fprintf(fp,"%lf\t%s\n",width,"setlinewidth");
fprintf(fp,"%s\n","stroke");
fprintf(fp,"\n");
}


void	PSDrawRectangle(fp, xy1, xy2, width)
FILE	*fp;		/*	�o�̓t�@�C���̃t�@�C���|�C���^	*/
XY_POS	*xy1;		/*	���[�U��ԏ�ł̍����p�̍��W�l	*/
XY_POS	*xy2;		/*	���[�U��ԏ�ł̉E��p�̍��W�l	*/
double	width;		/*	�`�悷�����	*/
{
fprintf(fp,"%s\n","newpath");
fprintf(fp,"%lf\t%lf\t%s\n",(*xy1).x,(*xy1).y,"moveto");
fprintf(fp,"%lf\t%lf\t%s\n",(*xy2).x,(*xy1).y,"lineto");
fprintf(fp,"%lf\t%lf\t%s\n",(*xy2).x,(*xy2).y,"lineto");
fprintf(fp,"%lf\t%lf\t%s\n",(*xy1).x,(*xy2).y,"lineto");
fprintf(fp,"%s\n","closepath");
fprintf(fp,"%lf\t%s\n",width,"setlinewidth");
fprintf(fp,"%s\n","stroke");
fprintf(fp,"\n");
}

void	PSDrawString(fp, xy, text, font, size)
FILE	*fp;		/*	�o�̓t�@�C���̃t�@�C���|�C���^	*/
XY_POS	*xy;		/*	���[�U��ԏ�ł̕����̍��W�l	*/
char	text[];		/*	�������镶��		*/
char	font[];		/*	�g�p�����t�H���g	*/
short	size;		/*	�����̃T�C�Y	*/
{
fprintf(fp,"%s\t%s\n",font,"findfont");
fprintf(fp,"%d\t%s\n",size,"scalefont");
fprintf(fp,"%s\n","setfont");
fprintf(fp,"%lf\t%lf\t%s\n",(*xy).x,(*xy).y,"moveto");
fprintf(fp,"(%s)\t%s\n",text,"show");
fprintf(fp,"\n");
}



void	PSFlush(fp)
FILE	*fp;
{
fprintf(fp,"showpage\n");
fprintf(fp,"%%\tEnd of STAR Postscript output.\n");

}




int	general_plot(dis,win,box_size,orig_pos,x_unit_char,x_range,y_range,
		x_data,y_data,data_num,line_color,label_color,title)

/*	��ʂ̊֐����v���b�g����	

1994/8/26	T.OKA

*/

Display	*dis;		/*�f�B�X�v���C*/
Window	win;		/*�E�B���h�E*/
XY_SIZE	box_size;	/*�{�b�N�X�̑傫���i�E�B���h�E�ɑ΂����*/
XY_POS	orig_pos;	/*�{�b�N�X�̌��_�̈ʒu�i�E�B���h�E�ɑ΂����*/
char	x_unit_char[];		/*�\���͈͒P�ʎw��*/
RANGE	x_range,	/*�\�������͈�*/
	y_range;	/*�\�����x�͈�*/	
double	x_data[], y_data[];
int	data_num;
char	*line_color;	/*�v���t�@�C���\���F*/
char	*label_color;	/*���x���\���F*/
char	*title;

{
Window	root;
GC	gc;
Font	font;
Status	status;

int		x,y;
unsigned int	w_height, w_width;	/* �E�B���h�E�̑傫�� */
unsigned int	border, depth;		/* */
unsigned int	b_height, b_width;	/* �{�b�N�X�̑傫�� */
unsigned int	b_orig_x, b_orig_y;	/* �{�b�N�X�̌��_�̈ʒu */
BOX_RANGE	real_box;	/* �����P�ʂł̘g�͈� */
BOX_RANGE_long	pixel_box;	/* pixel�P�ʂł̘g�͈� */

int	gauge;

unsigned int	ch;

unsigned short	err_code;
int	i, j;
int	ii, jj;
double	mean,rms;
double	T;
double	step;
double	x_buffer, y_buffer;
double	tick_step_x,label_step_x;
double	tick_step_y,label_step_y;

RANGE_long	ch_range;
RANGE	x_tick_range, y_tick_range;
int	n_tick_step_x, n_label_step_x;
int	n_tick_step_y, n_label_step_y;
XY_POS		plot_pos;
XY_POS_long	plot_pix,prev_pix;
XY_SIZE_long	box_size_in_pixel;

char	string[MAX_FILE_NAME_LENGTH];
static char	ext[]=".scn";

err_code=0;


/* Sort data for x increasing order */
for(ii=0;ii<data_num;ii++){
	for(jj=ii+1;jj<data_num;jj++){
		if(x_data[ii]>x_data[jj]){
			x_buffer=x_data[ii];
			x_data[ii]=x_data[jj];
			x_data[jj]=x_buffer;
			y_buffer=y_data[ii];
			y_data[ii]=y_data[jj];
			y_data[jj]=y_buffer;
		}
	}
}

/* set range along the x-axis in the proper physical unit */
ch_range.min=0;
ch_range.max=data_num;
for(ch=0;ch<data_num;ch++){
	if((x_data[ch]>x_range.min)&&(ch_range.min==0))
		ch_range.min=ch;
	if((x_data[ch]>x_range.max)&&(ch_range.max==data_num))
		ch_range.max=ch-1;
}


/* �E�B���h�D�̑傫�������߂� */
status=XGetGeometry(dis,win,&root,&x,&y,&w_width,&w_height,&border,&depth);

pixel_box.x.min=(long)((double)w_width*orig_pos.x);
pixel_box.x.max=(long)((double)w_width*(orig_pos.x+box_size.dx));
pixel_box.y.min=(long)((double)w_height*orig_pos.y);
pixel_box.y.max=(long)((double)w_height*(orig_pos.y+box_size.dy));
box_size_in_pixel.dx=pixel_box.x.max-pixel_box.x.min;
box_size_in_pixel.dy=pixel_box.y.max-pixel_box.y.min;

STAR_AutoScale(x_range,&x_tick_range,&n_tick_step_x,&tick_step_x);
STAR_AutoScale(y_range,&y_tick_range,&n_tick_step_y,&tick_step_y);
real_box.x.min=x_range.min;
real_box.x.max=x_range.max;
real_box.y.min=y_tick_range.min;
real_box.y.max=y_tick_range.max;

STAR_SetPixelScale(real_box,pixel_box);

gc=XCreateGC(dis,win,0,0);
font=XLoadFont(dis,CHAR_FONT);
XSetFont(dis,gc,font);
XFlush(dis);

/* �g�̕\�� */
XSetForeground(dis,gc,MyColor(dis,label_color));
XDrawRectangle(dis,win,gc,pixel_box.x.min,pixel_box.y.min,
	box_size_in_pixel.dx,box_size_in_pixel.dy);
XFlush(dis);

/* �O�j���x���̃v���b�g */
if(y_tick_range.min<0.0 && y_tick_range.max>0.0){
	plot_pos.x=1.0; plot_pos.y=0.0;
	STAR_PositionPixel(plot_pos,&plot_pix);
	XDrawLine(dis,win,gc,pixel_box.x.min,plot_pix.y,pixel_box.x.max,
			plot_pix.y);
}
XFlush(dis);

/* �X�L�������̕\�� */
XDrawString(dis,win,gc,pixel_box.x.min+box_size_in_pixel.dx/40,
	pixel_box.y.min+box_size_in_pixel.dy/40+20
	,title,strlen(title) );

XFlush(dis);

/* x-axis name */
XDrawString(dis,win,gc
	,(pixel_box.x.min+pixel_box.x.max)/2-LABEL_HEIGHT*11/4
	,pixel_box.y.max+LABEL_HEIGHT*2+10
	,x_unit_char,strlen(x_unit_char) );
XFlush(dis);


/* x-axis���x���ƃe�B�b�N�}�[�N�̕\�� */
font=XLoadFont(dis,NUM_FONT);
XSetFont(dis,gc,font);

n_label_step_x=n_tick_step_x/2;
label_step_x=tick_step_x*2.0;
gauge=(int)((double)box_size_in_pixel.dy*0.04);
for(i=0;i<n_label_step_x;i++)
{
	plot_pos.x=x_tick_range.min+label_step_x*i;
	plot_pos.y=0.0;
	STAR_PositionPixel(plot_pos,&plot_pix);
	if((real_box.x.min<=plot_pos.x)&&(plot_pos.x<=real_box.x.max))
	{
		sprintf(string,"%d",(int)plot_pos.x);
		XDrawLine(dis,win,gc,plot_pix.x,pixel_box.y.min,plot_pix.x,
			pixel_box.y.min+gauge);
		XDrawLine(dis,win,gc,plot_pix.x,pixel_box.y.max,plot_pix.x,
			pixel_box.y.max-gauge);
		XDrawString(dis,win,gc,plot_pix.x-strlen(string)*6,
			pixel_box.y.max+LABEL_HEIGHT+5,
			string,strlen(string));
	}
}
XFlush(dis);

/* x-axis���ԃe�B�b�N�}�[�N�̕\�� */
gauge=gauge/2;
for(i=0;i<n_tick_step_x;i++)
{
	plot_pos.x=x_tick_range.min+tick_step_x*i;
	STAR_PositionPixel(plot_pos,&plot_pix);
	if((real_box.x.min<=plot_pos.x)&&(plot_pos.x<=real_box.x.max))
	{
		XDrawLine(dis,win,gc,plot_pix.x,pixel_box.y.min,
			plot_pix.x,pixel_box.y.min+gauge);
		XDrawLine(dis,win,gc,plot_pix.x,pixel_box.y.max,
			plot_pix.x,pixel_box.y.max-gauge);
	}
}
XFlush(dis);

/* ���x�����x���ƃe�B�b�N�}�[�N�̕\�� */
n_label_step_y=n_tick_step_y/2;
label_step_y=tick_step_y*2.0;

gauge=(int)((double)box_size_in_pixel.dx*0.04);
for(i=0;i<n_label_step_y;i++)
{
	plot_pos.y=y_tick_range.min+label_step_y*i;
	STAR_PositionPixel(plot_pos,&plot_pix);
	if((real_box.y.min<=plot_pos.y)&&(plot_pos.y<=real_box.y.max))
	{
		XDrawLine(dis,win,gc,pixel_box.x.min,plot_pix.y,
			pixel_box.x.min+gauge,plot_pix.y);
		XDrawLine(dis,win,gc,pixel_box.x.max,plot_pix.y,
			pixel_box.x.max-gauge,plot_pix.y);
		if(label_step_y>1.0)
			sprintf(string,"%d",(int)plot_pos.y);
		else
			sprintf(string,"%.3f",plot_pos.y);
		XDrawString(dis,win,gc
		,pixel_box.x.min-strlen(string)*LABEL_WIDTH
		-(int)(LABEL_WIDTH*0.5),plot_pix.y+LABEL_HEIGHT/2
		,string,strlen(string));
	}
}
XFlush(dis);

/* ���x�����ԃe�B�b�N�}�[�N�̕\�� */
gauge=gauge/2;
for(i=0;i<n_tick_step_y;i++)
{
	plot_pos.y=y_tick_range.min+tick_step_y*i;
	STAR_PositionPixel(plot_pos,&plot_pix);
	if((real_box.y.min<=plot_pos.y)&&(plot_pos.y<=real_box.y.max))
	{
		XDrawLine(dis,win,gc,pixel_box.x.min,plot_pix.y,
			pixel_box.x.min+gauge,plot_pix.y);
		XDrawLine(dis,win,gc,pixel_box.x.max,plot_pix.y,
			pixel_box.x.max-gauge,plot_pix.y);
	}
}
XFlush(dis);
		
/* �v���t�@�C���f�[�^�̕\�� */	
XSetForeground(dis,gc,MyColor(dis,line_color));
	plot_pos.x=x_data[ch_range.min];
	plot_pos.y=y_data[ch_range.min];
	STAR_PositionPixel(plot_pos,&prev_pix);
{
/* �`�摬�x�������邽�߂� X-Window ���ɓn���f�[�^�ʂ�
	�����������Ɍ��炷���߂ɃR�[�h�������ύX
	1993/9/26 J.Morino                                 */

	int min_y,max_y;
	int same_x;  /*�O�񓯂�x�̒l���Ƃ����Ƃ� 1 ����ȊO 0 */
	same_x=0;
	for(ch=ch_range.min;ch<ch_range.max;ch++)
	{
		plot_pos.x=x_data[ch];
		plot_pos.y=y_data[ch];	/* data��ch1���琔���� */
		STAR_PositionPixel(plot_pos,&plot_pix);
			
/* �O��Ɠ��l�ɈقȂ�x��PLOT */ 
		if((same_x==0)&&(prev_pix.x!=plot_pix.x)){
		XDrawLine(dis,win,gc,prev_pix.x,prev_pix.y,plot_pix.x,
			plot_pix.y);
		}
/* �O��PLOT�����Ƃ��Ɠ���x�ł������Ƃ�(2 ��ڈȏ�) */
		if((same_x==1)&&(prev_pix.x==plot_pix.x)){
			if(min_y>plot_pix.y){min_y=plot_pix.y;}
			if(max_y<plot_pix.y){max_y=plot_pix.y;}
		}
/* �O��PLOT�����Ƃ��Ɠ���x�ł������Ƃ�(�P���) */
		if((same_x==0)&&(prev_pix.x==plot_pix.x)){
			same_x=1;
			if (prev_pix.y<plot_pix.y){
				min_y=prev_pix.y;max_y=plot_pix.y;
			}
			else{
				max_y=prev_pix.y;min_y=plot_pix.y;
			}
		}
/* �O��͓���x�̒l��PLOT�����ɂ��č���قȂ�x�̒l��������ꍇ*/
		if((same_x==1)&&(prev_pix.x!=plot_pix.x)){
			same_x=0;
			XDrawLine(dis,win,gc,prev_pix.x,prev_pix.y,plot_pix.x,
				plot_pix.y);
			XDrawLine(dis,win,gc,prev_pix.x,min_y,prev_pix.x,
				max_y);
		}
		prev_pix.x=plot_pix.x;
		prev_pix.y=plot_pix.y;
	}
/* ���ׂĂ�ch���ɂ��ďI�������̏��� */
	if(same_x==1){
		XDrawLine(dis,win,gc,prev_pix.x,min_y,prev_pix.x,max_y);
	}
}
XFlush(dis);
return(err_code);
}
/*-----------------------*/







int	profile_plot2(dis,win,box_size,orig_pos,x_unit,x_range,T_range,
		profile1,profile2,line_color,label_color)

/*	��̃v���t�@�C�����v���b�g����	

1994/8/24	T.OKA	
	
*/
Display	*dis;		/*�f�B�X�v���C*/
Window	win;		/*�E�B���h�E*/
XY_SIZE	box_size;	/*�{�b�N�X�̑傫���i�E�B���h�E�ɑ΂����*/
XY_POS	orig_pos;	/*�{�b�N�X�̌��_�̈ʒu�i�E�B���h�E�ɑ΂����*/
int	x_unit;		/*�\���͈͒P�ʎw�� (X_VELOCITY �ȊO�͎󂯕t���Ȃ��B)*/
RANGE	x_range,	/*�\�������͈�*/
	T_range;	/*�\�����x�͈�*/	
PROFILE	*profile1;	/*�v���t�@�C���f�[�^1*/
PROFILE	*profile2;	/*�v���t�@�C���f�[�^2*/
char	*line_color;	/*�v���t�@�C���\���F*/
char	*label_color;	/*���x���\���F*/

{
Window	root;
GC	gc;
Font	font;
Status	status;

int		x,y;
unsigned int	w_height, w_width;	/* �E�B���h�E�̑傫�� */
unsigned int	border, depth;		/* */
unsigned int	b_height, b_width;	/* �{�b�N�X�̑傫�� */
unsigned int	b_orig_x, b_orig_y;	/* �{�b�N�X�̌��_�̈ʒu */
BOX_RANGE	real_box;	/* �����P�ʂł̘g�͈� */
BOX_RANGE_long	pixel_box1, pixel_box2;	/* pixel�P�ʂł̘g�͈� */

int	gauge;

unsigned int	ch;

unsigned short	err_code;
int	i,j;
double	mean,rms;
double	T;
double	step;

double	tick_step_x,label_step_x;
double	tick_step_y,label_step_y;

RANGE_long	ch_range1, ch_range2;
RANGE	x_tick_range,y_tick_range;
int	n_tick_step_x,n_label_step_x;
int	n_tick_step_y,n_label_step_y;
XY_POS		plot_pos;
XY_POS_long	plot_pix,prev_pix;
XY_SIZE_long	box_size_in_pixel;

char	string[MAX_FILE_NAME_LENGTH];
static char	ext[]=".scn";

err_code=0;

/* set range along the x-axis in the proper physical unit */
if(x_unit!=X_VELOCITY){
	err_code=31;
	x_unit=X_VELOCITY;
}
else{
	ch_range1.min=(long)STAR_VelocityCh(profile1,x_range.min);
	ch_range1.max=(long)(STAR_VelocityCh(profile1,x_range.max)+0.9999);
	ch_range2.min=(long)STAR_VelocityCh(profile2,x_range.min);
	ch_range2.max=(long)(STAR_VelocityCh(profile2,x_range.max)+0.9999);
}
/* �p�����[�^�̃`�F�b�N */
STAR_SortRangeLong(&ch_range1);
STAR_SortRangeLong(&ch_range2);
if(ch_range1.min==ch_range1.max)	err_code=11;
if(ch_range2.min==ch_range2.max)	err_code=11;

STAR_SortRange(&T_range);
if(T_range.min==T_range.max)	err_code=21;

if(ch_range1.min<1) ch_range1.min=1;
if(ch_range1.max>(*profile1).be_status.data_ch)
	ch_range1.max=(*profile1).be_status.data_ch;
if(ch_range2.min<1) ch_range2.min=1;
if(ch_range2.max>(*profile1).be_status.data_ch)
	ch_range2.max=(*profile2).be_status.data_ch;

/* �E�B���h�D�̑傫�������߂� */
status=XGetGeometry(dis,win,&root,&x,&y,&w_width,&w_height,&border,&depth);

pixel_box1.x.min=(long)((double)w_width*orig_pos.x);
pixel_box1.x.max=(long)((double)w_width*(orig_pos.x+box_size.dx));
pixel_box1.y.min=(long)((double)w_height*2.0/3.0*orig_pos.y);
pixel_box1.y.max=(long)((double)w_height*2.0/3.0*(orig_pos.y+box_size.dy));
pixel_box2.x.min=(long)((double)w_width*orig_pos.x);
pixel_box2.x.max=(long)((double)w_width*(orig_pos.x+box_size.dx));
pixel_box2.y.min=(long)((double)w_height*2.0/3.0*(0.5+orig_pos.y));
pixel_box2.y.max=(long)((double)w_height*2.0/3.0*(0.5+orig_pos.y+box_size.dy));
box_size_in_pixel.dx=pixel_box1.x.max-pixel_box1.x.min;
box_size_in_pixel.dy=pixel_box1.y.max-pixel_box1.y.min;

STAR_AutoScale(x_range,&x_tick_range,&n_tick_step_x,&tick_step_x);
STAR_AutoScale(T_range,&y_tick_range,&n_tick_step_y,&tick_step_y);
real_box.x.min=x_range.min;
real_box.x.max=x_range.max;
real_box.y.min=y_tick_range.min;
real_box.y.max=y_tick_range.max;


/*	Profile1�̕\��	*/

STAR_SetPixelScale(real_box,pixel_box1);

gc=XCreateGC(dis,win,0,0);
font=XLoadFont(dis,CHAR_FONT);
XSetFont(dis,gc,font);
XFlush(dis);

/* �g�̕\�� */
XSetForeground(dis,gc,MyColor(dis,label_color));
XDrawLine(dis,win,gc,pixel_box1.x.min,pixel_box1.y.min,
		pixel_box1.x.min,pixel_box1.y.max);
XDrawLine(dis,win,gc,pixel_box1.x.min,pixel_box1.y.min,
		pixel_box1.x.max,pixel_box1.y.min);
XDrawLine(dis,win,gc,pixel_box1.x.max,pixel_box1.y.max,
		pixel_box1.x.max,pixel_box1.y.min);
/*XDrawRectangle(dis,win,gc,pixel_box1.x.min,pixel_box1.y.min,
	box_size_in_pixel.dx,box_size_in_pixel.dy);*/
XFlush(dis);
/* �O�j���x���̃v���b�g */
if(y_tick_range.min<0.0 && y_tick_range.max>0.0){
	plot_pos.x=1.0; plot_pos.y=0.0;
	STAR_PositionPixel(plot_pos,&plot_pix);
	XDrawLine(dis,win,gc,pixel_box1.x.min,plot_pix.y,pixel_box1.x.max,
			plot_pix.y);
}
XFlush(dis);

/* �X�L�������̕\�� */
plot_pos.x=0.0; plot_pos.y=0.0;
STAR_PositionPixel(plot_pos,&plot_pix);
XDrawString(dis,win,gc,pixel_box1.x.min+box_size_in_pixel.dx/40,
	plot_pix.y+box_size_in_pixel.dy/40+20
	,(*profile1).file_name,strlen((*profile1).file_name) );

XFlush(dis);

/* x-axis name */
/*
if(x_unit==X_CHANNEL){
	XDrawString(dis,win,gc
	,(pixel_box1.x.min+pixel_box1.x.max)/2-LABEL_HEIGHT*11/4
	,pixel_box1.y.max+LABEL_HEIGHT*2+10
	,"channel (ch)",12 );
}
else if(x_unit==X_VELOCITY){
	XDrawString(dis,win,gc
	,(pixel_box1.x.min+pixel_box1.x.max)/2-LABEL_HEIGHT*19/4
	,pixel_box1.y.max+LABEL_HEIGHT*2+10
	,"LSR velocity (km/s)",19 );
}
XFlush(dis);
*/


/* x-axis���x���ƃe�B�b�N�}�[�N�̕\�� */
font=XLoadFont(dis,NUM_FONT);
XSetFont(dis,gc,font);

n_label_step_x=n_tick_step_x/2;
label_step_x=tick_step_x*2.0;
gauge=(int)((double)box_size_in_pixel.dy*0.04);
for(i=0;i<n_label_step_x;i++)
{
	plot_pos.x=x_tick_range.min+label_step_x*i;
	plot_pos.y=0.0;
	STAR_PositionPixel(plot_pos,&plot_pix);
	if((real_box.x.min<=plot_pos.x)&&(plot_pos.x<=real_box.x.max))
	{
		sprintf(string,"%d",(int)plot_pos.x);
		XDrawLine(dis,win,gc,plot_pix.x,pixel_box1.y.min,plot_pix.x,
			pixel_box1.y.min+gauge);
		/*XDrawLine(dis,win,gc,plot_pix.x,pixel_box1.y.max,plot_pix.x,
			pixel_box1.y.max-gauge);*/
		/*XDrawString(dis,win,gc,plot_pix.x-strlen(string)*6,
			pixel_box1.y.max+LABEL_HEIGHT+5,
			string,strlen(string));*/
	}
}
XFlush(dis);

/* x-axis���ԃe�B�b�N�}�[�N�̕\�� */
gauge=gauge/2;
for(i=0;i<n_tick_step_x;i++)
{
	plot_pos.x=x_tick_range.min+tick_step_x*i;
	STAR_PositionPixel(plot_pos,&plot_pix);
	if((real_box.x.min<=plot_pos.x)&&(plot_pos.x<=real_box.x.max))
	{
		XDrawLine(dis,win,gc,plot_pix.x,pixel_box1.y.min,
			plot_pix.x,pixel_box1.y.min+gauge);
		/*XDrawLine(dis,win,gc,plot_pix.x,pixel_box1.y.max,
			plot_pix.x,pixel_box1.y.max-gauge);*/
	}
}
XFlush(dis);

/* ���x�����x���ƃe�B�b�N�}�[�N�̕\�� */
n_label_step_y=n_tick_step_y/2;
label_step_y=tick_step_y*2.0;

gauge=(int)((double)box_size_in_pixel.dx*0.04);
for(i=0;i<n_label_step_y;i++)
{
	plot_pos.y=y_tick_range.min+label_step_y*i;
	STAR_PositionPixel(plot_pos,&plot_pix);
	if((real_box.y.min<=plot_pos.y)&&(plot_pos.y<=real_box.y.max))
	{
		XDrawLine(dis,win,gc,pixel_box1.x.min,plot_pix.y,
			pixel_box1.x.min+gauge,plot_pix.y);
		XDrawLine(dis,win,gc,pixel_box1.x.max,plot_pix.y,
			pixel_box1.x.max-gauge,plot_pix.y);
		if(label_step_y>1.0)
			sprintf(string,"%d",(int)plot_pos.y);
		else
			sprintf(string,"%.3f",plot_pos.y);
		XDrawString(dis,win,gc
		,pixel_box1.x.min-strlen(string)*LABEL_WIDTH
		-(int)(LABEL_WIDTH*0.5),plot_pix.y+LABEL_HEIGHT/2
		,string,strlen(string));
	}
}
XFlush(dis);

/* ���x�����ԃe�B�b�N�}�[�N�̕\�� */
gauge=gauge/2;
for(i=0;i<n_tick_step_y;i++)
{
	plot_pos.y=y_tick_range.min+tick_step_y*i;
	STAR_PositionPixel(plot_pos,&plot_pix);
	if((real_box.y.min<=plot_pos.y)&&(plot_pos.y<=real_box.y.max))
	{
		XDrawLine(dis,win,gc,pixel_box1.x.min,plot_pix.y,
			pixel_box1.x.min+gauge,plot_pix.y);
		XDrawLine(dis,win,gc,pixel_box1.x.max,plot_pix.y,
			pixel_box1.x.max-gauge,plot_pix.y);
	}
}
XFlush(dis);
		
/* �v���t�@�C���f�[�^�̕\�� */	
XSetForeground(dis,gc,MyColor(dis,line_color));
     if(x_unit==X_CHANNEL){
       plot_pos.x=ch_range1.min;
     }
     else{
       plot_pos.x=STAR_ChVelocity(profile1,(double)ch_range1.min);
     }
     plot_pos.y=(*profile1).data[ch_range1.min-1]*(*profile1).rx_status.scale;
     STAR_PositionPixel(plot_pos,&prev_pix);
     {
       /* �`�摬�x�������邽�߂� X-Window ���ɓn���f�[�^�ʂ�
	  �����������Ɍ��炷���߂ɃR�[�h�������ύX
	  1993/9/26 J.Morino                                 */

       int min_y,max_y;
       int same_x;  /*�O�񓯂�x�̒l���Ƃ����Ƃ� 1 ����ȊO 0 */
       same_x=0;
       for(ch=ch_range1.min;ch<=ch_range1.max;ch++)
	 {
	   if(x_unit==X_CHANNEL){
	     plot_pos.x=ch;
	   }
	   else{
	     plot_pos.x=STAR_ChVelocity(profile1,(double)ch);
	   }
	   plot_pos.y=(*profile1).data[ch-1];	/* data��ch1���琔���� */
	   STAR_PositionPixel(plot_pos,&plot_pix);

	   /* X-Window��łЂƂ̕\�������Ƃ��ɉ������Ƀs�N�Z������
	      �[���łȂ��Ƃ������̂P�s�N�Z���ɑ΂��Đ�ch�̃f�[�^���d
	      �Ȃ�B���̂悤�ȏꍇ�ɏd�Ȃ��Ă�����̂ɑ΂��čő�l��
	      �ŏ��l�݂̂�X-Window���ɓn���Ώd�ˏ�������Ԃ�̎d����
	      ����A�`�摬�x���オ��A�܂��l�b�g���[�N���o�R����ꍇ
	      ���̕��S���ւ�B
	      �����ł͐��������Ƃ��ɂ������Ƀs�N�Z�����ς��Ȃ��ꍇ
	      same_x ���P�ɃZ�b�g���ő�l�ƍŏ��l���L������B
	      �������Ƀs�N�Z�����ς�����ꍇ�A�������ɂ��̐���������
	      �Ƃ��ɁA���Ƃ̂����W�ōő�l�ƍŏ��l���Ȃ��d�ˏ�����
	      ���̂Ƃ��Ȃ���Ԃɂ���B
	      ���ׂĂ�ch���������I��������same_x  ���P�ɃZ�b�g�����
	      �����ꍇ���Ƃ̂����W�ōő�l�ƍŏ��l���Ȃ��A���ׂĂ�
	      �����������Ƃ���B

	      ��
			     * 
			     ** <-- max_y
		   ^	      **
		   |y	      ** ....... * �̓f�[�^
      		   |	      **
		   |	      *
		   +----> x   *  <-- min_y

			      ^----- ���̗�ɂ���              */


	       /* �O��Ɠ��l�ɈقȂ�x��PLOT */ 
	   if((same_x==0)&&(prev_pix.x!=plot_pix.x)){
	     XDrawLine(dis,win,gc,prev_pix.x,prev_pix.y,plot_pix.x,
		       plot_pix.y);
	   }
	   /* �O��PLOT�����Ƃ��Ɠ���x�ł������Ƃ�(2 ��ڈȏ�) */
	   if((same_x==1)&&(prev_pix.x==plot_pix.x)){
	     if(min_y>plot_pix.y){min_y=plot_pix.y;}
	     if(max_y<plot_pix.y){max_y=plot_pix.y;}
	   }
	   /* �O��PLOT�����Ƃ��Ɠ���x�ł������Ƃ�(�P���) */
	   if((same_x==0)&&(prev_pix.x==plot_pix.x)){
	     same_x=1;
	     if (prev_pix.y<plot_pix.y){
	       min_y=prev_pix.y;max_y=plot_pix.y;}
	     else{
	       max_y=prev_pix.y;min_y=plot_pix.y;}
	   }
	   /* �O��͓���x�̒l��PLOT�����ɂ��č���قȂ�x�̒l��������ꍇ*/
	   if((same_x==1)&&(prev_pix.x!=plot_pix.x)){
	     same_x=0;
	     XDrawLine(dis,win,gc,prev_pix.x,prev_pix.y,plot_pix.x,
		       plot_pix.y);
	     XDrawLine(dis,win,gc,prev_pix.x,min_y     ,prev_pix.x,max_y);
	   }
	   prev_pix.x=plot_pix.x;
	   prev_pix.y=plot_pix.y;
	 }
       /* ���ׂĂ�ch���ɂ��ďI�������̏��� */
       if(same_x==1){
	 XDrawLine(dis,win,gc,prev_pix.x,min_y,prev_pix.x,max_y);
       }
     }
XFlush(dis);









/*	Profile2�̕\��	*/

STAR_SetPixelScale(real_box,pixel_box2);

gc=XCreateGC(dis,win,0,0);
font=XLoadFont(dis,CHAR_FONT);
XSetFont(dis,gc,font);
XFlush(dis);

/* �g�̕\�� */
XSetForeground(dis,gc,MyColor(dis,label_color));
XDrawLine(dis,win,gc,pixel_box2.x.min,pixel_box2.y.min,
		pixel_box2.x.min,pixel_box2.y.max);
XDrawLine(dis,win,gc,pixel_box2.x.min,pixel_box2.y.max,
		pixel_box2.x.max,pixel_box2.y.max);
XDrawLine(dis,win,gc,pixel_box2.x.max,pixel_box2.y.max,
		pixel_box2.x.max,pixel_box2.y.min);
/*XDrawRectangle(dis,win,gc,pixel_box2.x.min,pixel_box2.y.min,
	box_size_in_pixel.dx,box_size_in_pixel.dy);*/
XFlush(dis);

/* �O�j���x���̃v���b�g */
if(y_tick_range.min<0.0 && y_tick_range.max>0.0){
	plot_pos.x=1.0; plot_pos.y=0.0;
	STAR_PositionPixel(plot_pos,&plot_pix);
	XDrawLine(dis,win,gc,pixel_box2.x.min,plot_pix.y,pixel_box2.x.max,
			plot_pix.y);
}
XFlush(dis);

/* �X�L�������̕\�� */
plot_pos.x=0.0; plot_pos.y=0.0;
STAR_PositionPixel(plot_pos,&plot_pix);
XDrawString(dis,win,gc,pixel_box2.x.min+box_size_in_pixel.dx/40,
	plot_pix.y+box_size_in_pixel.dy/40+20
	,(*profile2).file_name,strlen((*profile2).file_name) );

XFlush(dis);

/* x-axis name */
if(x_unit==X_CHANNEL){
	XDrawString(dis,win,gc
	,(pixel_box2.x.min+pixel_box2.x.max)/2-LABEL_HEIGHT*11/4
	,pixel_box2.y.max+LABEL_HEIGHT*2+10
	,"channel (ch)",12 );
}
else if(x_unit==X_VELOCITY){
	XDrawString(dis,win,gc
	,(pixel_box2.x.min+pixel_box2.x.max)/2-LABEL_HEIGHT*19/4
	,pixel_box2.y.max+LABEL_HEIGHT*2+10
	,"LSR velocity (km/s)",19 );
}
XFlush(dis);


/* x-axis���x���ƃe�B�b�N�}�[�N�̕\�� */
font=XLoadFont(dis,NUM_FONT);
XSetFont(dis,gc,font);

n_label_step_x=n_tick_step_x/2;
label_step_x=tick_step_x*2.0;
gauge=(int)((double)box_size_in_pixel.dy*0.04);
for(i=0;i<n_label_step_x;i++)
{
	plot_pos.x=x_tick_range.min+label_step_x*i;
	plot_pos.y=0.0;
	STAR_PositionPixel(plot_pos,&plot_pix);
	if((real_box.x.min<=plot_pos.x)&&(plot_pos.x<=real_box.x.max))
	{
		sprintf(string,"%d",(int)plot_pos.x);
		/*XDrawLine(dis,win,gc,plot_pix.x,pixel_box2.y.min,plot_pix.x,
			pixel_box2.y.min+gauge);*/
		XDrawLine(dis,win,gc,plot_pix.x,pixel_box2.y.max,plot_pix.x,
			pixel_box2.y.max-gauge);
		XDrawString(dis,win,gc,plot_pix.x-strlen(string)*6,
			pixel_box2.y.max+LABEL_HEIGHT+5,
			string,strlen(string));
	}
}
XFlush(dis);

/* x-axis���ԃe�B�b�N�}�[�N�̕\�� */
gauge=gauge/2;
for(i=0;i<n_tick_step_x;i++)
{
	plot_pos.x=x_tick_range.min+tick_step_x*i;
	STAR_PositionPixel(plot_pos,&plot_pix);
	if((real_box.x.min<=plot_pos.x)&&(plot_pos.x<=real_box.x.max))
	{
		/*XDrawLine(dis,win,gc,plot_pix.x,pixel_box2.y.min,
			plot_pix.x,pixel_box2.y.min+gauge);*/
		XDrawLine(dis,win,gc,plot_pix.x,pixel_box2.y.max,
			plot_pix.x,pixel_box2.y.max-gauge);
	}
}
XFlush(dis);

/* ���x�����x���ƃe�B�b�N�}�[�N�̕\�� */
n_label_step_y=n_tick_step_y/2;
label_step_y=tick_step_y*2.0;

gauge=(int)((double)box_size_in_pixel.dx*0.04);
for(i=0;i<n_label_step_y;i++)
{
	plot_pos.y=y_tick_range.min+label_step_y*i;
	STAR_PositionPixel(plot_pos,&plot_pix);
	if((real_box.y.min<=plot_pos.y)&&(plot_pos.y<=real_box.y.max))
	{
		XDrawLine(dis,win,gc,pixel_box2.x.min,plot_pix.y,
			pixel_box2.x.min+gauge,plot_pix.y);
		XDrawLine(dis,win,gc,pixel_box2.x.max,plot_pix.y,
			pixel_box2.x.max-gauge,plot_pix.y);
		if(label_step_y>1.0)
			sprintf(string,"%d",(int)plot_pos.y);
		else
			sprintf(string,"%.3f",plot_pos.y);
		XDrawString(dis,win,gc
		,pixel_box2.x.min-strlen(string)*LABEL_WIDTH
		-(int)(LABEL_WIDTH*0.5),plot_pix.y+LABEL_HEIGHT/2
		,string,strlen(string));
	}
}
XFlush(dis);

/* ���x�����ԃe�B�b�N�}�[�N�̕\�� */
gauge=gauge/2;
for(i=0;i<n_tick_step_y;i++)
{
	plot_pos.y=y_tick_range.min+tick_step_y*i;
	STAR_PositionPixel(plot_pos,&plot_pix);
	if((real_box.y.min<=plot_pos.y)&&(plot_pos.y<=real_box.y.max))
	{
		XDrawLine(dis,win,gc,pixel_box2.x.min,plot_pix.y,
			pixel_box2.x.min+gauge,plot_pix.y);
		XDrawLine(dis,win,gc,pixel_box2.x.max,plot_pix.y,
			pixel_box2.x.max-gauge,plot_pix.y);
	}
}
XFlush(dis);
		
/* �v���t�@�C���f�[�^�̕\�� */	
XSetForeground(dis,gc,MyColor(dis,line_color));
     if(x_unit==X_CHANNEL){
       plot_pos.x=ch_range2.min;
     }
     else{
       plot_pos.x=STAR_ChVelocity(profile2,(double)ch_range2.min);
     }
     plot_pos.y=(*profile2).data[ch_range2.min-1]*(*profile2).rx_status.scale;
     STAR_PositionPixel(plot_pos,&prev_pix);
     {
       /* �`�摬�x�������邽�߂� X-Window ���ɓn���f�[�^�ʂ�
	  �����������Ɍ��炷���߂ɃR�[�h�������ύX
	  1993/9/26 J.Morino                                 */

       int min_y,max_y;
       int same_x;  /*�O�񓯂�x�̒l���Ƃ����Ƃ� 1 ����ȊO 0 */
       same_x=0;
       for(ch=ch_range2.min;ch<=ch_range2.max;ch++)
	 {
	   if(x_unit==X_CHANNEL){
	     plot_pos.x=ch;
	   }
	   else{
	     plot_pos.x=STAR_ChVelocity(profile2,(double)ch);
	   }
	   plot_pos.y=(*profile2).data[ch-1]*(*profile2).rx_status.scale;	/* data��ch1���琔���� */
	   STAR_PositionPixel(plot_pos,&plot_pix);

	   /* X-Window��łЂƂ̕\�������Ƃ��ɉ������Ƀs�N�Z������
	      �[���łȂ��Ƃ������̂P�s�N�Z���ɑ΂��Đ�ch�̃f�[�^���d
	      �Ȃ�B���̂悤�ȏꍇ�ɏd�Ȃ��Ă�����̂ɑ΂��čő�l��
	      �ŏ��l�݂̂�X-Window���ɓn���Ώd�ˏ�������Ԃ�̎d����
	      ����A�`�摬�x���オ��A�܂��l�b�g���[�N���o�R����ꍇ
	      ���̕��S���ւ�B
	      �����ł͐��������Ƃ��ɂ������Ƀs�N�Z�����ς��Ȃ��ꍇ
	      same_x ���P�ɃZ�b�g���ő�l�ƍŏ��l���L������B
	      �������Ƀs�N�Z�����ς�����ꍇ�A�������ɂ��̐���������
	      �Ƃ��ɁA���Ƃ̂����W�ōő�l�ƍŏ��l���Ȃ��d�ˏ�����
	      ���̂Ƃ��Ȃ���Ԃɂ���B
	      ���ׂĂ�ch���������I��������same_x  ���P�ɃZ�b�g�����
	      �����ꍇ���Ƃ̂����W�ōő�l�ƍŏ��l���Ȃ��A���ׂĂ�
	      �����������Ƃ���B

	      ��
			     * 
			     ** <-- max_y
		   ^	      **
		   |y	      ** ....... * �̓f�[�^
      		   |	      **
		   |	      *
		   +----> x   *  <-- min_y

			      ^----- ���̗�ɂ���              */


	       /* �O��Ɠ��l�ɈقȂ�x��PLOT */ 
	   if((same_x==0)&&(prev_pix.x!=plot_pix.x)){
	     XDrawLine(dis,win,gc,prev_pix.x,prev_pix.y,plot_pix.x,
		       plot_pix.y);
	   }
	   /* �O��PLOT�����Ƃ��Ɠ���x�ł������Ƃ�(2 ��ڈȏ�) */
	   if((same_x==1)&&(prev_pix.x==plot_pix.x)){
	     if(min_y>plot_pix.y){min_y=plot_pix.y;}
	     if(max_y<plot_pix.y){max_y=plot_pix.y;}
	   }
	   /* �O��PLOT�����Ƃ��Ɠ���x�ł������Ƃ�(�P���) */
	   if((same_x==0)&&(prev_pix.x==plot_pix.x)){
	     same_x=1;
	     if (prev_pix.y<plot_pix.y){
	       min_y=prev_pix.y;max_y=plot_pix.y;}
	     else{
	       max_y=prev_pix.y;min_y=plot_pix.y;}
	   }
	   /* �O��͓���x�̒l��PLOT�����ɂ��č���قȂ�x�̒l��������ꍇ*/
	   if((same_x==1)&&(prev_pix.x!=plot_pix.x)){
	     same_x=0;
	     XDrawLine(dis,win,gc,prev_pix.x,prev_pix.y,plot_pix.x,
		       plot_pix.y);
	     XDrawLine(dis,win,gc,prev_pix.x,min_y     ,prev_pix.x,max_y);
	   }
	   prev_pix.x=plot_pix.x;
	   prev_pix.y=plot_pix.y;
	 }
       /* ���ׂĂ�ch���ɂ��ďI�������̏��� */
       if(same_x==1){
	 XDrawLine(dis,win,gc,prev_pix.x,min_y,prev_pix.x,max_y);
       }
     }
XFlush(dis);










return(err_code);
}
/*-----------------------*/
