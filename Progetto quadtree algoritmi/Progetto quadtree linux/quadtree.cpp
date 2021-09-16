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

	NodePoint<T>* root = NULL;
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
	NodePoint<T>* nodo = new NodePoint<T>();

	PointQuadtree<T>(T x1, T y1, string key) {
		nodo->point->x = x1;
		nodo->point->y = y1;
		nodo->aKey = key;
	}
	
	PointQuadtree<T>* insertPoint(T x, T y, string key) {
		Point<T>* addPoint = new Point<T>(x, y);
		NodePoint<T>* tmp = nodo;
		NodePoint<T>* root = nodo;
		while (tmp != NULL) {
			root = tmp;
			if (x >= tmp->point->x) {
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
		tmp1->root = root;
		tmp1->aKey = key;
		if (x >= root->point->x) {
			if (y >= root->point->y) {
				root->NE = tmp1;
			}
			else {
				root->SE = tmp1;
			}
		}
		else {
			if (y >= root->point->y) {
				root->NW = tmp1;
			}
			else {
				root->SW = tmp1;
			}
		}
		return this;
	}

	bool search(T x, T y) {
		NodePoint<T>* tmp = nodo;
		Point<T> searched(x, y);
		while ((tmp != NULL) && (tmp->point->isSamePoint(searched) == false)) {
			if (x >= tmp->point->x) {
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
		else
			return true;
	}

	bool cancel(T x, T y) {
		Point<T> canceled(x, y);
		NodePoint<T>* tmp = nodo;
		while (tmp != NULL && tmp->point->isSamePoint(canceled) != true) {
			if (x >= tmp->point->x) {
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
		else {//controllo se nodo foglia
			if (tmp->NE == NULL && tmp->NW == NULL && tmp->SW == NULL && tmp->SE == NULL) { 
				tmp->root = NULL;
				delete tmp;
				return true;
			}
			else
				return false;
		}
	}

	bool keyPresence(string key) {
		queue<NodePoint<T>*> coda;
		coda.push(nodo);
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
	}
};


template <class T>
struct NodePointList {
public:
	vector<Point<T>*> listPoint; //ho usato il vector e non list+iterator per semplicità
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
	NodePointList<T>* nodo = new NodePointList<T>();

	PointQuadtreeList<T>(T x1, T y1) {
		Point<T>* ins = new Point<T>(x1, y1);
		nodo->listPoint.push_back(ins);
	} 
	
	void Subdivide(NodePointList<T>* fix, Point<T>* to_ins) {
		float mediax = 0;
		float mediay = 0;
		for (int i = 0; i < fix->dim; i++) {
			mediax += fix->listPoint[i]->x;
			mediay += fix->listPoint[i]->y;
		}

		mediax = mediax / fix->dim;
		mediay = mediay / fix->dim;
		float bestChoiceArray[10];

		for (int i = 0; i < fix->dim; i++)
			bestChoiceArray[i] = abs(fix->listPoint[i]->x - mediax) + abs(fix->listPoint[i]->y - mediay);

		float min = bestChoiceArray[0];
		int index = 0;
		for (int i = 1; i < fix->dim; i++) {
			if (min > bestChoiceArray[i]) {
				min = bestChoiceArray[i];
				index = i;
			}
		}
		Point<T>* best = new Point<T>();
		best = fix->listPoint[index];
		fix->SplitPoint = best;
		fix->listPoint.erase(fix->listPoint.begin() + index);
		
		fix->NE = new NodePointList<T>();
		fix->SE = new NodePointList<T>();
		fix->NW = new NodePointList<T>();
		fix->SW = new NodePointList<T>();

		for (int i = 0; i < fix->dim - 1; i++) {
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

		if (to_ins->x >= fix->SplitPoint->x) {
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
		NodePointList<T>* tmp = nodo;
		Point<T>* to_ins = new Point<T>(x, y);
		while (tmp->SplitPoint != NULL) {
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
		if (tmp->listPoint.size() == tmp->dim)
			Subdivide(tmp, to_ins);
		else
			tmp->listPoint.push_back(to_ins);
		return this;
	}

	bool search(T x, T y) {
		NodePointList<T>* tmp = nodo;
		Point<T> searched(x, y);
		while (tmp->SplitPoint != NULL) {
			if (tmp->SplitPoint->isSamePoint(searched) == true) //messo qui e non nel while perché se esce con valore null allora si genera una eccezione in lettura
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
		for (unsigned int i = 0; i < tmp->listPoint.size(); i++) {
			if (tmp->listPoint[i]->isSamePoint(searched) == true)
				return true;
		}
		return false;
	}
	
	bool cancel(T x, T y) {
		NodePointList<T>* tmp = nodo;
		Point<T> canceled(x, y);
		while (tmp->SplitPoint != NULL) {
			if (tmp->SplitPoint->isSamePoint(canceled))
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
		for (unsigned int i = 0; i < tmp->listPoint.size(); i++) {
			if (tmp->listPoint[i]->isSamePoint(canceled) == true) {
				tmp->listPoint.erase(tmp->listPoint.begin() + i);
				return true;
			}
		}
		return false;
	}
	
};
