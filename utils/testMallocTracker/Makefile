CC = gcc

CFLAGS = -g -Wall

testMallocTracker: generateListCode testMallocTracker.h testMallocTracker.c
	$(CC) -I../../../jTypes testMallocTracker.c $(CFLAGS) -c -fpic
	$(CC) -shared -o libtestMallocTracker.so testMallocTracker.o

generateListCode: ../../../dataStructures/templates/listTemplate.h ../../../dataStructures/templates/listTemplate.inc
	../../../dataStructures/generateList.py listHeaders listCode ptrTable:ptrTableNode

clean: 
	rm *.so *.o listCode/* listHeaders/*
