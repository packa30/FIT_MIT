# Author:   Peter Grofcik, xgrofc00
# Project:  Vigenere cipher password finder (KRY)

CXX=g++
CXXFLAGS=-O2 -g -Wall -std=c++11 -pthread -lcrypto -lssl
LDFLAGS=-Wl,-rpath=/usr/local/lib/gcc49/

all: friedman.cpp kasisk.cpp main.cpp
	$(CXX) -o kry $(CXXFLAGS) $^

clean:
	rm -f kry