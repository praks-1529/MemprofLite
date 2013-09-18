CC = gcc
CXX = g++

CFLAGS   = -g -fPIC -W -Wall -ansi -I. -I./third_party/include 
#CXXFLAGS = -g -fPIC -W -Wall -ansi -I. -I./third_party/include -DCUSTOM_ALLOC
#CXXFLAGS = -g -fPIC -W -Wall -ansi -I. -I./third_party/include -DSTART_STATIC
CXXFLAGS = -g -fPIC -W -Wall -ansi -I. -I./third_party/include -DSTART_STATIC -DTEXT_OP -DDEMANGLE

LDFLAGS =
LIBS = -L./third_party/sqlite3/lib/ -lsqlite3 -ldl -lpthread -lbfd -liberty
OBJS = memproflite.o MemprofLite.o MemprofLiteExecInfoWriter.o MemprofLiteHook.o MemprofLiteDB.o MemprofLiteUtils.o

all: test

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

#Create a shared lib 
memproflite : $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -shared -fPIC -Wl -o lib$@.so $(OBJS) $(LIBS)

#Create a static lib
libmemproflite.a : $(OBJS)
	ar rcs $@ $(OBJS) 

#Link with main exec
test: $(OBJS) memproflite
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ main.cc -L. -lmemproflite

clean:
	rm -f *.o test *.db *.so *db-journal *.txt
