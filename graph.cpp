/*
delatvan@gmail.com
*/

#include "graph.hpp"
#include <fstream> 
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include "vertex.hpp"
#include "edge.hpp"
#define INFTY std::numeric_limits<int>::max()

using namespace std;

//constructor (G,u,s,t)
// there are at least 2 vertex, s!=t
graph::graph(char const * filename)
{
	string buf;
	ifstream file(filename);

	if (file.is_open())
	{	
		
		getline(file,buf);
		stringstream s1(buf);
		s1>>number_vertices; 
		vertices.resize(number_vertices);
		//construction of all vertices
		for (int i = 0; i < number_vertices; ++i)
		{
		 getline(file,buf);
		 stringstream ss(buf);
		 int balance;
		 ss>>balance;
		 vertex v(i,balance,balance);
		 vertices[i]= v;
		 if (vertices[i].balance_ > 0)
		 {
		 	pos_balance.push_back(&vertices[i]);
		 }
		 if (vertices[i].balance_ < 0)
		 {
		 	neg_balance.push_back(&vertices[i]);	
		 }
		}
		getline(file,buf);
		stringstream ss(buf);
		ss>>number_edges;
		edges.resize(number_edges);
		int count = 0;
		//construction of all the edges
		while(getline(file,buf)){
			stringstream ss(buf);
			int v_iD, w_iD, capacity, cost;
			ss>>v_iD >> w_iD >>capacity>>cost;
			edge e(count,&(vertices[v_iD]), &(vertices[w_iD]), capacity, cost);
			edges[count]=e;
			vertices[v_iD].delta_plus.push_back(e.index);
			vertices[w_iD].delta_min.push_back(e.index);
			++count;
		}
	}
	file.close();
}


graph::~graph()
{}


void graph::print()
{		cout<<"Edges"<<endl;
	for(int i=0; i<edges.size(); i++){
		edges[i].print();
	}
	cout<<endl;
	cout<<"Vertices: "<<endl;
	for(int i=0; i<vertices.size(); i++){
		vertices[i].print();
	}
	cout<<endl;

}

void graph::update_excess(vertex * v){
	int incoming = 0;
	int outgoing = 0;
	for (int i = 0; i < v->delta_min.size(); ++i)
	{
		incoming += edges[v->delta_min[i]].flow;
	}
	for (int i = 0; i < v->delta_plus.size(); ++i)
	{
		outgoing += edges[v->delta_plus[i]].flow;
	}
	v->excess = incoming - outgoing;
}

void graph::active_insert(vertex & head, vertex * x){
	x->next = head.next;
	if (head.next!=NULL)
	{
		head.next->prev = x;
	}
	head.next = x;
	x->prev = NULL;
	head.active = 1;
	x->active=1;
}

void graph::active_delete(vertex & head, vertex * x){
	if (x->prev!=NULL)
	{
		x->prev->next = x->next;
	}
	else{
		head.next = x->next;
	}
	if (x->next!=NULL)
	{
		x->next->prev = x->prev;
	}
	if (head.next==NULL)
	{
		head.active = 0;
	}
	x->active = 0;
}

//insert edge into admissible edges from Gf
void graph::admissible_insert(edge & head, edge * e, int v){
	e->next_Gf = head.next;
	if (head.next!=NULL)
	{	
		//if head.next edge is in Gf, head.next=(v,w)
		if ( head.next->x->iD==v)
		{
			head.next->prev_Gf = e;
		}
		//if it corresponds to delta_minus, thus in Gf_opp
		else{
			head.next->prev_Gf_opp = e;
		}
	}
	head.next = e;
	e->prev_Gf = NULL;
	e->active_res = 1;
}

//delete edge from admissible edges from Gf
void graph::admissible_delete(edge & head, edge * e, int v){
	if (e->prev_Gf!=NULL)
	{	
		//if e.prev_Gf is in Gf
		if (e->prev_Gf->x->iD==v)
		{
			e->prev_Gf->next_Gf = e->next_Gf;
		}
		else{
			e->prev_Gf->next_Gf_opp = e->next_Gf;
		}
	}
	else{
		head.next = e->next_Gf;
	}
	if (e->next_Gf!=NULL)
	{	
		//if e.next_Gf is in Gf
		if (e->next_Gf->x->iD==v)
		{
			e->next_Gf->prev_Gf = e->prev_Gf;
		}
		else{
			e->next_Gf->prev_Gf_opp = e->prev_Gf;
		}
	}
	e->active_res = 0;

}

//insert edge into admissible edges from Gf_opp
void graph::admissible_opp_insert(edge & head, edge * e, int v){
	e->next_Gf_opp = head.next;
	if (head.next!=NULL)
	{	
		//if head.next edge is in Gf, head.next=(v,w)
		if ( head.next->x->iD==v)
		{
			head.next->prev_Gf = e;
		}
		//if it corresponds to delta_minus, thus in Gf_opp
		else{
			head.next->prev_Gf_opp = e;
		}
	}
	head.next = e;
	e->prev_Gf_opp = NULL;
	e->active_res_opp = 1;

}


//delete edge from admissible edges from Gf_opp
void graph::admissible_opp_delete(edge & head, edge * e, int v){
	/*
	We're treating e as if it belongs to delta_minus of v.
	So e is a reverse edge for Gf.So e=(w,v). Thus only 
	appearing as an admissible edge for v in the case 
	were uf(e)_opp is greater than 0. So only if
	the fluss in greater than 0.
	*/
	if (e->prev_Gf_opp!=NULL)
	{	
		//if e.prev_Gf_opp is in Gf
		if (e->prev_Gf_opp->x->iD==v)
		{
			e->prev_Gf_opp->next_Gf = e->next_Gf_opp;
		}
		else{
			e->prev_Gf_opp->next_Gf_opp = e->next_Gf_opp;
		}
	}
	else{
		head.next = e->next_Gf_opp;
	}
	if (e->next_Gf_opp!=NULL)
	{	
		//if e.next_Gf_opp is in Gf
		if (e->next_Gf_opp->x->iD==v)
		{
			e->next_Gf_opp->prev_Gf = e->prev_Gf_opp;
		}
		else{
			e->next_Gf_opp->prev_Gf_opp = e->prev_Gf_opp;
		}
	}
	e->active_res_opp = 0;

}

int graph::edge_admissible(edge * e, int v){
	//e in delta+ of v
	if (e->x->iD==v)
	{
		if (e->res_capacity>0 && e->x->height == e->y->height+1)
		{
			return 1;
		}
	}
	//e in delta- of v, e=(w,v) thus e_opp=(v,w)
	else{
		if (e->res_capacity_opp>0 && e->y->height == e->x->height+1)
		{
			return 1;
		}
	}
	return 0;
}

void graph::admissible_update(int v_iD){
	for(int j = 0; j < vertices[v_iD].delta_plus.size(); ++j)
		{
			if (edge_admissible(&edges[vertices[v_iD].delta_plus[j]],v_iD) && edges[vertices[v_iD].delta_plus[j]].active_res==0)
			{
				admissible_insert(admissible_edges[v_iD],&edges[vertices[v_iD].delta_plus[j]],v_iD);
			}
		}
		for(int j = 0; j < vertices[v_iD].delta_min.size(); ++j)
		{
			if (edge_admissible(&edges[vertices[v_iD].delta_min[j]],v_iD) && edges[vertices[v_iD].delta_min[j]].active_res_opp==0)
			{
				admissible_opp_insert(admissible_edges[v_iD],&edges[vertices[v_iD].delta_min[j]],v_iD);
			}
		}
}

void graph::admissible_init(){
	for (int i = 0; i < number_vertices; ++i)
	{
		admissible_update(i);
	}
}

int graph::create_path(vertex * r){
	vertex * p = r;
	int u = INFTY;
	while(p->prev != NULL){
			edge* e = NULL;
			
			//create path 
			for (int i = 0; i < p->delta_min.size(); ++i)
			{
				if (edges[p->delta_min[i]].res_capacity>0 && edges[p->delta_min[i]].x==p->prev)
				{
					e = &edges[p->delta_min[i]];
					e->used_as = 1;
				}
			}
			for (int i = 0; i < p->delta_plus.size(); ++i)
			{
				if (edges[p->delta_plus[i]].res_capacity_opp>0 && edges[p->delta_plus[i]].y==p->prev)
				{	
					//If there is no edge found in delta_min then e==NULL
					if (e == NULL)
					{
						e = &edges[p->delta_plus[i]];
						e->used_as = -1;
					}
					//In case there are edges (t,p) and (p,t) so that both appear as (p,t) in Gf
					//choose the one with smallest value
					
					else if (e->cost > edges[p->delta_plus[i]].cost)
					{
						e->used_as = 0;
						e = &edges[p->delta_plus[i]];
						e->used_as = -1;
					}
				}
			}
			path.push_back(e);
			switch(e->used_as){
				case -1:{
					if (e->res_capacity_opp < u)
					{
						u = e->res_capacity_opp;
					}
					break;
				}
				case 1:{
					if (e->res_capacity < u)
					{
						u = e->res_capacity;
					}
					break;
				}
				default: {
					cout<<"ERROR"<<endl;
					break;
				}
			}
			p = p->prev;
			
		}
	return u;
}

void graph::augment_path(int gamma){
	int path_size = path.size();
	for (int i = 0; i < path_size; ++i)
		{	
			if (path.back()->used_as == -1)
			{
				path.back()->update_flow(-gamma);
			}
			else{
				path.back()->update_flow(gamma);
			}
			path.back()->used_as = 0;
			path.pop_back();
		}
}

