CC=g++
CXXFLAGS= -std=c++17 -Wall -Wextra -Wpedantic -Werror

all: box rebuild

box: box.cpp
	$(CC) $(CXXFLAGS) -o box box.cpp

rebuild: rebuild.cpp
	$(CC) $(CXXFLAGS) -o rebuild rebuild.cpp

clean:
	rm -f box rebuild *.box
