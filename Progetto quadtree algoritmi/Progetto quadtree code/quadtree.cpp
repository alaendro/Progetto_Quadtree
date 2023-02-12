#include <vector>
#include <string>
#include <queue>
#include <cmath>
#include <iostream>
using namespace std;

template <class T>
class Point {  
public:
	T x;
	T y;
	Point<T>(T x, T y) {
		this->x = x;
		this->y = y;
	}

	Point<T>() {
		x = 0;
		y = 0;
	}

	bool isSamePoint(Point<T> check) {
		if (x == check.x && y == check.y)
			return true;
		else
			return false;
	}
};


template <class T>
struct NodePoint {
public:
	Point<T>* point = new Point<T>();
	string aKey = "";

	NodePoint<T>* root = NULL; //called root because the quadtree is a recursive structure
	NodePoint<T>* NW = NULL;
	NodePoint<T>* NE = NULL;
	NodePoint<T>* SW = NULL;
	NodePoint<T>* SE = NULL;
};

/*NW   |   NE
       |
--------------
       |
  SW   |   SE
*/	

template <class T>
class PointQuadtree {
public:
	NodePoint<T>* node = new NodePoint<T>();

	PointQuadtree<T>(T x1, T y1, string key) {
		node->point->x = x1;
		node->point->y = y1;
		node->aKey = key;
	}
	
	PointQuadtree<T>* insertPoint(T x, T y, string key) {
		Point<T>* addPoint = new Point<T>(x, y);
		NodePoint<T>* tmp = node; //temporary node used for scroll inside the quadtree
		NodePoint<T>* father = node;
		while (tmp != NULL) { //if it is null you have reached the end
			father = tmp; //at the end, the father node will be taken in which to insert the child
			if (x >= tmp->point->x) { //the child future position is searched based on x, y coordinates
				if (y >= tmp->point->y) {
					tmp = tmp->NE;
				}
				else {
					tmp = tmp->SE;
				}
			}
			else {
				if (y >= tmp->point->y) {
					tmp = tmp->NW;
				}
				else {
					tmp = tmp->SW;
				}
			}
		}
		NodePoint<T>* tmp1 = new NodePoint<T>();
		tmp1->point = addPoint;
		tmp1->root = father;
		tmp1->aKey = key; 
		if (x >= father->point->x) { //the child is inserted based on x, y coordinates
			if (y >= father->point->y) {
				father->NE = tmp1;
			}
			else {
				father->SE = tmp1;
			}
		}
		else {
			if (y >= father->point->y) {
				father->NW = tmp1;
			}
			else {
				father->SW = tmp1;
			}
		}
		return this;
	}

	bool search(T x, T y) {
		NodePoint<T>* tmp = node;
		Point<T> searched(x, y);
		while ((tmp != NULL) && (tmp->point->isSamePoint(searched) == false)) {
			if (x >= tmp->point->x) { //the position is searched based on x, y coordinates
				if (y >= tmp->point->y) {
					tmp = tmp->NE;
				}
				else {
					tmp = tmp->SE;
				}
			}
			else {
				if (y >= tmp->point->y) {
					tmp = tmp->NW;
				}
				else {
					tmp = tmp->SW;
				}
			}
		}
		if (tmp == NULL) 
			return false; //the leaf node was not found in that position so it does not exist
		else
			return true;
	}

	bool cancel(T x, T y) {
		Point<T> canceled(x, y);
		NodePoint<T>* tmp = node;
		while (tmp != NULL && tmp->point->isSamePoint(canceled) != true) { 
			if (x >= tmp->point->x) { //the position is searched based on x, y coordinates
				if (y >= tmp->point->y) {
					tmp = tmp->NE;
				}
				else {
					tmp = tmp->SE;
				}
			}
			else {
				if (y >= tmp->point->y) {
					tmp = tmp->NW;
				}
				else {
					tmp = tmp->SW;
				}
			}
		}
		if (tmp == NULL)
			return false;
		else { //check if is a leaf node
			if (tmp->NE == NULL && tmp->NW == NULL && tmp->SW == NULL && tmp->SE == NULL) { 
				tmp->root = NULL;
				delete tmp;
				return true;
			}
			else //Deleting a point is a possible operation if and only if the point in question is in a leaf node, deleting a split point would cause inconsistency in the quadtree structure.
				return false;
		} //TODO rename this function with cancelPoint and create another one that consider a node recursively (scegli uno split point dai figli e così fino alle foglie se anche loro sono padri)
	}

	bool keyPresence(string key) {
		queue<NodePoint<T>*> coda; //queue used to parse all nodes in branch order
		coda.push(node);
		NodePoint<T>* tmp = new NodePoint<T>();
		while (tmp->aKey != key && coda.empty() != true) {
			tmp = coda.front();
			coda.pop();
			if (tmp->NE != NULL)
				coda.push(tmp->NE);
			if (tmp->NW != NULL)
				coda.push(tmp->NW);
			if (tmp->SE != NULL)
				coda.push(tmp->SE);
			if (tmp->SW != NULL)
				coda.push(tmp->SW);
		}
		if (tmp->aKey == key)
			return true;
		else
			return false;
	} //TODO crearne una che modifica la chiave
};


template <class T>
struct NodePointList {
public:
	vector<Point<T>*> listPoint; //vector was used instead of list+iterator for simplicity
	Point<T>* SplitPoint = NULL;
	int dim = 10;

	NodePointList<T>* NW = NULL;
	NodePointList<T>* NE = NULL;
	NodePointList<T>* SW = NULL;
	NodePointList<T>* SE = NULL;
};


template <class T>
class PointQuadtreeList {
public:
	NodePointList<T>* node = new NodePointList<T>();

	PointQuadtreeList<T>(T x1, T y1) {
		Point<T>* ins = new Point<T>(x1, y1);
		node->listPoint.push_back(ins);
	} 
	
	void Subdivide(NodePointList<T>* fix, Point<T>* to_ins) {
		float averageX = 0;
		float averageY = 0;
		for (int i = 0; i < fix->dim; i++) {
			averageX += fix->listPoint[i]->x;
			averageY += fix->listPoint[i]->y;
		}

		averageX = averageX / fix->dim;
		averageY = averageY / fix->dim;
		float bestChoiceArray[10]; //indicates how close each node point is to the average of node points

		for (int i = 0; i < fix->dim; i++)
			bestChoiceArray[i] = abs(fix->listPoint[i]->x - averageX) + abs(fix->listPoint[i]->y - averageY);

		float min = bestChoiceArray[0];
		int index = 0;
		for (int i = 1; i < fix->dim; i++) { //the minimum value is sought because it is the closest to the average of the points
			if (min > bestChoiceArray[i]) {
				min = bestChoiceArray[i];
				index = i;
			}
		}
		Point<T>* best = new Point<T>();
		best = fix->listPoint[index];
		fix->SplitPoint = best;
		fix->listPoint.erase(fix->listPoint.begin() + index); //erase the point that is used as split point 
		
		fix->NE = new NodePointList<T>();
		fix->SE = new NodePointList<T>();
		fix->NW = new NodePointList<T>();
		fix->SW = new NodePointList<T>();

		for (int i = 0; i < fix->dim - 1; i++) { //take every point of the old node and place it in a different node according to the position of the split point
			Point<T>* ins = new Point<T>(fix->listPoint[i]->x, fix->listPoint[i]->y);
			if (fix->listPoint[i]->x >= fix->SplitPoint->x) {
				if (fix->listPoint[i]->y >= fix->SplitPoint->y) {
					fix->NE->listPoint.push_back(ins);
				}
				else {
					fix->SE->listPoint.push_back(ins);
				}
			}
			else {
				if (fix->listPoint[i]->y >= fix->SplitPoint->y) {
					fix->NW->listPoint.push_back(ins);
				}
				else {
					fix->SW->listPoint.push_back(ins);
				}
			}
		}

		if (to_ins->x >= fix->SplitPoint->x) { //the point that triggered the subdivide and has to be inserted
			if (to_ins->y >= fix->SplitPoint->y) {
				fix->NE->listPoint.push_back(to_ins);
			}
			else {
				fix->SE->listPoint.push_back(to_ins);
			}
		}
		else {
			if (to_ins->y >= fix->SplitPoint->y) {
				fix->NW->listPoint.push_back(to_ins);
			}
			else {
				fix->SW->listPoint.push_back(to_ins);
			}
		}
//		cout << "SplitPoint " << fix->SplitPoint->x << " " << fix->SplitPoint->y << endl;
		fix->listPoint.clear();
	}
	
	PointQuadtreeList<T>* insertPoint(T x, T y) {
		NodePointList<T>* tmp = node;
		Point<T>* to_ins = new Point<T>(x, y);
		while (tmp->SplitPoint != NULL) { //the child future position is searched based on x, y coordinates
			if (x >= tmp->SplitPoint->x) { 
				if (y >= tmp->SplitPoint->y) {
					tmp = tmp->NE;
				}
				else {
					tmp = tmp->SE;
				}
			}
			else {
				if (y >= tmp->SplitPoint->y) {
					tmp = tmp->NW;
				}
				else {
					tmp = tmp->SW;
				}
			}
		}
		if (tmp->listPoint.size() == tmp->dim) //if the node size reaches its maximum, a split point is created and the points are divided between new children
			Subdivide(tmp, to_ins);
		else
			tmp->listPoint.push_back(to_ins);
		return this;
	}

	bool search(T x, T y) {
		NodePointList<T>* tmp = node;
		Point<T> searched(x, y);
		while (tmp->SplitPoint != NULL) { //the child position is searched based on x, y coordinates
			if (tmp->SplitPoint->isSamePoint(searched) == true) //put here and not in the while because if it comes out with a null value then a read exception is thrown
				return true;
			if (x >= tmp->SplitPoint->x) {
				if (y >= tmp->SplitPoint->y) {
					tmp = tmp->NE;
				}
				else {
					tmp = tmp->SE;
				}
			}
			else {
				if (y >= tmp->SplitPoint->y) {
					tmp = tmp->NW;
				}
				else {
					tmp = tmp->SW;
				}
			}
		}
		for (unsigned int i = 0; i < tmp->listPoint.size(); i++) { //check if the point it's inside the last node
			if (tmp->listPoint[i]->isSamePoint(searched) == true)
				return true;
		}
		return false;
	}
	
	bool cancel(T x, T y) {
		NodePointList<T>* tmp = node;
		Point<T> canceled(x, y);
		while (tmp->SplitPoint != NULL) { //the child position is searched based on x, y coordinates
			if (tmp->SplitPoint->isSamePoint(canceled)) //deleting a split point would cause inconsistency in the quadtree structure
				return false;
			if (x >= tmp->SplitPoint->x) {
				if (y >= tmp->SplitPoint->y) {
					tmp = tmp->NE;
				}
				else {
					tmp = tmp->SE;
				}
			}
			else {
				if (y >= tmp->SplitPoint->y) {
					tmp = tmp->NW;
				}
				else {
					tmp = tmp->SW;
				}
			}
		}
		for (unsigned int i = 0; i < tmp->listPoint.size(); i++) { //check if the point it's inside the last node
			if (tmp->listPoint[i]->isSamePoint(canceled) == true) {
				tmp->listPoint.erase(tmp->listPoint.begin() + i);
				return true;
			}
		}
		return false;
	}
	//TODO si può chiamare la subdivideErase() che permette di cancellare split point e che viene richiamata ricorsivamente nel caso il nuovo split point fa da split point per un sotto quadtree
};
