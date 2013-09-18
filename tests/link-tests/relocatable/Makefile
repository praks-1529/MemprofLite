# Simplistic Makefile for malloc_count example
CC = gcc
CXX = g++

CFLAGS   = -g -fPIC -W -Wall -ansi -I../../../
CXXFLAGS = -g -fPIC -W -Wall -ansi -I../../../

LDFLAGS =
#Here we need to re-compile the static lib by including the header file memproflite-static.h
LIBS = -L../../../ -lmemproflite malloc.o
#LIBS = -L../../../ -lmemproflite -L. -lmalloc-rebuilt
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
	rm -f test *.db *.so
