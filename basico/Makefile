CXX     := g++
CXXOPTS := -ggdb -Wall -Wextra -std=gnu++0x -Dnullptr=NULL
LDLIBS  := boost_program_options



.PHONY: all

all: basico



%.o: %.cc
	$(CXX) $(CXXOPTS) -o $@ -c $<



basico: basico.o
	$(CXX) $(CXXOPTS) -o $@ $^ $(LDLIBS:%=-l%)

basico.o: basico.cc
