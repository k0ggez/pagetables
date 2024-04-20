#Will Goodrum (wcg9ev)
CC:= clang
CFLAGS:= -c
LDFLAGS:= -o
.PHONY: all clean

all: libmlpt.a

libmlpt.a: libmlpt.a(main.o)
	ranlib libmlpt.a

main.o: main.c mlpt.h config.h
	$(CC) $(CFLAGS) main.c $(LDFLAGS) main.o

clean:
	rm *.o *.a