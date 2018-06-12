#!/usr/bin/make

OBJS = kcst.o seqreader.o hitcounter.o kmerdb.o vectordb.o mapdb.o kmeriser.o kmerator.o baserator.o utils.o 

HDRS = seqreader.h hitcounter.h kmerdb.h kmerise.h utils.h

CXXFLAGS += -O2 -g -Wall -Wextra -std=c++11

all: kcst

clean:
	rm -f *.o kcst

kcst: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) -c $<

