#!/usr/bin/make

OBJS = khc.o templatedb.o seqreader.o kmerdb.o vectordb.o mapdb.o kmeriser.o kmerator.o baserator.o utils.o 

HDRS = templatedb.h seqreader.h kmerdb.h kmerise.h readwrite.h utils.h

CXXFLAGS += -O2 -g -Wall -Wextra -std=c++14

all: khc

clean:
	rm -f *.o khc

khc: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) -c $<

