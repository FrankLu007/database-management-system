all:
	g++ -o shell -Wall -O3 -g src/main.cpp
testgen:
	g++ -o testgen -Wall -O3 src/test.cpp
clean:
	rm shell
	rm result*
	rm -rf test/system/output/*
