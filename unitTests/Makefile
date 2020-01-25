CC = gcc

CFLAGS = -g -Wall -lm --coverage

all: generateListCode ../studiousBroccoli.c ../studiousBroccoli.h
	$(CC) $(CFLAGS) -include ../utils/testMallocTracker/malloc_redefinition.h -L../utils/testMallocTracker -Wl,-rpath=$(shell pwd)/../utils/testMallocTracker -ltestMallocTracker -I../../jTypes ../studiousBroccoli.c main.c -o unitTests
	./unitTests
	gcov studiousBroccoli.c

generateListCode: ../../dataStructures/templates/listTemplate.h ../../dataStructures/templates/listTemplate.inc
	../../dataStructures/generateList.py ../listHeaders ../listCode transitionFnNode:transitionFnNode state:state

clean:
	rm unitTests ../listHeaders/* ../listCode/* *.gcov *.gcno *.gcda

