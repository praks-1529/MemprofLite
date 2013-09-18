# Simplistic Makefile for malloc_count example
CC = gcc
CXX = g++

CFLAGS   = -g -fPIC -W -Wall -ansi -I../../../
CXXFLAGS = -g -fPIC -W -Wall -ansi -I../../../

LDFLAGS =

LIBS = -L../../.. -lmemproflite -L. -lmalloc -lpthread
#LIBS = -L. -lmalloc -L../../.. -lmemproflite -lpthread

OBJS = 

all: test

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

#Link with main exec
test: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ main.cc $(LIBS)

clean:
	rm -f *.o test *.db
