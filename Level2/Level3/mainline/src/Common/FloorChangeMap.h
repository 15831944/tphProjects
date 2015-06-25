#pragma once

class Path;
class Point;
class FloorChangeMap
{
public:
	double getNewFloor(double oldFloor)const;

	int getNewFloor(int iOldFloor)const;
	std::vector<int> nNewFloorIndex;

	
	void ChangePathFloor(Path& path )const;
	void ChangePointFloor(Point& pt)const;

};
