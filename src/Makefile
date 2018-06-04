all: kcst

clean:
	rm -f *.o kcst

kcst: kcst.cpp fastareader.cpp sequence.cpp converters.cpp kmeriser.cpp kmerator.cpp baserator.cpp utils.cpp
	g++ -O2 -g -o $@ $^

