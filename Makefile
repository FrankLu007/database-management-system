all:
	g++ -o shell -Wall -O3 -g src/main.cpp
clean:
	rm shell
	rm -rf test/system/output/*
