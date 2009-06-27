# Wedit Makefile for project ntpbserver
SRCDIR=d:\msys\1.0\local\ps2dev\task2308\ntpbserver_pc
CFLAGS=-I"c:\lcc\include"  -g2 
CC=$(LCCROOT)\bin\lcc.exe
LINKER=$(LCCROOT)\bin\lcclnk.exe
OBJS=\
	ntpbserver.res \
	ntpbserver.obj

LIBS=
EXE=ntpbserver.exe

$(EXE):	$(OBJS) Makefile
	$(LINKER)  -subsystem windows -o $(SRCDIR)\lcc\ntpbserver.exe $(OBJS) $(LIBS)

# Build ntpbserver.res
NTPBSERVER_RC=\
	$(SRCDIR)\ntpbserverres.h\

ntpbserver.res:	$(NTPBSERVER_RC) $(SRCDIR)\ntpbserver.rc
	$(LCCROOT)\bin\lrc.exe -I$(SRCDIR) -I"c:\lcc\include"  $(SRCDIR)\ntpbserver.rc

# Build ntpbserver.c
NTPBSERVER_C=\
	$(SRCDIR)\ntpbserverres.h\

ntpbserver.obj: $(NTPBSERVER_C) $(SRCDIR)\ntpbserver.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\ntpbserver.c

link:
	$(LINKER)  -subsystem windows -o $(SRCDIR)\lcc\ntpbserver.exe $(OBJS) $(LIBS)

clean:
	del $(OBJS) ntpbserver.exe
