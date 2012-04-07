CXX = g++
CXXOPTS = -ggdb -Wall -Wextra -std=gnu++0x -Dnullptr=NULL



.PHONY: all

all: basico



%.o: %.cc
	$(CXX) $(CXXOPTS) -o $@ -c $<



basico: basico.o
	$(CXX) $(CXXOPTS) -o $@ $^

basico.o: basico.cc
