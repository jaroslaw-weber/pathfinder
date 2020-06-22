
#include <vector>
#include <iostream>
using std::vector;

using std::cout;

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
	int distance;
	float eclideanDistance;
	Position position;

	float totalDistance()
	{
		return distance + eclideanDistance;
	}

	bool IsEqual(Node &node)
	{
		return position.IsEqual(node.position);
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
            int x = i/width;
            int y = i%width;
            Node n = Node();
            Position p = Position();
            p.x = x;
            p.y = y;
            n.position = p;
            nodes.push_back(n);
		}

		//nodes.push_back(node);
	}
    
    void Print()
    {
        
    }

	bool IsTraversable(Position position)
	{
		return true;
	}

	vector<Node> GetNeighbors(Node &node)
	{
		return vector<Node>();
	}
};

class PathFinder
{
	unsigned char *map;

	vector<Position> opened;
	vector<Position> closed;

	int FindPath(const int nStartX, const int nStartY,
				 const int nTargetX, const int nTargetY,
				 const unsigned char* pMap, const int nMapWidth, const int nMapHeight,
				 int *pOutBuffer, const int nOutBufferSize)
	{
		//todo
		return -1;
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
    v.push_back(true);
    v.push_back(true);
    v.push_back(false);
    m.Constructor(v, 3,1);
	return 0;
}
