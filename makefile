CC=g++
CFLAGS = -g

LIBS = -lsocket -lnsl -lresolv

all: client1 client2 dhtserver

client1: client1.cpp
	$(CC) -o client1 client1.cpp $(LIBS)
client2: client2.cpp
	$(CC) -o client2 client2.cpp $(LIBS)
dhtserver: dhtserver.cpp
	$(CC) -o dhtserver dhtserver.cpp $(LIBS)


clean:
	rm -f dhtserver client2 client1 *.o
