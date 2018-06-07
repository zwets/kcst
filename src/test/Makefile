CFLAGS="-std=c++11"

all: test-baserator test-kmerator test-kmeriser test-int-sizes

clean:
	rm -f test-baserator test-kmerator test-kmeriser

test: test-baserator
	./test-baserator

test-kmeriser: test-kmeriser.cpp ../kmeriser.cpp ../utils.cpp
	g++ $(CFLAGS) -I.. -o $@ $^

test-kmerator: test-kmerator.cpp ../kmerator.cpp ../baserator.cpp ../utils.cpp
	g++ $(CFLAGS) -I.. -o $@ $^

test-baserator: test-baserator.cpp ../baserator.cpp ../utils.cpp
	g++ $(CFLAGS) -I.. -o $@ $^

test-int-sizes: test-int-sizes.cpp
	g++ $(CFLAGS) -I.. -o $@ $^

