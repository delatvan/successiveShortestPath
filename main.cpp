#include <iostream>
#include <fstream> 
#include "vertex.hpp"
#include "edge.hpp"
#include "graph.hpp"
#include <vector>
#include <algorithm>
#include <limits>
#define INFTY std::numeric_limits<int>::max()
using namespace std;


void dijkstra(vertex * s, graph * g){
	vector<vertex*> q(g->number_vertices);
	int min_potential = 0;
	for (int i = 0; i < g->vertices.size(); ++i)
	{	

		//set each l of v in V(G)\{s} to infty
		g->vertices[i].l = INFTY;
		g->vertices[i].reachable = 0;
		g->vertices[i].prev = NULL;
		//Create Q
		q[i]=&(g->vertices[i]);
	}
	//set l of s to 0
	s->l = 0;
	s->reachable = 1;


	while(q.size()!=0){
		int l = INFTY;
		vertex * v; 
		//look for vertex with minimal l(v)
		for (int i = 0; i < q.size(); ++i)
		{
			if (q[i]->l<=l)
			{	
				
				l= q[i]->l;
				//pointer to the vector with minimal l(v) in q
				v = q[i];
			}
		}
		//delete v from q
		q.erase(remove(q.begin(),q.end(),v),q.end());


		//case when only l(v)= infty remain, not reached by s
		if (v->reachable == 0)
		{	
			v->pot += min_potential;
			for (int i = 0; i < q.size(); ++i)
			{
				q[i]->pot += min_potential;
			}
			return;
		}
		
		
		//replacing l(v) with new minimum for every element in q
		for (int i = 0; i < v->delta_min.size(); ++i)
		{
			/* Checks two things:
			for y in q with (x,y) in E(Gf)
			In this case checking the incoming edges (y,x) in G which appear as outgoing in Gf (x,y)
			iff uf_opp >0
			*/
			if (g->edges[v->delta_min[i]].res_capacity_opp > 0 && 
				find(q.begin(),q.end(),g->edges[v->delta_min[i]].x)!=q.end() )
			{	
				if (g->edges[v->delta_min[i]].x->l > 
					v->l - g->edges[v->delta_min[i]].cost -  v->pot + g->edges[v->delta_min[i]].x->pot )
				{	
					g->edges[v->delta_min[i]].x->l = v->l - g->edges[v->delta_min[i]].cost +
					 g->edges[v->delta_min[i]].x->pot - v->pot;
					g->edges[v->delta_min[i]].x->prev = v;
					g->edges[v->delta_min[i]].x->reachable = 1;
	
				}
				
			}
		}
		for (int i = 0; i < v->delta_plus.size(); ++i)
		{	
			/* Checks two things:
			for y in q with (x,y) in E(Gf)
			1. If the edge actually appears in Gf as an outgoing edge and thus y can be updated
			2. checks if y, the incoming vertex, is in q
			*/
			if (g->edges[v->delta_plus[i]].res_capacity > 0 && 
				find(q.begin(),q.end(),g->edges[v->delta_plus[i]].y)!=q.end() )
			{	
				if (g->edges[v->delta_plus[i]].y->l > 
					v->l+ (g->edges[v->delta_plus[i]].cost - v->pot + g->edges[v->delta_plus[i]].y->pot))
				{	
					g->edges[v->delta_plus[i]].y->l = v->l+ g->edges[v->delta_plus[i]].cost -
					 v->pot + g->edges[v->delta_plus[i]].y->pot;
					g->edges[v->delta_plus[i]].y->prev = v;
					g->edges[v->delta_plus[i]].y->reachable = 1;
				}
			}
		}
		v->pot -= v->l;
		min_potential -= v->l;
	}
	
	return;
}



int main(int argc, char* argv[]){
	
	clock_t begin = std::clock();

	graph g(argv[1]);
	
	
	while(g.pos_balance.size() != 0 && g.neg_balance.size() != 0){

		vector<vertex *>to_append; 
		vector<edge* > path;
		bool cont = 1;

		vertex* s = g.pos_balance.back();

		g.pos_balance.pop_back();
		vertex* t;


		dijkstra(s,&g);


		//Look for t with balance_ <0 with a path from s->t
		while(cont){
			t = g.neg_balance.back();
 			g.neg_balance.pop_back();
			if(t->reachable == 0){
				to_append.push_back(t);
				if(g.neg_balance.size()==0){
 					cout<<"BREAK no b-flow"<<endl;
 					return 0;
 				}
			}
			else{
				break;
			}			
		}


		int u = g.create_path(t);
	

		/*Reinsert all the vectors with balance_ <0 for which there's not path
		from s to t. There can afterwards exist a path.
		*/
		g.neg_balance.insert(g.neg_balance.end(), to_append.begin(), to_append.end());
		to_append.clear();
		
		
		int set[] = {u,s->balance_,-t->balance_};
		int gamma = *std::min_element(set,set+3);

		g.augment_path(gamma);


		s->balance_ -= gamma;
		if (s->balance_ > 0)
		{
			g.pos_balance.push_back(s);
		}
		t->balance_ += gamma;
		if (t->balance_ <0)
		{
			g.neg_balance.push_back(t);
		}
			
	}

	ofstream to_write("result.txt");
	if(to_write.is_open()){


	
	
	int output = 0;
	for (int i = 0; i < g.edges.size(); ++i)
	{	
		output += g.edges[i].flow * g.edges[i].cost;
	}
	to_write<<output<<endl;
	cout<<output<<endl;
	for (int i = 0; i < g.edges.size(); ++i)
	{
		if (g.edges[i].flow > 0 )
		{
			to_write<<g.edges[i].index<<" "<<g.edges[i].flow<<endl;
			cout<<g.edges[i].index<<" "<<g.edges[i].flow<<endl;
			
		}
	}
	
	}
	to_write.close();
	
	clock_t end = std::clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    std::cout << elapsed_secs << std::endl;
	return 0;

}
