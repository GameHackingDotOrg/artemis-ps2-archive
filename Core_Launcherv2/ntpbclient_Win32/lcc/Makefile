# Wedit Makefile for project ntpbclient_win32
SRCDIR=d:\msys\1.0\local\ps2dev\core_launcherv2\ntpbclient_win32
CFLAGS=-I"c:\lcc\include"  -g2 
CC=$(LCCROOT)\bin\lcc.exe
LINKER=$(LCCROOT)\bin\lcclnk.exe
OBJS=\
	ntpbclient_win32.res \
	ntpbclient_win32.obj

LIBS=
EXE=ntpbclient_win32.exe

$(EXE):	$(OBJS) Makefile
	$(LINKER)  -subsystem windows -o $(SRCDIR)\lcc\ntpbclient_win32.exe $(OBJS) $(LIBS)

# Build ntpbclient_win32.res
NTPBCLIENT_WIN32_RC=\
	$(SRCDIR)\ntpbclient_win32res.h\

ntpbclient_win32.res:	$(NTPBCLIENT_WIN32_RC) $(SRCDIR)\ntpbclient_win32.rc
	$(LCCROOT)\bin\lrc.exe -I$(SRCDIR) -I"c:\lcc\include"  $(SRCDIR)\ntpbclient_win32.rc

# Build ntpbclient_win32.c
NTPBCLIENT_WIN32_C=\
	$(SRCDIR)\ntpbclient_win32res.h\

ntpbclient_win32.obj: $(NTPBCLIENT_WIN32_C) $(SRCDIR)\ntpbclient_win32.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\ntpbclient_win32.c

link:
	$(LINKER)  -subsystem windows -o $(SRCDIR)\lcc\ntpbclient_win32.exe $(OBJS) $(LIBS)

clean:
	del $(OBJS) ntpbclient_win32.exe
