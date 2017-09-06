bool pathExists(string maze[], int nRows, int nCols, int sr, int sc, int er, int ec)
{
	if (sr == er && sc == ec) return true; //yay

	maze[sr][sc] = '%'; //mark start location as visited

	if (maze[sr - 1][sc] == '.') //if North is unvisited, run pathexist from there
	{
		if (pathExists(maze, nRows, nCols, sr - 1, sc, er, ec)) return true;
	}

	if (maze[sr][sc + 1] == '.') //East
	{
		if (pathExists(maze, nRows, nCols, sr, sc + 1, er, ec)) return true;
	}

	if (maze[sr + 1][sc] == '.') //South
	{
		if (pathExists(maze, nRows, nCols, sr + 1, sc, er, ec)) return true;
	}

	if (maze[sr][sc - 1] == '.') //West
	{
		if (pathExists(maze, nRows, nCols, sr, sc - 1, er, ec)) return true;
	}

	return false;
}