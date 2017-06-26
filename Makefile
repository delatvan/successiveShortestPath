all: sukzessive_kuerzeste_wege

sukzessive_kuerzeste_wege: main.o vertex.o edge.o graph.o
	g++ main.o vertex.o edge.o graph.o -o sukzessive_kuerzeste_wege

main.o: main.cpp vertex.hpp edge.hpp graph.hpp
	g++ -c main.cpp

graph.o: graph.hpp graph.cpp edge.hpp vertex.hpp
	g++ -c graph.cpp

edge.o: edge.hpp edge.cpp vertex.hpp
	g++ -c edge.cpp

vertex.o: vertex.hpp vertex.cpp edge.hpp
	g++ -c vertex.cpp 

clean:
	rm -rf *.o sukzessive_kuerzeste_wege