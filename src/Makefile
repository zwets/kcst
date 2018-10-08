CXXFLAGS += -std=c++14 -O3 -DNDEBUG -Wall -Wextra -pedantic -mtune=native

OBJS = khc.o templatedb.o seqreader.o vectordb.o mapdb.o kmeriser.o kmerator.o baserator.o utils.o 

LIBS =

HDRS = templatedb.h seqreader.h kmerdb.h kmerise.h utils.h

TARGET = khc

ifeq (,$(wildcard /usr/include/boost/iostreams/filter/gzip.hpp))
  $(warning "NOTE: khc will be built without gzip support, so won't be able to open gzipped fasta.")
  $(warning "      To build khc with decompression support, install the libboost-iostream-dev package,")
  $(warning "      and run 'make clean; make' again")
  CXXFLAGS += -DNO_GZIP
else
  LIBS += -Wl,-Bstatic -lboost_iostreams -lz -Wl,-Bdynamic
endif

$(TARGET): $(OBJS) $(HDRS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all: $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
	$(MAKE) -C unit-test clean

test:
	$(MAKE) -C unit-test test

%.o: %.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) -c $<

