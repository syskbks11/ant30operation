/*
Software Tools for radio Astronomical data Reduction

Window library header file
1992/9/6 T.Handa
1993/1/8 T.Handa
1994/8/30 T.OKA
*/

#define	X_VELOCITY	1
#define	X_CHANNEL	0

#ifdef _NO_PROTO
extern int	profile_plot();
extern void	STAR_AutoScale();
extern void	STAR_SetPixelScale();
extern void	STAR_PixelPosition();
extern void	STAR_PositionPixel();
extern unsigned long	MyColor();
extern void	PSInitialize();
extern void	PSDrawLine();
extern void	PSDrawRectangle();
extern void	PSDrawString();
extern void	PSFlush();
extern int	general_plot();
extern int	profile_plot2();


#else /* undef _NO_PROTO */
extern int	profile_plot(Display *, Window, XY_SIZE, XY_POS,
	int, RANGE, RANGE, PROFILE *, char *, char *);
extern void	STAR_AutoScale(RANGE, RANGE *, int *, double *);
extern void	STAR_SetPixelScale(BOX_RANGE, BOX_RANGE_long);
extern void	STAR_PixelPosition(XY_POS_long, XY_POS *);
extern void	STAR_PositionPixel(XY_POS, XY_POS_long *);
extern unsigned long	MyColor(Display *, char *);
extern void	PSInitialize(FILE *, XY_POS *, XY_POS *, double);
extern void	PSDrawLine(FILE *, XY_POS *, XY_POS *, double);
extern void	PSDrawRectangle(FILE *, XY_POS *, XY_POS *, double);
extern void	PSDrawString(FILE *, XY_POS *, char *, char *, short);
extern void	PSFlush(FILE *);
extern int	general_plot(Display *, Window, XY_SIZE, XY_POS,
	char *, RANGE, RANGE, double *, double *, int, char *, char *, char *);
extern int	profile_plot2(Display *, Window, XY_SIZE, XY_POS,
	int, RANGE, RANGE, PROFILE *, PROFILE *, char *, char *);

#endif /* endif of define _NO_PROTO */
