OBJS = sin.o
CC = g++
	DEBUG = -g
	CFLAGS = -Wall -c $(DEBUG) -std=c++11 
LFLAGS = -Wall $(DEBUG)

sin : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o sin

sin.o : sin.hh
	$(CC) $(CFLAGS) sin.cc

clean:
	\rm *.o sin
