#pragma once

#include "../InputAirside/Runway.h"
#include <cmath>

const static DistanceUnit DMaxHeight = 1000*SCALE_FACTOR;
const static DistanceUnit DMaxLength = 1000* SCALE_FACTOR;

struct OffsetHightMap
{
	std::vector<ARCVector2> m_vOffsetHigth;
	void Add(DistanceUnit offset, DistanceUnit dheight){ m_vOffsetHigth.push_back(ARCVector2(offset,dheight)); }

	void clear(){ m_vOffsetHigth.clear(); }
	ARCVector2& Get(int idx){ return m_vOffsetHigth[idx]; }

	int GetCount()const{ return (int)m_vOffsetHigth.size(); }

	//get height offset
	bool getHightOffset(const DistanceUnit& height,DistanceUnit& doffset )const
	{
		for(int i=0;i< (int) m_vOffsetHigth.size()-1; ++i)
		{
			const ARCVector2& v1 = m_vOffsetHigth[i];
			const ARCVector2& v2 = m_vOffsetHigth[i+1];
			if(v1.y == height)
			{
				doffset = v1.x;
				return true;
			}
			if(v2.y == height )
			{
				doffset = v2.x;
				return true;
			}			
			if( v1.y < height && height < v2.y )
			{				
				double dRat = (height - v1.y)/(v2.y - v1.y);
				doffset = (1- dRat)*v1.x + dRat * v2.x;
				return true;
			}
		}
		return false;
	}

	DistanceUnit getOffsetHeight(const DistanceUnit& offset)const
	{		
		for(int i=0;i<GetCount()-1; i++)
		{
			const ARCVector2& offsetHeigh1 = m_vOffsetHigth[i];
			const ARCVector2& offsetHeght2 = m_vOffsetHigth[i+1];
			
			if( offsetHeigh1.x == offset)
				return offsetHeigh1.y;
			if( offsetHeght2.x == offset)
				return offsetHeght2.y;

			if( offsetHeigh1.x < offset && offsetHeght2.x > offset )
			{
				DistanceUnit dRat = ( offset - offsetHeigh1.x ) / (offsetHeght2.x - offsetHeigh1.x );
				return offsetHeigh1.y * (1-dRat) + offsetHeght2.y * dRat;
			}
		}
		return 0;
	}

	//get the offset , only get the first meet indexs
	std::vector<int>  getHeightRangeOffsetIndexs(const DistanceUnit& dHeightFrom, const DistanceUnit& dHeightTo )const
	{
		std::vector<int> vIndexs;
		bool bStarted = false;

		for(int i=0;i < (int) m_vOffsetHigth.size(); i++)
		{
			const ARCVector2& v = m_vOffsetHigth[i];
			if( v.y > dHeightFrom && v.y < dHeightTo )
			{
				vIndexs.push_back(i);
				bStarted = true;
			}
			else 
			{
				if(bStarted)
					break;
			}
		}
		return vIndexs;
	}

	DistanceUnit getMaxOffset()
	{ 
		if(m_vOffsetHigth.size())
		{
			return m_vOffsetHigth.rbegin()->x;
		}
		else 
			return 100*SCALE_FACTOR;
	}
	

	void BuildHeightMap(ObjSurface& face)
	{

		DistanceUnit dOffset = face.dOffset;
		Add(dOffset, 0);

		DistanceUnit AccOffset = dOffset;
		DistanceUnit AccHeight = 0;

		for(int i=0;i< face.getCount();i++)
		{
			SurfaceData* pData = face.getItem(i);

			DistanceUnit dLen = pData->dDistance;
			DistanceUnit dAngle = pData->nAngle;

			DistanceUnit dHeight;
			if(dAngle == 90){
				dLen = 0;
				dHeight = DMaxHeight;
			}
			else{
				dHeight =  dLen * std::tan( ARCMath::DegreesToRadians(dAngle));			
			}

			AccOffset+= dLen;
			AccHeight += dHeight;

			Add(AccOffset,AccHeight);
		}


	}
};

