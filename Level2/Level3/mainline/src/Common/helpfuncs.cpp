#include "StdAfx.h"
#include "helpfuncs.h"
#include <algorithm>

double HelpFuncs::getInterpolateXPos( double dclamf, const std::vector<double>& vposList )
{
	int nSize = vposList.size();
	if(nSize == 0)
		return 0;


	double dTotal = dclamf * (nSize-1); 
	int nidx = (int)dTotal;
	double dext = dTotal - nidx;

	if(nidx>=0 && nidx<nSize-1)
	{
		return vposList[nidx] * (1-dext) + vposList[nidx+1] * dext;	
	}

	if(nidx <0 )
		return *vposList.begin();

	if(nidx>= nSize-1)
		return *vposList.rbegin(); 
	return 0;
}

bool HelpFuncs::getRelatePos( double dPos, const std::vector<double>& vposList, double& outret )
{
	if(vposList.size())
	{
		if(dPos < *vposList.begin())
			return false;
		if(dPos > *vposList.rbegin() )
			return false;

		if(dPos == *vposList.begin() )
		{
			outret = 0;
			return true;
		}

		
		for(int i=0;i<(int)vposList.size()-1;i++)
		{
			double curPos = vposList[i];
			double nextPos = vposList[i+1];

			if(dPos==curPos)
			{
				outret = i;
				return true;
			}
			if(dPos == nextPos)
			{
				outret = i+1;
				return true;
			}

			if(dPos > curPos && dPos < nextPos )
			{				
				double dExt = (dPos - curPos)/(nextPos - curPos);				
				outret =  dExt + i;
				return true;
			}	

		}
	}	
	return false;
}

std::vector<double> HelpFuncs::GetPointXWithYVaule( double dy, const std::vector<ARCVector2>& thepath )
{
	std::vector<double> dRet;
	for(int i=0;i<(int)thepath.size();i++)
	{
		const ARCVector2& p1 = thepath[i];
		const ARCVector2& p2 = thepath[(i+1)%thepath.size()];

		if(p1[VY] == dy)
			dRet.push_back(p1[VX]);
		if( p2[VY] == dy)
			dRet.push_back(p2[VX]);

		if( dy < max(p1[VY],p2[VY]) && dy > min(p1[VY],p2[VY]) )
		{
			double drat = (dy - p1[VY])/(p2[VY]  - p1[VY]);
			double dx = p1[VX] * (1-drat) + p2[VX]*drat;
			dRet.push_back(dx);
		}
	}
	std::sort(dRet.begin(),dRet.end());
	return dRet;
}

void GetPointXWithYValue::Caculate( double dy, const std::vector<ARCVector2>& thepath )
{
	vXValues.clear();
	vPtsIdx.clear();		
	for(int i=0;i<(int)thepath.size();i++)
	{
		const ARCVector2& p1 = thepath[i];
		const ARCVector2& p2 = thepath[(i+1)%thepath.size()];

		if(p1[VY] == dy){
			vPtsIdx.push_back(i);
			vXValues.push_back(p1[VX]);
		}
		if( p2[VY] == dy)
		{
			vPtsIdx.push_back(i+1);
			vXValues.push_back(p2[VX]);
		}

		if( dy < max(p1[VY],p2[VY]) && dy > min(p1[VY],p2[VY]) )
		{
			double drat = (dy - p1[VY])/(p2[VY]  - p1[VY]);
			double dx = p1[VX] * (1-drat) + p2[VX]*drat;
			vXValues.push_back(dx);
			vPtsIdx.push_back(i);
		}
	}
	//std::sort(vXValues.begin(),vXValues.end());	
	//std::sort(vPtsIdx.begin(), vPtsIdx.end());
}