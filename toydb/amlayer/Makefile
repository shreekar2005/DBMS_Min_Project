CC = cc
CFLAGS = -g -c
LDFLAGS = -g

OBJS = am.o amfns.o amsearch.o aminsert.o amstack.o amglobals.o main.o amscan.o amprint.o misc.o

PF_OBJ = ../pflayer/pflayer.o

TARGET = a.out

$(TARGET): $(OBJS) $(PF_OBJ)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJS) $(PF_OBJ)

amlayer.o : am.o amfns.o amsearch.o aminsert.o amstack.o amglobals.o amscan.o amprint.o
	ld -r am.o amfns.o amsearch.o aminsert.o  amstack.o amglobals.o amscan.o amprint.o  -o amlayer.o

am.o : am.c am.h pf.h
	$(CC) $(CFLAGS) am.c

amfns.o : amfns.c am.h pf.h
	$(CC) $(CFLAGS) amfns.c

amsearch.o : amsearch.c am.h pf.h
	$(CC) $(CFLAGS) amsearch.c

aminsert.o : aminsert.c am.h pf.h
	$(CC) $(CFLAGS) aminsert.c

amscan.o : amscan.c am.h pf.h
	$(CC) $(CFLAGS) amscan.c

amstack.o : amstack.c am.h pf.h
	$(CC) $(CFLAGS) amstack.c

amglobals.o : amglobals.c am.h
	$(CC) $(CFLAGS) amglobals.c

amprint.o : amprint.c am.h pf.h 
	$(CC) $(CFLAGS) amprint.c

misc.o : misc.c am.h pf.h testam.h
	$(CC) $(CFLAGS) misc.c
    
main.o : main.c am.h pf.h 
	$(CC) $(CFLAGS) main.c

clean:
	rm -f *.o *.out $(TARGET) amlayer.o

loadtest1: test1.c
	cp test1.c main.c
loadtest2: test2.c
	cp test2.c main.c
loadtest3: test3.c
	cp test3.c main.c

.PHONY: clean loadtest1 loadtest2 loadtest3