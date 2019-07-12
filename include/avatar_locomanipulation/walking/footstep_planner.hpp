#ifndef ALM_FOOTSTEP_PLANNER_H
#define ALM_FOOTSTEP_PLANNER_H

#include <iostream>
#include <cmath>
#include <math.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <algorithm>
#include <set>

#include <memory>

using namespace std;

namespace footstep_planner{
	class Node;
	class Node_Compare;
	class Node_Compare_Fcost_Greater;

	class NodePtr_Compare;
	class NodePtr_Equality;
	class NodePtr_Compare_Fcost_Greater;


	class Node {
	public:
		Node(); // Constructor
		Node(const double x_in, const double y_in); // Constructor

		~Node(); // Destructor
		double x;
		double y;
		double g_score;
		double f_score;
		string key;

	private:
		void commonInitialization();
	};

	class Node_Compare{
	public:
		bool operator() (const Node & lhs, const Node &rhs) const;
	};

	class NodePtr_Equality{
	public:
		NodePtr_Equality(shared_ptr<Node> current_node_in);
		bool operator() (const shared_ptr<Node> & nodeObj) const;
	private:
		shared_ptr<Node> current_node_;
	};
	
	class NodePtr_Compare{
	public:
		bool operator() (const shared_ptr<Node> & lhs, const shared_ptr<Node> &rhs) const;
	};

	class Node_Compare_Fcost_Greater{
	public:
		bool operator() (const Node & lhs, const Node &rhs) const;
	};

	class NodePtr_Compare_Fcost_Greater{
	public:
		bool operator() (const shared_ptr<Node> & lhs, const shared_ptr<Node> &rhs) const;
	};

	class A_starPlanner{
	public:
		A_starPlanner();
		~A_starPlanner();
		double goalDistance(const Node neighbor, const Node goal);

		void getPath(std::vector<Node> & path);

		std::vector<Node> getNeighbors(Node & current);
		
		void doAstar();

		double gScore(const Node & current, const Node & neighbor);
	};


}



#endif