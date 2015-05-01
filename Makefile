OBJS = sin.o market.o user.o
CC = g++
	DEBUG = -g
	CFLAGS = -Wall -c $(DEBUG) -std=c++11
LFLAGS = -Wall $(DEBUG)

sin : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o sin

market.o : common.hh market.hh market.cc
	$(CC) $(CFLAGS) market.cc

user.o : common.hh user.hh user.cc
	$(CC) $(CFLAGS) user.cc

sin.o : sin.hh sin.cc
	$(CC) $(CFLAGS) sin.cc

clean:
	\rm *.o sin
