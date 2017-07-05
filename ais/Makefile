CC = gcc

ais: main.o selectslot.o AIS_TDMA.o
	$(CC) -o  ais main.o selectslot.o AIS_TDMA.o

main.o: main.c
	$(CC) -c main.c main.h

selectslot.o: selectslot.c selectslot.h
	$(CC) -c selectslot.c 

AIS_TDMA.O: AIS_TDMA.c AIS_TDMA.h
	$(CC) -c AIS_TDMA.c 

clear:
	-rm *.o ais 

