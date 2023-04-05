# Author:   Peter Grofcik, xgrofc00
# Project:  Turbo coder (BMS)

CXX=g++
CXXFLAGS=-O2 -g -Wall -std=c++11 -pthread -lcrypto -lssl
LDFLAGS=-Wl,-rpath=/usr/local/lib/gcc49/

all: coder.cpp sisodecoder.cpp decoder.cpp main.cpp
	$(CXX) -o bms $(CXXFLAGS) $^

clean:
	rm -f bms
