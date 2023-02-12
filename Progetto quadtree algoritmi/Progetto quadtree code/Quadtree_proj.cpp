#include "quadtree.cpp"
using namespace std;

int main() {
	cout << "PointQuadtree" << endl;
	PointQuadtree<int>* pq = new PointQuadtree<int>(50, 50, "c");

	pq->insertPoint(30, 30, "r")->insertPoint(20, 30, "el")->insertPoint(10, 15, "k");
	pq->insertPoint(10, 20, "p")->insertPoint(15, 90, "op")->insertPoint(12, 55, "ek");
	pq->insertPoint(13, 30, "m")->insertPoint(0, 5, "rhe")->insertPoint(99, 12, "kel");
	pq->insertPoint(90, 43, "la")->insertPoint(9, 3, "jj")->insertPoint(14, 15, "kaj")->insertPoint(10, 2, "r");
	pq->insertPoint(3, 1, "p")->insertPoint(-5, -10, "ko")->insertPoint(-10, -9, "jr")->insertPoint(90, -1, "r");
	pq->insertPoint(302, -35, "ppppp")->insertPoint(3910, -1038, "jra")->insertPoint(193, -1093, "eh");

	bool keysearch = pq->keyPresence("op");
	if (keysearch == true)
		cout << "key = op found" << endl;
	else
		cout << "key = op not found" << endl;

	bool keysearch2 = pq->keyPresence("oplie");
	if (keysearch2 == true)
		cout << "key = oplie found" << endl;
	else
		cout << "key = oplie not found" << endl;

	bool canc = pq->cancel(30, 30);
	if (canc == true)
		cout << "(30, 30) found and deleted" << endl;
	else
		cout << "(30, 30) not found, or split point node" << endl;

	bool canc2 = pq->cancel(193, -1093);
	if (canc2 == true)
		cout << "(193, -1093) found and deleted" << endl;
	else
		cout << "(193, -1093) not found, or split point node" << endl;

	bool look = pq->search(10, 15);
	if (look == true)
		cout << "(10, 15) found" << endl;
	else
		cout << "(10, 15) not found" << endl;

	bool look2 = pq->search(11, 15);
	if (look2 == true)
		cout << "(11, 15) found" << endl;
	else
		cout << "(11, 15) not found" << endl;


	cout << "\nPointQuadtreeList" << endl;
	PointQuadtreeList<float>* pql = new PointQuadtreeList<float>(50, 50);
	pql->insertPoint(10, 10)->insertPoint(30, 13)->insertPoint(30, 10)->insertPoint(5, 0);
	pql->insertPoint(20, 6)->insertPoint(6, 30)->insertPoint(12, 19)->insertPoint(0, 8);
	pql->insertPoint(-4, 7)->insertPoint(-10, -20)->insertPoint(56, -10)->insertPoint(-13, -20);
	pql->insertPoint(-1, -2)->insertPoint(-19, -18)->insertPoint(-20, -1)->insertPoint(-111, 0);

	bool cancpql = pql->cancel(30, 10);
	if (cancpql == true)
		cout << "(30, 10) found and deleted" << endl;
	else
		cout << "(30, 10) not found, or split point node" << endl;

	bool cancpql2 = pql->cancel(350, 10); 
	if (cancpql2 == true)
		cout << "(350, 10) found" << endl;
	else
		cout << "(350, 10) not found, or split point node" << endl;

	bool cancpql3 = pql->cancel(12, 19); //Split Point
	if (cancpql3 == true)
		cout << "(12, 19) found" << endl;
	else
		cout << "(12, 19) not found, or split point node" << endl;

	bool lookpql = pql->search(56, -10);
	if (lookpql == true)
		cout << "(56, -10) found" << endl;
	else
		cout << "(56, -10) not found" << endl;

	bool lookpql2 = pql->search(560, -10);
	if (lookpql2 == true)
		cout << "(560, -10) found" << endl;
	else
		cout << "(560, -10) not found" << endl;

	bool lookpql3 = pql->search(12, 19); //Split Point
	if (lookpql3 == true)
		cout << "(12, 19) found" << endl;
	else
		cout << "(12, 19) not found" << endl;
}
