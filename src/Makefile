CXXFLAGS += -O2 -g -Wall -Wextra -std=c++14

OBJS = khc.o templatedb.o seqreader.o kmerdb.o vectordb.o mapdb.o kmeriser.o kmerator.o baserator.o utils.o 

LIBS =

HDRS = templatedb.h seqreader.h kmerdb.h kmerise.h utils.h

TARGET = khc

$(TARGET): $(OBJS) $(HDRS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all: $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

%.o: %.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) -c $<

