CC=gcc
CFLAGS=-Wall -g -DDEBUG
LDFLAGS=
LIBS=-lpthread

all:ping

ping: ping.c
	$(CC) -o $@ $(LDFLAGS)  $^ $(LIBS)
clean:
	rm ping -f
