#include <shared_ptr>

class Position
{
public:
	int x;
	int y;

	bool IsEqual(Position otherPosition)
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

	bool IsEqual(Node node)
	{
		return position.IsEqual(node.position);
	}
};

class Map
{
public:
	//std::vector<Node> nodes;
	unsigned char *grid;

	int width;
	int height;

	bool IsTraversable(Position position)
	{
		return true;
	}
	/*
	std::vector<Node> GetNeighbors(*Node node)
	{
	}
	*/
};

class PathFinder
{
	unsigned char *map;

	Node opened;
	Node closed;

	int FindPath(const int nStartX, const int nStartY,
				 const int nTargetX, const int nTargetY,
				 const unsigned char *pMap, const int nMapWidth, const int nMapHeight,
				 int *pOutBuffer, const int nOutBufferSize)
	{
		//todo
		return -1;
	}

	bool IsClosed(std::shared_ptr<Node> node)
	{
	}
};