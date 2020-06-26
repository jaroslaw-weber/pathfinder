#include <vector>
#include <iostream>
#include <memory>
#include <sstream>
#include <math.h>

#include <cassert>

using std::any_of;
using std::cout;
using std::for_each;
using std::make_shared;
using std::optional;
using std::shared_ptr;
using std::string;
using std::unique_ptr;
using std::vector;

/*

about this algorithm:

I am using A* algorithm for finding path.
I have used it because it is simple enough to reason about it and have good performance.
First I convert input to simpler data structures such as Position, Map, Node.
I do it to make it more readable.
Then I use this data to calculate path.
Before I calculate path, I print map in console, as 2d grid of symbols.
It is for the purpose of debugging. I recomment checking out comments on PathPrinter.
After path is calculated, I print map with PathPrinter again, this time showing path.

There are still some things to improve but algorithm seems to work correctly.
With some more time I would add more test cases and more automated testing.

*/

//printing int array to console
void PrintIntArray(int *arr, int arrSize)
{
	for (int i = 0; i < arrSize; i++)
	{
		cout << arr[i] << ",";
	}
}

//point on a 2d grid
struct Position
{
public:
	int x;
	int y;

	//are two points equal
	bool IsEqual(Position &otherPosition)
	{
		return otherPosition.x == x && otherPosition.y == y;
	}

	string ToString()
	{
		std::stringstream s;
		s << "x:" << x << ", y:" << y;
		// assign to std::string
		return s.str();
	}
};

//converting positions to indexes in a array
//use this for representing path as int array (as in api specification)
vector<int> PositionsToIntVector(vector<Position> positions, int width)
{
	auto result = vector<int>();
	for_each(positions.begin(), positions.end(), [&result, &width](Position &p) {
		int index = p.y * width + p.x;
		result.push_back(index);
	});
	return result;
}

//path node
//containing some information about path
class Node
{
public:
	//distance from start node.
	//huge number at default means that distance is unknown.
	//todo: change to "optional"
	int distance = 9999999;

	//euclidean distance from end node.
	//used for getting better direction when finding path
	float euclideanDistance = -1;

	//position on grid
	Position position;
	//is it blocking node? if so, need to go around and cannot pass
	bool canTraverse;
	//previous node
	Position previous;
	//is it a start node
	bool isStart;
	//is it part of the shortest path?
	//after path calculated, set this to "true"
	bool isPartOfThePath;

	//get letter representing node
	//using for displaying path in console
	string ToLetter()
	{
		if (isPartOfThePath)
		{
			return "p";
		}
		else if (canTraverse)
		{
			return ".";
		}

		return "X";
	}

	//A* total distance calculation
	//the smaller it is the better choice it is for next step
	float GetTotalDistance()
	{
		return distance + euclideanDistance;
	}

	bool IsEqual(Node &node)
	{
		return position.IsEqual(node.position);
	}

	//using when debugging
	string ToString()
	{

		std::stringstream s;
		s << position.ToString() << ",distance:" << distance << ", euclidean: " << euclideanDistance << ",totalDistance:" << GetTotalDistance();
		// assign to std::string
		std::string str = s.str();
		return str;
	}
};

//simplified representation of map which will be used when searching for path
class Map
{
public:
	int width;
	int height;

	//map as one dimensional grid
	//true means that the position is traverable
	//the grid is given in row-major order,
	//each row is given in order of increasing 𝑥-coordinate,
	//and the rows are given in order of increasing 𝑦-coordinate
	vector<bool> grid;
};

//printing path
class PathPrinter
{
public:
	vector<shared_ptr<Node>> nodes;
	/*
	printing nodes.

	it will look like this:
	__________
	....
	X.X.
	X...
	__________

	it can also print path like this:

	__________
	p...
	XpX.
	Xp..
	__________

	symbols:
	X -> cannot traverse
	. -> can traverse 
	p -> path
	*/
	void Print()
	{
		cout << "\n__________\n";
		int count = std::size(nodes);
		int currentY = 0;
		for (size_t i = 0; i < count; i++)
		{
			shared_ptr<Node> node = nodes.at(i);
			int y = node->position.y;
			if (currentY != y)
			{
				cout << "\n";
				currentY = y;
			}
			cout << node->ToLetter();
		}
		cout << "\n__________\n";
	}

	void PrintEachNodeInfo()
	{
		for_each(nodes.begin(), nodes.end(), [](shared_ptr<Node> &n) {
			cout << "\n";
			cout << n->ToString();
		});
	}
};

//finding path using A* algorithm
class PathFinder
{
public:
	//cached info about nodes and distances
	vector<shared_ptr<Node>> m_nodes;

	Map m_map;
	Position m_start;
	Position m_end;
	vector<shared_ptr<Node>> m_opened;
	vector<shared_ptr<Node>> m_closed;

	//cached map info
	//unsigned char *map;

	//convert Map to vector of Nodes
	void CreateNodes()
	{
		m_nodes = vector<shared_ptr<Node>>();
		int gridSize = std::size(m_map.grid);
		for (size_t i = 0; i < gridSize; i++)
		{
			int y = i / m_map.width;
			int x = i % m_map.width;
			auto n = make_shared<Node>();
			Position p = Position();
			p.x = x;
			p.y = y;
			n->position = p;
			n->canTraverse = m_map.grid.at(i);
			m_nodes.push_back(n);
		}
	}

	//find path (function signature as defined in api documentation)
	int FindPath(const int nStartX, const int nStartY,
				 const int nTargetX, const int nTargetY,
				 const unsigned char *pMap, const int nMapWidth, const int nMapHeight,
				 int *pOutBuffer, const int nOutBufferSize)
	{
		Position start;
		start.x = nStartX;
		start.y = nStartY;
		Position end;
		end.x = nTargetX;
		end.y = nTargetY;
		Map m;
		m.width = nMapWidth;
		m.height = nMapHeight;
		auto grid = vector<bool>();
		int mapSize = m.width * m.height;
		for (int i = 0; i < mapSize; ++i)
		{
			bool current = pMap[i] == 1;
			grid.push_back(current);
		}
		m.grid = grid;

		optional<vector<shared_ptr<Node>>> pathOptional = FindPath(start, end, m);

		//if path is found
		if (pathOptional.hasValue())
		{
			auto path = pathOptional.value();
			//how many steps
			int pathSize = std::size(path);
			int stepCount = pathSize - 1;

			//if there is path available
			if (stepCount > 0)
			{
				//convert path from 2d coordinates to 1d array
				auto intPath = PositionsToIntVector(path, m.width);
				intPath.erase(intPath.begin());

				for (int i = 0; i < stepCount; i++)
				{
					pOutBuffer[i] = intPath.at(i);
				}
				//printing for debug
				cout << "\npath as int array:\n";
				PrintIntArray(pOutBuffer, nOutBufferSize);
			}
			return stepCount;
		}
		//if path is not found
		else
		{
			return -1;
		}
	}

	//get first node
	shared_ptr<Node> GetStartNode()
	{
		return GetNode(m_start).value();
	}

	//check if already visited the node
	bool IsClosed(shared_ptr<Node> node)
	{
		return any_of(m_closed.begin(), m_closed.end(), [&node](shared_ptr<Node> &n) {
			return n->position.IsEqual(node->position);
		});
	}

	//find path (simplified function signature)
	optional<vector<Position>> FindPath(Position start, Position end, Map map)
	{
		//initialize
		bool success = false;
		m_start = start;
		m_end = end;
		m_map = map;
		m_opened = vector<shared_ptr<Node>>();
		m_closed = vector<shared_ptr<Node>>();
		CreateNodes();

		//print map
		cout << "\nmap:";
		PathPrinter pp;
		pp.nodes = m_nodes;
		pp.Print();
		//

		shared_ptr<Node> current = GetStartNode();
		m_opened.push_back(current);

		current->distance = 0;
		current->isStart = true;
		current->euclideanDistance = GetEuclideanDistance(start, end);

		while (std::size(m_opened) > 0)
		{
			//update some data after each loop
			CalculateEuclideanPositions();
			SortOpenedByTotalDistance();

			//get node with smallest "total distance"
			auto current = m_opened.front();
			auto currentPos = current->position;

			//check if reached the goal
			bool isEndNode = currentPos.IsEqual(end);
			if (isEndNode)
			{
				success = true;
				break;
			}

			vector<shared_ptr<Node>> neighbors = GetNeighbors(currentPos);

			//check neighbors
			for_each(neighbors.begin(), neighbors.end(), [this, &current, &currentPos](shared_ptr<Node> &neighbor) {
				this->AddToOpened(neighbor);

				//update path info if new distance is smaller
				int distance = current->distance + 1;
				if (distance < neighbor->distance)
				{
					neighbor->distance = distance;
					neighbor->previous = currentPos;
				}
			});

			//remove visited node from opened
			RemoveFromOpenedNodes(current);
			//add checked node to "closed" so wont check twice
			m_closed.push_back(current);
		}

		//if found path
		if (success)
		{
			//get path as vector of positions
			auto path = GetPath(map, start, end);
			//prepare for printing
			for_each(path.begin(), path.end(), [this](Position &p) {
				this->GetNode(p).value()->isPartOfPath = true;
			});
			//print path
			cout << "\npath:";
			pp.Print();
			//return result
			return path;
		}
		else
		{

			cout << "path not found!";
			return optional<vector<shared_ptr<Node>>>(); //if failed return empty optional
		}
	}

private:
	//add to queue of nodes to check next
	void AddToOpened(shared_ptr<Node> node)
	{
		if (IsClosed(node))
			return; // don't add if closed;

		m_opened.push_back(node);
	}

	//calculate how far from target (end) are nodes
	void CalculateEuclideanPositions()
	{
		for_each(m_opened.begin(), m_opened.end(), [this](auto n) {
			// if euclidean distance is not calculated
			if (n->euclideanDistance < 0)
			{
				// then calculate and cache
				n->euclideanDistance = this->GetEuclideanDistance(n->position, this->m_end);
			}
		});
	}

	//sort for the purpose of making decision which node check next
	void SortOpenedByTotalDistance()
	{
		std::sort(m_opened.begin(), m_opened.end(), [](shared_ptr<Node> a, shared_ptr<Node> b) -> bool {
			return a->GetTotalDistance() < b->GetTotalDistance();
		});
	}

	//get nodes next to the position,
	//which can be traversed
	//reminder: cannot traverse diagnostically, only horizontal or vertical
	vector<shared_ptr<Node>> GetNeighbors(Position position)
	{
		//cout << "\nget neighbor: " << position.ToString();
		vector<shared_ptr<Node>> arr;

		Position p1; //left
		Position p2; //right
		Position p3; //up
		Position p4; //down
		p1.x = position.x - 1;
		p2.x = position.x + 1;
		p1.y = position.y;
		p2.y = position.y;
		p3.x = position.x;
		p4.x = position.x;
		p3.y = position.y - 1;
		p4.y = position.y + 1;
		auto positions = vector<Position>();
		positions.push_back(p1);
		positions.push_back(p2);
		positions.push_back(p3);
		positions.push_back(p4);

		//check if those positions are available
		//if so, add them to result array
		for_each(positions.begin(), positions.end(), [this, &arr](Position &p) {
			auto neighbor = this->GetNode(p);
			//if out of range then discard
			if (neighbor.has_value())
			{
				auto neighborNode = neighbor.value();
				//can it be traversed? if no then ignore
				if (neighborNode->canTraverse)
				{
					arr.push_back(neighborNode);
				}
			}
		});

		return arr;
	}

	//get node at this position
	optional<shared_ptr<Node>> GetNode(Position position)
	{
		optional<shared_ptr<Node>> result;

		for_each(m_nodes.begin(), m_nodes.end(), [&position, &result](shared_ptr<Node> &n) {
			if (n->position.IsEqual(position))
			{
				result = shared_ptr<Node>(n);
			}
		});
		return result;
	}

	//remove from checking queue
	void RemoveFromOpenedNodes(shared_ptr<Node> node)
	{
		m_opened.erase(std::remove_if(
						   m_opened.begin(), m_opened.end(),
						   [&node](shared_ptr<Node> &n) {
							   return n->position.IsEqual(node->position);
						   }),
					   m_opened.end());
	}

	//get euclidean distance between 2 points on 2d grid
	float GetEuclideanDistance(Position a, Position b)
	{

		return sqrt(pow(b.x - a.x, 2) +
					pow(b.y - a.y, 2) * 1.0);
	}

	//after calculating all distances and finding a path,
	//create a vector of steps of that path
	vector<Position> GetPath(Map &map, Position start, Position end)
	{
		auto path = vector<Position>();
		int x = 0;

		//start from end node and get steps in reverse
		auto current = GetNode(end).value();
		while (true)
		{

			path.push_back(current->position);

			x++;
			//get previous step
			Position prevPos = current->previous;

			//if reached start position then leave the loop
			if (current->position.IsEqual(start))
			{
				break;
			}
			current = GetNode(prevPos).value();
		}
		//because steps are in reverse we need to reverse the vector
		std::reverse(path.begin(), path.end());
		return path;
	}
};

//test if algorithm is working properly
//using data provided in documentation
void exampleTest()
{
	cout << "\n===example1===\n";
	unsigned char pMap[] = {1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1};
	int pOutBuffer[12];
	PathFinder pf;
	int steps = pf.FindPath(0, 0, 1, 2, pMap, 4, 3, pOutBuffer, 12);
	//testing if result is correct
	assert(steps == 3);
	assert(pOutBuffer[0] == 1);
	assert(pOutBuffer[1] == 5);
	assert(pOutBuffer[2] == 9);
};

//test if algorithm is working properly
//using data provided in documentation
void exampleTest2()
{
	cout << "\n===example2===\n";
	unsigned char pMap[] = {0, 0, 1, 0, 1, 1, 1, 0, 1};
	PathFinder pf;
	int pOutBuffer[7];
	int steps = pf.FindPath(2, 0, 0, 2, pMap, 3, 3, pOutBuffer, 7);
	//testing if result is correct
	assert(steps == -1);
};

//test if algorithm is working properly
//using custom data (big grid)
void myTest()
{
	cout << "=== custom test ===";
	Map m;
	vector<bool> v;
	for (size_t i = 0; i < 65; i++)
	{
		v.push_back(true);
		v.push_back(true);
		v.push_back(true);
		v.push_back(true);
		v.push_back(false);
		v.push_back(true);
	}

	m.grid = v;
	m.width = 39;
	m.height = 10;
	PathFinder pf;
	Position p1;
	Position p2;
	p2.x = 28;
	p2.y = 3;

	auto path = pf.FindPath(p1, p2, m);
};

//run all tests
void runTests()
{
	myTest();
	exampleTest();
	exampleTest2();
}

int main()
{
	runTests();
	return 0;
};
