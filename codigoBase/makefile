#CPPFLAGS=-Wall
#CXXFLAGS=-g -pg

srcs = $(wildcard *.cpp)
objs = $(srcs:%.cpp=%.o)

coopworld: $(objs)
	g++ -o $@ $^

%.o: %.cpp
	g++ -ggdb -Wno-deprecated -o $@ -c $<

Agent.o: Agent.cpp Agent.h 
World.o: World.cpp World.h 
global.o: global.cpp global.h
Grid1D.o: Grid1D.cpp Grid1D.h World.h 
Grid2D.o: Grid2D.cpp Grid2D.h World.h 
RandomGraph.o: RandomGraph.cpp RandomGraph.h World.h
BAGraph.o: BAGraph.cpp BAGraph.h World.h
SWGraph.o: SWGraph.cpp SWGraph.h World.h
NoSpace.o: NoSpace.cpp NoSpace.h World.h
prueba.o: prueba.cpp Agent.h 

clean:
	rm -f ./*.o coopworld
