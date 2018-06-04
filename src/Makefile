all: kcst

clean:
	rm -f *.o kcst

kcst: kcst.cpp fastareader.cpp kmeriser.cpp kmerator.cpp baserator.cpp utils.cpp
	g++ -O2 -g -o $@ $^

