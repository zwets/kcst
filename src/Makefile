CXXFLAGS += -std=c++14 -O3 -DNDEBUG -Wall -Wextra -pedantic -mtune=native

OBJS = khc.o templatedb.o seqreader.o vectordb.o mapdb.o kmeriser.o kmerator.o baserator.o utils.o 

LIBS =

HDRS = templatedb.h seqreader.h kmerdb.h kmerise.h utils.h

TARGET = khc

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

