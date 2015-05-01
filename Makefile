OBJS = market.o sin.o 
CC = g++
	DEBUG = -g
	CFLAGS = -Wall -c $(DEBUG) -std=c++11
LFLAGS = -Wall $(DEBUG)

sin : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o sin

market.o : common.hh market.hh market.cc
	$(CC) $(CFLAGS) market.cc

sin.o : abstract_user.hh sin.hh sin.cc
	$(CC) $(CFLAGS) sin.cc

clean:
	\rm *.o sin
