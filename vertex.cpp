/*
delatvan@gmail.com
*/

#include "vertex.hpp"
#include "edge.hpp"

#include <fstream> 
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <limits>
#define INFTY std::numeric_limits<int>::max()

using namespace std;

vertex::vertex()
{
	iD = -1;
	rank = 0;
	height = 0;
	active=0;
	excess = 0;
	next = NULL;
	prev = NULL;
}

vertex::vertex(int iD_, int height_)
{	
	/*
	constructor for Push-Relabel
	*/
	iD = iD_;
	rank =0;
	active = 0;
	excess = 0;
	height = height_;
}

vertex::vertex(int iD_, int balance_real, int balance_copy){
	/*
	constructor for sukzessive_kuerzeste_Wege
	*/
	iD = iD_;
	balance = balance_real;
	balance_ = balance_copy;
	pot = 0;
	reachable = 0;
	l = INFTY;
	prev = NULL;
}

vertex::~vertex()
{}

void vertex::print(){
	cout<<"iD = "<<iD<<endl;
	cout<<"balance = "<<balance<<" ,balance_ = "<<balance_<<endl;
	cout<<" l = "<<l<<endl;
	cout<<" pot = "<<pot<<endl;
	cout<<" reachable = "<<reachable<<endl;
	cout<<endl;
}

/*
void vertex::print(){
	// Print for Push-Relabel
	cout<<"iD = "<<iD<<", height = "<<height<<" ,excess = "<<excess<<endl;
	cout<<"delta plus edges:"<<endl;
	for (int i = 0; i < delta_plus.size(); ++i)
	{
		cout<<delta_plus[i]<<" "<<endl;
	}
	cout<<"delta minus edges:"<<endl;
	for (int j = 0; j < delta_min.size(); ++j)
	{
		cout<<delta_min[j]<<" "<<endl;
	}
	cout<<endl;
}
*/
