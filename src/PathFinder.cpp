
#include <vector>
#include <iostream>
#include <memory>
#include <sstream>

using std::cout;
using std::shared_ptr;
using std::string;
using std::unique_ptr;
using std::vector;

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
	bool isStart;

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

		std::stringstream s;
		s << "x:" << position.x << ",y:" << position.y << ",distance:" << distance;
		// assign to std::string
		std::string str = s.str();
		return str;
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
			cout << "\n";
			cout << n.ToString();
		});
	}

	vector<Node> GetNeighbors(Position position)
	{
		vector<Node> arr;

		std::for_each(nodes.begin(), nodes.end(), [&arr](Node &n) {
			//cout << n;
			arr.push_back(n);
		});

		return arr;
	}

	//todo: using uniqueptr here results in errors.
	unique_ptr<Node> GetAt(Position position)
	{
		unique_ptr<Node> result;

		std::for_each(nodes.begin(), nodes.end(), [&position, &result](Node &n) {
			if (n.position.IsEqual(position))
			{
				//cout << "found";
				result = unique_ptr<Node>(&n);
			}
		});
		return result;
	}
};

class PathFinder
{
public:
	unsigned char *map;

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
		map.Print();	  //show map
		map.PrintNodes(); // show starting valuess
		vector<Position> opened;
		opened.push_back(start);

		int maxLoops = 5; //for debugging, delete later;
		int loopNow = 0;

		unique_ptr<Node> current = map.GetAt(start);

		current->distance = 0;
		current->isStart = true;

		while (loopNow < maxLoops)
		{
			loopNow++;
			current = map.GetAt(start); //todo smallest

			Position currentPos = Position(); //current->position

			vector<Node> neighbors = map.GetNeighbors(currentPos);

			//check neighbors
			std::for_each(neighbors.begin(), neighbors.end(), [&opened, &current, &currentPos](Node &neighbor) {
				Position neighborPos = neighbor.position;
				opened.push_back(neighborPos);

				//update path info if new distance is smaller
				int distance = 999; //current->distance + 1;
				//cout << "d:" << distance << "\n";
				if (distance < neighbor.distance)
				{
					neighbor.distance = distance;
					neighbor.previous = currentPos;
					//cout << "update: " << neighbor.ToString();
				}

				//cout << "neighbor: " << neighbor.ToString();
			});

			map.PrintNodes();
			//remove visited node from opened
			opened.erase(std::remove_if(
							 opened.begin(), opened.end(),
							 [&currentPos](Position &p) {
								 return p.IsEqual(currentPos);
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
