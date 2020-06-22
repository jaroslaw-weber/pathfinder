
#include <vector>
#include <iostream>
#include <memory>
#include <format>

using std::vector;
using std::string;
using std::cout;
using std::shared_ptr;

struct Position
{
public:
	int x;
	int y;

	bool IsEqual(Position &otherPosition)
	{
		return otherPosition.x == x && otherPosition.y == y;
	}
};

class Node
{
public:
	//std::shared_ptr<Node> previousNode;
	int distance = 9999999;
	float eclideanDistance;
	Position position;
	bool canTraverse;
	Position previous;

	float totalDistance()
	{
		return distance + eclideanDistance;
	}

	bool IsEqual(Node &node)
	{
		return position.IsEqual(node.position);
	}
    
    string ToString()
    {
        return std::format("x:{},y:{}", position.x,position.y);
    }
};

class Map
{
public:
	vector<Node> nodes;

	int width;
	int height;
	vector<bool> grid;

	void Constructor(vector<bool> _grid, int _width, int _height)
	{
		width = _width;
		height = _height;
		grid = _grid;

		nodes = vector<Node>();
		int gridSize = std::size(_grid);
		for (size_t i = 0; i < gridSize; i++)
		{
			//cout << "i"<<i;
			/* code */
			int y = i / width;
			int x = i % width;
			Node n = Node();
			Position p = Position();
			p.x = x;
			p.y = y;
			n.position = p;
			n.canTraverse = _grid.at(i);
			nodes.push_back(n);
		}

		//nodes.push_back(node);
	}

	void Print()
	{
		int count = std::size(nodes);
		int currentY = 0;
		for (size_t i = 0; i < count; i++)
		{
			Node node = nodes.at(i);
			int x = node.position.x;
			int y = node.position.y;
			bool value = node.canTraverse;
			if (currentY != y)
			{
				cout << "\n";
				currentY = y;
			}

			cout << value;
		}
	}

	void PrintNodes()
	{
		std::for_each(nodes.begin(), nodes.end(), [](Node &n) {
			cout << n.ToString();
		});
	}

	vector<Node> GetNeighbors(Position position)
	{
		vector<Node> arr;

		std::for_each(nodes.begin(), nodes.end(), [](Node &n) {
			//cout << n;
			arr.push_back(n);
		});

		return arr;
	}

	Node GetAt(Position position)
	{
		std::for_each(nodes.begin(), nodes.end(), [](Node &n) {
			if (n.position == position)
			{
				return n;
			}
		});
        Node n;
        return n;
	}
};

class PathFinder
{
public:
	unsigned char *map;

	vector<Position> opened;
	vector<Position> closed;

	int FindPath(const int nStartX, const int nStartY,
				 const int nTargetX, const int nTargetY,
				 const unsigned char *pMap, const int nMapWidth, const int nMapHeight,
				 int *pOutBuffer, const int nOutBufferSize)
	{
		//todo
		return -1;
	}

	void FindPath(Position start, Position end, Map map)
	{
		opened.push_back(current.position);

		int maxLoops = 5; //for debugging, delete later;
		int loopNow = 0;

		while (loopNow < maxLoops)
		{
			loopNow++;
			Node current = map.GetAt(start); //todo smallest

			vector<Node> neighbors = map.GetNeighbors(node.position);

			//check neighbors
			std::for_each(neighbors.begin(), neighbors.end(), [](Node &neighbor) {
				Position neighborPos = neighbor.position;
				opened.push_back(neighborPos);

				//update path info if new distance is smaller
				int distance = current.distance + 1;
				if (distance < neighbor.distance)
				{
					neighbor.distance = distance;
					neighbor.previous = current.position;
				}
			});

			//remove visited node from opened
			opened.erase(std::remove_if(
							 opened.begin(), opened.end(),
							 [](const Position &pos) {
								 return pos.IsEqual(current.position);
							 }),
						 opened.end());
		}
	}

	bool IsClosed(Node &node)
	{
		return true;
	}
};

int main()
{
	Map m;
	//const unsigned char cmd1[] = {0x01, 0x00};

	//const unsigned char* mc2 = cmd1;
	vector<bool> v;
	for (size_t i = 0; i < 5; i++)
	{
		v.push_back(true);
		v.push_back(true);
		v.push_back(true);
		v.push_back(true);
	}

	m.Constructor(v, 5, 4);
	PathFinder pf;
	Position p1;
	Position p2;
	p2.x = 3;
	p2.y = 3;

	pf.FindPath(p1, p2, m);
	return 0;
}
