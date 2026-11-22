CC ?= gcc
CFLAGS ?= -O2 -Wall `sdl2-config --cflags`
LDFLAGS ?= `sdl2-config --libs` -lm

all: byterunner

byterunner: main.c
	$(CC) $(CFLAGS) main.c -o byterunner $(LDFLAGS)

clean:
	rm -f byterunner
