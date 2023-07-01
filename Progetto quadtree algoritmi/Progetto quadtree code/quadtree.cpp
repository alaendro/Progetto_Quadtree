#include <vector>
#include <string>
#include <queue>
#include <cmath>
#include <iostream>
#include <tuple>
#include <climits>
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

	bool cancelLeaf(T x, T y) { //delete only if it's a leaf
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
			else
				return false;
		}
	}

	NodePoint<T>* getBalancedNode(NodePoint<T>* toRebalance){ //take the children's node, which of all is the most balanced
		float avgX, avgY, validNode = 0; 
		NodePoint<T>* nodeArray[4] = {{toRebalance->NE}, {toRebalance->SE}, {toRebalance->NW}, {toRebalance->SW}};

		for(int i = 0; i < 4; i++){
			if(nodeArray[i] != NULL){
				avgX += nodeArray[i]->point->x;
				avgY += nodeArray[i]->point->y;
				validNode++;
			}
		}

		if(validNode == 0) //it means that the node is a leaf
			return NULL;
		
		avgX = avgX / validNode;
		avgY = avgY /validNode;

		float bestChoiceArray[4]; //indicates how close each node point is to the average of node points
		for(int i = 0; i < 4; i++)
			bestChoiceArray[i] = INT_MAX;

		for (int i = 0; i < 4; i++){
			if(nodeArray[i] != NULL)
				bestChoiceArray[i] = abs(toRebalance->point->x - abs(nodeArray[i]->point->x - avgX)) + abs(toRebalance->point->y - abs(nodeArray[i]->point->y - avgY));
		}

		float min = bestChoiceArray[0];
		int index = 0;
		for (int i = 1; i < 4; i++) { //the minimum value is sought because it is the closest to the average of the points
			if (min > bestChoiceArray[i]) {
				min = bestChoiceArray[i];
				index = i;
			}
		}
		return nodeArray[index];
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
		else { //delete the point and recompone the structure
			//take the nearest point for avoid high unbalanced structure
			NodePoint<T>* bestNode = getBalancedNode(tmp);

			if(bestNode == NULL){ //it's a leaf node
				delete tmp;
				return true;
			}

			bestNode->root = tmp->root; //avoids problems of unreachability, so the new node takes the old parent and the new position

			//handling the childs positions
			NodePoint<T>* nodeArray[4] = {{tmp->NE}, {tmp->SE}, {tmp->NW}, {tmp->SW}};

			for(int i = 0; i < 4; i++){
				if(nodeArray[i] == NULL)
					continue;
				else if(nodeArray[i] == bestNode)
					continue;

				if (bestNode->point->x >= tmp->point->x) {
					if (bestNode->point->y >= tmp->point->y) {
						bestNode->NE = nodeArray[i];
					}
					else {
						bestNode->SE = nodeArray[i];
					}
				}
				else {
					if (bestNode->point->y >= tmp->point->y) {
						bestNode->NW = nodeArray[i];
					}
					else {
						bestNode->SW = nodeArray[i];
					}
				}
				nodeArray[i]->root = bestNode;
			}

			//handling the position from the root pov
			if(tmp == tmp->root->NE){
				tmp->root->NE = bestNode;
			}
			else if(tmp == tmp->root->SE){
				tmp->root->SE = bestNode;
			}
			else if(tmp == tmp->root->NW){
				tmp->root->NW = bestNode;
			}
			else if(tmp == tmp->root->SW){
				tmp->root->SW = bestNode;
			}
			else{ //unexepted error
				return false;
			}

			delete tmp;
			return true;
		}
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
	}

	bool changeKey(string old_key, string new_key){
		queue<NodePoint<T>*> coda; //queue used to parse all nodes in branch order
		coda.push(node);
		NodePoint<T>* tmp = new NodePoint<T>();
		while (tmp->aKey != old_key && coda.empty() != true) {
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
		if (tmp->aKey == old_key){
			tmp->aKey = new_key;
			return true;
		}
		else
			return false;
	}

	bool changePointKey(T x, T y, string new_key){
		Point<T> point_to_found(x, y);
		NodePoint<T>* tmp = node;
		while (tmp != NULL && !(tmp->point->isSamePoint(point_to_found))) { 
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
		else {
			tmp->aKey = new_key;
			return true;
		}
	}
};


template <class T>
struct NodePointList {
public:
	vector<Point<T>*> listPoint; //vector was used instead of list+iterator for simplicity
	Point<T>* SplitPoint = NULL;
	int dim = 10; //TODO fare dei test per vedere quale è la dimensione ottimale

	NodePointList<T>* root = NULL; 
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

	int getBestPointPosition(int dim, vector<Point<T>*> listPoint){
		float averageX = 0;
		float averageY = 0;
		for (int i = 0; i < dim; i++) {
			averageX += listPoint[i]->x;
			averageY += listPoint[i]->y;
		}

		averageX = averageX / dim;
		averageY = averageY / dim;
		float bestChoiceArray[10]; //indicates how close each node point is to the average of node points

		for (int i = 0; i < dim; i++)
			bestChoiceArray[i] = abs(listPoint[i]->x - averageX) + abs(listPoint[i]->y - averageY);
		
		float min = bestChoiceArray[0];
		int index = 0;
		for (int i = 1; i < dim; i++) { //the minimum value is sought because it is the closest to the average of the points
			if (min > bestChoiceArray[i]) {
				min = bestChoiceArray[i];
				index = i;
			}
		}

		return index;
	}

	int getBestChoicePositionFromVector(NodePointList<T>* fix){
		return getBestPointPosition(fix->dim, fix->listPoint);
	}

	tuple<vector<Point<T>*>, vector<Point<T>*>> getVectorOfQuadtreePoint(NodePointList<T>* fix){ //TOTEST
		vector<Point<T>*> setOfSplitPoints;
		vector<Point<T>*> vectorPoints;
		if(fix->NE->SplitPoint != NULL)
			setOfSplitPoints.push_back(fix->NE->SplitPoint);
		else
			vectorPoints.insert(vectorPoints.end(), fix->NE->listPoint.begin(), fix->NE->listPoint.end());

		if(fix->SE->SplitPoint != NULL)
			setOfSplitPoints.push_back(fix->SE->SplitPoint);
		else
			vectorPoints.insert(vectorPoints.end(), fix->SE->listPoint.begin(), fix->SE->listPoint.end());

		if(fix->NW->SplitPoint != NULL)
			setOfSplitPoints.push_back(fix->NW->SplitPoint);
		else
			vectorPoints.insert(vectorPoints.end(), fix->NW->listPoint.begin(), fix->NW->listPoint.end());

		if(fix->SW->SplitPoint != NULL)
			setOfSplitPoints.push_back(fix->SW->SplitPoint);
		else
			vectorPoints.insert(vectorPoints.end(), fix->SW->listPoint.begin(), fix->SW->listPoint.end());

		return make_tuple(setOfSplitPoints, vectorPoints);
	}

	void checkIntegrity(NodePointList<T>* fix){
		auto [setOfSplitPoints, vectorPoints] = getVectorOfQuadtreePoint(fix); //tuple<vector<Point<T>*>>
		vector<Point<T>*> allPoints;
		allPoints.insert(allPoints.end(), setOfSplitPoints.begin(), setOfSplitPoints.end());
		allPoints.insert(allPoints.end(), vectorPoints.begin(), vectorPoints.end());

		//TODO persistenza nel capire da dove provengono i punti, oppure ricreare?
		for(int i = 0; i < allPoints.size(); i++){
			if (allPoints[i].x >= fix->SplitPoint->x) { 
				if (allPoints[i].y >= fix->SplitPoint->y) {
					fix = fix->NE;
				}
				else {
					fix = fix->SE;
				}
			}
			else {
				if (allPoints[i].y >= fix->SplitPoint->y) {
					fix = fix->NW;
				}
				else {
					fix = fix->SW;
				}
			}
		}
		//da continuare?
	}	
	
	void Subdivide(NodePointList<T>* fix, Point<T>* to_ins) {
		int index = getBestChoicePositionFromVector(fix);
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

		fix->NE->root = fix;
		fix->NW->root = fix;
		fix->SE->root = fix;
		fix->SW->root = fix;
		fix->listPoint.clear();
	}

	void addPoint(NodePointList<T>* tmp, Point<T>* to_ins){
		if (tmp->listPoint.size() == tmp->dim) //if the node size reaches its maximum, a split point is created and the points are divided between new children
			Subdivide(tmp, to_ins);
		else
			tmp->listPoint.push_back(to_ins);
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
		addPoint(tmp, to_ins);
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
	
	bool cancelOnLeaf(T x, T y) {
		NodePointList<T>* tmp = node;
		Point<T> canceled(x, y);
		while (tmp->SplitPoint != NULL) { //the child position is searched based on x, y coordinates
			if (tmp->SplitPoint->isSamePoint(canceled)) //deleting a split point would cause inconsistency in the quadtree structure if not handled
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

	void handleSplitPointPositions(NodePointList<T>* fix, NodePointList<T>* bestNode){ //TOTEST
		if(fix == fix->root->NE){
			fix->root->NE = bestNode;
		}
		else if(fix == fix->root->SE){
			fix->root->SE = bestNode;
		}
		else if(fix == fix->root->NW){
			fix->root->NW = bestNode;
		}
		else if(fix == fix->root->SW){
			fix->root->SW = bestNode;
		}

		NodePointList<T>* nodeArray[4] = {{fix->NE}, {fix->SE}, {fix->NW}, {fix->SW}};

		for(int i = 0; i < 4; i++){
			if(nodeArray[i] == bestNode || nodeArray[i] == NULL)
				continue;

			if (bestNode->SplitPoint->x >= fix->SplitPoint->x) {
				if (bestNode->SplitPoint->y >= fix->SplitPoint->y) {
					bestNode->NE = nodeArray[i];
				}
				else {
					bestNode->SE = nodeArray[i];
				}
			}
			else {
				if (bestNode->SplitPoint->y >= fix->SplitPoint->y) {
					bestNode->NW = nodeArray[i];
				}
				else {
					bestNode->SW = nodeArray[i];
				}
			}
			nodeArray[i]->root = bestNode;
		}
	}

	void redistributeVectorPoints(NodePointList<T>* bestNode, vector<Point<T>*> vectorPoints){ //TOTEST
		//bug on clear, vector bestNode->NE->listPoint initialized with errors TOFIX
		bestNode->NE->listPoint.clear();
		bestNode->SE->listPoint.clear();
		bestNode->NW->listPoint.clear();
		bestNode->SW->listPoint.clear();
		for(int i = 0; i < (vectorPoints.size()-1); i++){
			if (bestNode->SplitPoint->x >= vectorPoints[i]->x) {
				if (bestNode->SplitPoint->y >= vectorPoints[i]->y) {
					addPoint(bestNode->NE, vectorPoints[i]);
				}
				else {
					addPoint(bestNode->SE, vectorPoints[i]);
				}
			}
			else {
				if (bestNode->SplitPoint->y >= vectorPoints[i]->y) {
					addPoint(bestNode->NW, vectorPoints[i]);
				}
				else {
					addPoint(bestNode->SW, vectorPoints[i]);
				}
			}
		}
	}

	void subdivideErase(NodePointList<T>* fix){ //TOTEST
		/*
		cerca lo split point tra i figli che è il migliore
		se manca lo split point allora vedere il migliore considerando anche tutti i figli
		per splint point che hanno sotto altri split point bisogna gestire ricorsivamente la subdivide()
		cioè si punta ad avere un caso in cui gli unici punti interessati sono punti non di splitpoint in cui si fa la subdivide()
		a quel punto di potrebbe sfruttare la semplice subdivide() gestendo il caso del punto == null

		caso 1
		tutti figli sono splitpoint -> si cerca tra quelli e si fa ricorsivamente la subdivide
		caso 2
		tutti i figli sono vector -> si cerca tra quelli, si toglie dalla lista e si mette come punto migliore così da non ribilanciare
		caso 3
		ci sono figli vector e splitpoint -> prendere il migliore degli split point e considerarlo insieme ai vector
		caso 3.1 figlio splitpoint allora si fa ricorsivamente la subdivide
		caso 3.2 figlio dal vector e si mette come punto migliore così da non ribilanciare
		*/

		auto [setOfSplitPoints, vectorPoints] = getVectorOfQuadtreePoint(fix); //tuple<vector<Point<T>*>>
		
		int indexSplitPoints;
		if(setOfSplitPoints.size() == 0){ //it means that there are only leafs
			int indexPoint = getBestPointPosition(vectorPoints.size(), vectorPoints);
			NodePointList<T>* new_node = new NodePointList<T>();
			new_node->SplitPoint = vectorPoints[indexPoint];
			
			redistributeVectorPoints(new_node, vectorPoints);
		}
		else if(setOfSplitPoints.size() >= 1 && setOfSplitPoints.size() <= 4){
			indexSplitPoints = getBestPointPosition(setOfSplitPoints.size(), setOfSplitPoints); //NE - SE - NW - SW
			NodePointList<T>* bestNode;
			bestNode->SplitPoint = setOfSplitPoints[indexSplitPoints];

			handleSplitPointPositions(fix, bestNode);
			redistributeVectorPoints(bestNode, vectorPoints);
		}
		else{
			cout << "error in cancel" << endl;
		}

	}

	bool cancel(T x, T y){
		NodePointList<T>* tmp = node;
		Point<T> canceled(x, y);
		bool samePoint = false;
		while (tmp->SplitPoint != NULL) { //the child position is searched based on x, y coordinates
			if (tmp->SplitPoint->isSamePoint(canceled)){
				samePoint = true;
				break;
			}
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
		if(samePoint == false)
			return false;
		else{
			subdivideErase(tmp);
			delete tmp;
			return true;
		}
	}
	//TODO si può chiamare la subdivideErase() che permette di cancellare split point e che viene richiamata ricorsivamente nel caso il nuovo split point fa da split point per un sotto quadtree
};
