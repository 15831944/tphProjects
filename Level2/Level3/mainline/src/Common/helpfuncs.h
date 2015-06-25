#pragma once

#include <common/ARCVector.h>
class HelpFuncs
{
public:
	static double getInterpolateXPos(double dclamf, const std::vector<double>& vposList);

	static bool getRelatePos(double dPos, const std::vector<double>& vposList, double& outret);


	static std::vector<double> GetPointXWithYVaule(double dy, const std::vector<ARCVector2>& thepath);

};


class GetPointXWithYValue
{
public:	
	void Caculate(double dy, const std::vector<ARCVector2>& thepath);
	std::vector<double> vXValues;
	std::vector<int> vPtsIdx; 

};
