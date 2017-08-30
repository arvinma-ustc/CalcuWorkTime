CC = gcc

ais: main.o selectslot.o AIS_TDMA.o transport.o
	$(CC) -o  ais main.o selectslot.o AIS_TDMA.o transport.o

main.o: main.c
	$(CC) -c main.c main.h

selectslot.o: selectslot.c selectslot.h
	$(CC) -c selectslot.c 


transport.O: transport.c transport.h
	$(CC) -c transport.c 

AIS_TDMA.O: AIS_TDMA.c AIS_TDMA.h
	$(CC) -c AIS_TDMA.c 

clear:
	 rm -f *.o ais turbo


turbo: turbo.o
	$(CC) -o turbo turbo.o 


turbo.o: turbo.c turbo.h
	$(CC) -c turbo.c  turbo.h
