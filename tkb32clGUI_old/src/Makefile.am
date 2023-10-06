## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DPACKAGE_PREFIX=\""$(prefix)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@

bin_PROGRAMS = tkb32clgui

tkb32clgui_SOURCES = \
	main.c \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	callbacksSub.c callbacksSub.h \
	import/tkb32clFunc.cpp import/tkb32clFunc.h \
	import/tkb32clPort.cpp import/tkb32clPort.h \
	import/errno.cpp import/errno.h

tkb32clgui_LDADD = @PACKAGE_LIBS@ $(INTLLIBS) ../../libtkb/export/libtkb.a
#tkb32clgui_LDADD = @PACKAGE_LIBS@ $(INTLLIBS) -L/usr/local/FFC/lib -lssl2 -lstd6b ../../libtkb/export/libtkb_ffc.a

