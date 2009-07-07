# Project: PS2CC

NAME  = ps2cc
CC = gcc.exe
WINDRES = windres
RES  = ps2cc.res
OBJ  = ps2cc.o tab_search.o lib_api.o lib_fileio.o lib_memory.o lib_misc.o $(RES)
LDFLAGS = -s -mwindows -lwsock32 -lcomctl32
CFLAGS = -O2 -s

.PHONY: all

all: $(NAME)


clean:
	rm -f $(OBJ) $(NAME) $(NAME).exe

$(NAME): $(OBJ)
	$(CC) $(OBJ) -o $(NAME) $(LDFLAGS)

ps2cc.o: ps2cc.c ps2cc.h ps2cc_gui.h _types.h
	$(CC) -c ps2cc.c -o ps2cc.o $(CFLAGS)

tab_search.o: tab_search.c ps2cc.h _types.h
	$(CC) -c tab_search.c -o tab_search.o $(CFLAGS)

lib_api.o: lib_api.c _types.h
	$(CC) -c lib_api.c -o lib_api.o $(CFLAGS)	

lib_fileio.o: lib_fileio.c _types.h
	$(CC) -c lib_fileio.c -o lib_fileio.o $(CFLAGS)	

lib_misc.o: lib_misc.c _types.h
	$(CC) -c lib_misc.c -o lib_misc.o $(CFLAGS)	

lib_memory.o: lib_memory.c _types.h
	$(CC) -c lib_memory.c -o lib_memory.o $(CFLAGS)	

ps2cc.res: ps2cc.rc ps2cc_gui.h
	$(WINDRES) -i ps2cc.rc -J rc -o ps2cc.res -O coff

