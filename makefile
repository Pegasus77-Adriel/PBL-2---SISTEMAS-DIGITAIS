LIBOBJS = lcdInit.o map.o lcdWrite.o clear.o

all: main

%.o : %.s
	as $(DEBUGFLGS) $(LSTFLGS) $< -o $@
	
libdisplay.a: $(LIBOBJS)
	ar -cvq libdisplay.a lcdInit.o map.o lcdWrite.o clear.o

main: main.c libdisplay.a
	gcc -o main main.c libdisplay.a
