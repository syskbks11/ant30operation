LIBDIR	=export

all	:
	make -f Makefile.gcc
        #/bin/rm -f core *.o *.BAK *.ti *.ii
        #make -f Makefile.ffc
	cp src/libtkb.h ${LIBDIR}/.
clean   :
	/bin/rm -f core *.o *.BAK *.ti *.ii
	/bin/rm -fr ${LIBDIR}/lib*
