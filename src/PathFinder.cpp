
#include <vector>
#include <iostream>
#include <memory>
#include <sstream>

using std::cout;
using std::for_each;
using std::make_shared;
using std::optional;
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
    
    string ToString()
    {
		std::stringstream s;
		s << "x:" << x << ", y:" << y;
		// assign to std::string
		std::string str = s.str();
		return str;
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
		s << position.ToString() << ",distance:" << distance;
		// assign to std::string
		std::string str = s.str();
		return str;
	}
};

class Map
{
public:
	vector<shared_ptr<Node>> nodes;

	int width;
	int height;
	vector<bool> grid;

	void Constructor(vector<bool> _grid, int _width, int _height)
	{
		width = _width;
		height = _height;
		grid = _grid;

		nodes = vector<shared_ptr<Node>>();
		int gridSize = std::size(_grid);
		for (size_t i = 0; i < gridSize; i++)
		{
			//cout << "i"<<i;
			/* code */
			int y = i / width;
			int x = i % width;
			auto n = make_shared<Node>();
			Position p = Position();
			p.x = x;
			p.y = y;
			n->position = p;
			n->canTraverse = _grid.at(i);
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
			shared_ptr<Node> node = nodes.at(i);
			int x = node->position.x;
			int y = node->position.y;
			bool value = node->canTraverse;
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
		std::for_each(nodes.begin(), nodes.end(), [](shared_ptr<Node> &n) {
			cout << "\n";
			cout << n->ToString();
		});
	}

	vector<shared_ptr<Node>> GetNeighbors(Position position)
	{
        cout << "\nget neighbor: " << position.ToString();
		vector<shared_ptr<Node>> arr;

		for (int i = position.x; i < position.x + 2; i++)
		{
			for (int j = position.y - 1; j < position.y + 2; j++)
			{
                
				Position p;
				p.x = i;
				p.y = j;
                
                if(p.IsEqual(position))
                {
                    continue;
                }
				auto neighbor = GetAt(p);
				if (neighbor.has_value())
				{
					arr.push_back(neighbor.value());
				}
			}
		}

		return arr;
	}

	optional<shared_ptr<Node>> GetAt(Position position)
	{
		optional<shared_ptr<Node>> result;

		for_each(nodes.begin(), nodes.end(), [&position, &result](shared_ptr<Node> &n) {
			if (n->position.IsEqual(position))
			{
				//cout << "found";
				result = shared_ptr<Node>(n);
			}
		});
		return result;
	}
};

class PathFinder
{
public:
	unsigned char *map;


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
		map.Print(); //show map
        
		vector<Position> opened;
		vector<Position> closed;
		opened.push_back(start);

		int maxLoops = 5; //for debugging, delete later;
		int loopNow = 0;

		shared_ptr<Node> current = map.GetAt(start).value();
		current->distance = 0;
		current->isStart = true;

		while (loopNow < maxLoops)
		{
			loopNow++;
            auto currentPos = opened.back();
			current = map.GetAt(currentPos).value();


			vector<shared_ptr<Node>> neighbors = map.GetNeighbors(currentPos);

			//check neighbors
			for_each(neighbors.begin(), neighbors.end(), [&opened, &current, &currentPos](shared_ptr<Node> &neighbor) {
				Position neighborPos = neighbor->position;
                if(!Contains(closed, neighborPos))
                {
				opened.push_back(neighborPos);
                }

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

			map.PrintNodes();
			//remove visited node from opened
            
			opened.erase(std::remove_if(
							 opened.begin(), opened.end(),
							 [&currentPos](Position &p) {
								 return p.IsEqual(currentPos);
							 }),
						 opened.end());
            //add checked node to "closed" so wont check twice
            closed.push_back(currentPos);
		}
	}
    
    bool Contains(vector<Position> &arr, Position &pos)
    {
        return std::is_any(arr.begin(), arr.end(), [&pos](Position p){
            return p.IsEqual(pos);
        })
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
