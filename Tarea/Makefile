CC	= gcc
CFLAGS	= -O2 -Wall

all: client server broker

client: client.o
server: server.o
broker: broker.o

clean:
	rm -f client server broker\
              client.o server.o broker.o

.PHONY: all clean

