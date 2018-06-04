all: test-baserator test-kmerator

test: test-baserator
	./test-baserator

test-kmerator: test-kmerator.cpp ../kmerator.cpp ../baserator.cpp ../utils.cpp
	g++ -I.. -o $@ $^

test-baserator: test-baserator.cpp ../baserator.cpp ../utils.cpp
	g++ -I.. -o $@ $^

