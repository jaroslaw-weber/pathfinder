
#include <vector>
#include <iostream>
#include <memory>
#include <sstream>
#include <math.h>

using std::any_of;
using std::cout;
using std::for_each;
using std::make_shared;
using std::optional;
using std::shared_ptr;
using std::string;
using std::unique_ptr;
using std::vector;

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

//path node
//containing some information which is used in when finding path
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
	bool isSpecial;

	//get letter representing node
	//using for displaying path in console
	string ToLetter()
	{
		if (isSpecial)
		{
			return "p";
		}
		else if (canTraverse)
		{
			return " ";
		}

		return "X";
	}

	//A* total distance calculation
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

	vector<bool> grid;
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

	//find path (function signature as defined in task documentation)
	int FindPath(const int nStartX, const int nStartY,
				 const int nTargetX, const int nTargetY,
				 const unsigned char *pMap, const int nMapWidth, const int nMapHeight,
				 int *pOutBuffer, const int nOutBufferSize)
	{
		//todo
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
		for (int i = 0; i < mapSize; ++i) // ---- working
		{
			bool current = pMap[i] == 1;
			grid.push_back(current);
		}
		FindPath(start, end, m);
		return 0;
	}

	shared_ptr<Node> GetStartNode()
	{
		return GetNode(m_start).value();
	}

	bool IsClosed(shared_ptr<Node> node)
	{
		return any_of(m_closed.begin(), m_closed.end(), [&node](shared_ptr<Node> &n) {
			return n->position.IsEqual(node->position);
		});
	}

	//find path (simplified function signature)
	vector<Position> FindPath(Position start, Position end, Map map)
	{
		m_start = start;
		m_end = end;
		m_map = map;
		m_opened = vector<shared_ptr<Node>>();
		m_closed = vector<shared_ptr<Node>>();
		CreateNodes();

		shared_ptr<Node> current = GetStartNode();
		m_opened.push_back(current);

		current->distance = 0;
		current->isStart = true;
		current->euclideanDistance = GetEuclideanDistance(start, end);

		while (std::size(m_opened) > 0)
		{
			CalculateEuclideanPositions();
			SortOpenedByTotalDistance();

			auto current = m_opened.front();
			auto currentPos = current->position;

			bool isEndNode = currentPos.IsEqual(end);
			if (isEndNode)
			{
				//cout << "\nreached goal!";
				break;
			}

			vector<shared_ptr<Node>> neighbors = GetNeighbors(currentPos);

			//check neighbors
			for_each(neighbors.begin(), neighbors.end(), [this, &current, &currentPos](shared_ptr<Node> &neighbor) {
				this->AddToOpened(neighbor);

				//update path info if new distance is smaller
				int distance = current->distance + 1;
				//cout << "d:" << distance << "\n";
				if (distance < neighbor->distance)
				{
					neighbor->distance = distance;
					neighbor->previous = currentPos;
					//cout << "update: " << neighbor.ToString();
				}

				//cout << "neighbor: " << neighbor.ToString();
			});

			//remove visited node from opened
			RemoveFromOpenedNodes(current);
			//add checked node to "closed" so wont check twice
			m_closed.push_back(current);
		}

		auto path = GetPath(map, start, end);
		for_each(path.begin(), path.end(), [this](Position &p) {
			this->GetNode(p).value()->isSpecial = true;
		});
		return path;
	}

private:
	void AddToOpened(shared_ptr<Node> node)
	{
		if (IsClosed(node))
			return; // don't add if closed;

		m_opened.push_back(node);
	}
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

	void SortOpenedByTotalDistance()
	{
		std::sort(m_opened.begin(), m_opened.end(), [](shared_ptr<Node> a, shared_ptr<Node> b) -> bool {
			return a->GetTotalDistance() < b->GetTotalDistance();
		});
	}

	//get nodes next to the position,
	//which can be traversed
	vector<shared_ptr<Node>> GetNeighbors(Position position)
	{
		//cout << "\nget neighbor: " << position.ToString();
		vector<shared_ptr<Node>> arr;

		for (int i = position.x; i < position.x + 2; i++)
		{
			for (int j = position.y - 1; j < position.y + 2; j++)
			{

				Position p;
				p.x = i;
				p.y = j;

				if (p.IsEqual(position))
				{
					continue;
				}
				auto neighbor = GetNode(p);
				if (neighbor.has_value())
				{
					auto neighborNode = neighbor.value();
					//can it be traversed? if no then ignore
					if (neighborNode->canTraverse)
					{
						arr.push_back(neighborNode);
					}
				}
			}
		}

		return arr;
	}

	//get node at this position
	optional<shared_ptr<Node>> GetNode(Position position)
	{
		optional<shared_ptr<Node>> result;

		for_each(m_nodes.begin(), m_nodes.end(), [&position, &result](shared_ptr<Node> &n) {
			if (n->position.IsEqual(position))
			{
				//cout << "found";
				result = shared_ptr<Node>(n);
			}
		});
		return result;
	}

	void RemoveFromOpenedNodes(shared_ptr<Node> node)
	{

		m_opened.erase(std::remove_if(
						   m_opened.begin(), m_opened.end(),
						   [&node](shared_ptr<Node> &n) {
							   return n->position.IsEqual(node->position);
						   }),
					   m_opened.end());
	}

	float GetEuclideanDistance(Position a, Position b)
	{

		return sqrt(pow(b.x - a.x, 2) +
					pow(b.y - a.y, 2) * 1.0);
	}

	vector<Position> GetPath(Map &map, Position start, Position end)
	{
		auto path = vector<Position>();
		//cout << "\npath:";
		int x = 0;

		auto current = GetNode(end).value();
		while (x < 100) //todo
		{

			path.push_back(current->position);

			x++;
			//cout << "\n"<< current -> position.ToString();
			Position prevPos = current->previous;
			//cout << "\n prev pos:" << prevPos.ToString();

			if (current->position.IsEqual(start))
			{
				break;
			}
			current = GetNode(prevPos).value();
		}
		return path;
	}

	bool IsClosed(Node &node)
	{
		return true;
	}
};

class PathPrinter
{
public:
	vector<shared_ptr<Node>> nodes;

	void Print()
	{
		cout << "\n";
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
	}

	void PrintEachNodeInfo()
	{
		for_each(nodes.begin(), nodes.end(), [](shared_ptr<Node> &n) {
			cout << "\n";
			cout << n->ToString();
		});
	}
};

void exampleTest()
{
	unsigned char pMap[] = {1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1};
	int pOutBuffer[12];
	PathFinder pf;
	pf.FindPath(0, 0, 1, 2, pMap, 4, 3, pOutBuffer, 12);
};

void myTest()
{

	Map m;
	//const unsigned char cmd1[] = {0x01, 0x00};

	//const unsigned char* mc2 = cmd1;
	vector<bool> v;
	for (size_t i = 0; i < 65; i++)
	{
		v.push_back(true);
		v.push_back(true);
		v.push_back(false);
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
	p2.x = 22;
	p2.y = 7;

	auto path = pf.FindPath(p1, p2, m);
	PathPrinter pp;
	pp.nodes = pf.m_nodes;
	pp.Print();
};

int main()
{
	myTest();
	exampleTest();
	return 0;
};
