/*
delatvan@gmail.com
*/

// vertex.hpp (Declaration of Class Graph)
#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <iostream>
#include <vector>

class edge;

using namespace std;


class vertex{

public:

	vertex();
	vertex(int iD_, int height_);
	vertex(int iD_, int balance_real, int balance_copy);
	~vertex();

	int iD;
	int rank;
	int excess;
	int balance;
	int balance_;
	int pot;
	int l;
	//for head vertices to tell if there's an active vector of this height
	int active;
	//if the vector is from s reachable
	bool reachable;
	//Distanzmarkierung
	int height;
	//next vertex 
	vertex * next;
	vertex * prev;
	//vector with indices of incoming edges
	vector<int> delta_min;
	//vector with indices of outgoing edges
	vector<int> delta_plus;

	void print();

private:


};


#endif