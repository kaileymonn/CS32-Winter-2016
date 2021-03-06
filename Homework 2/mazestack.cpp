#include <iostream>
#include <stack>
#include <cassert>
using namespace std;

class Coord
{
public:
	Coord(int rr, int cc) : m_r(rr), m_c(cc) {}
	int r() const { return m_r; }
	int c() const { return m_c; }
private:
	int m_r;
	int m_c;
};

bool pathExists(string maze[], int nRows, int nCols, int sr, int sc, int er, int ec)
//Return true if there is a path from (sr, sc) to (er, ec) through the maze, else return false
{
	stack<Coord> coordStack;
	Coord start(sr, sc);
	coordStack.push(start); //push start coord
	maze[sr][sc] = '#'; //update maze

	while (!coordStack.empty())
	{
		Coord temp = coordStack.top();
		coordStack.pop(); //pop off top

		int r = temp.r();
		int c = temp.c();
		cerr << "we popped " << r << " " << c << endl;

		if (r == er && c == ec) return true; //check if maze is solved

		if (maze[r - 1][c] == '.') //north?
		{
			coordStack.push(Coord(r - 1, c));
			maze[r - 1][c] = '#';
		}

		if (maze[r][c + 1] == '.') //east?
		{
			coordStack.push(Coord(r, c + 1));
			maze[r][c + 1] = '#';
		}

		if (maze[r + 1][c] == '.') //south?
		{
			coordStack.push(Coord(r + 1, c));
			maze[r + 1][c] = '#';
		}

		if (maze[r][c - 1] == '.') //west?
		{
			coordStack.push(Coord(r, c - 1));
			maze[r][c - 1] = '#';
		}
	}
	return false;	//no solution:(
}

int main()
{
	string maze[10] = {
		"XXXXXXXXXX",
		"X........X",
		"XX.X.XXXXX",
		"X..X.X...X",
		"X..X...X.X",
		"XXXX.XXX.X",
		"X.X....XXX",
		"X..XX.XX.X",
		"X...X....X",
		"XXXXXXXXXX"
	};

	if (pathExists(maze, 10, 10, 6, 4, 1, 1))
		cout << "Solvable!" << endl;
	else
		cout << "Out of luck!" << endl;
}