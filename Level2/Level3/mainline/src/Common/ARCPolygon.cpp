#include "stdafx.h"
#include "ARCLine.h"
#include "ARCPolygon.h"
#include "ARCRay.h"
//
// if a point is in a Polygon then the sum of angles which this point between two continous

bool ARCPolygon::Contains(const ARCPoint2& _pt) const
{		
	ARCRayIntersect intersect;	
	int intersectCount  = intersect.Get( ARCRay2(_pt,ARCVector2(1,0) ),*this);
	return (intersectCount & 1);
}

bool ARCPolygon::Contains(const ARCPolygon& _poly) const
{
	if( _poly.size() < 1 )
		return false;
	for(ARCPolygon::const_iterator it=_poly.begin(); it!=_poly.end(); it++) {
		if(!Contains(*it))
			return false;
	}
	return true;
}

bool ARCPolygon::Intersects(const ARCPoint2& _pt1, const ARCPoint2& _pt2) const
{	
	ARCRayIntersect intersect;	
	
	ARCVector2 vNorm = _pt2- _pt1;
	double dLen = vNorm.Length();
	vNorm.Normalize();

	int intersectCount  = intersect.Get( ARCRay2(_pt1,vNorm),*this);
	for(int i=0;i<intersectCount;i++)
	{
		if( intersect.rayDistList[i] <= dLen )
			return true;
	}
	return false;
}

void ARCPolygon::DivideAsConvexPolygons(std::vector<ARCPolygon>& _vResult) const
{
	ASSERT( size() >= 3 );
	if( IsConvex() )
	{
		_vResult.push_back( *this );
	}
	else
	{
		ARCPolygon thisCopy(*this);

		std::vector<ARCPoint2> tempTrianglePoint(3);
		ARCPolygon tempTriangle;

		bool bNotEnd = true;
		unsigned iStartPos =0;
		while( bNotEnd )
		{
			if ( iStartPos >= thisCopy.size() )
					return;
			
			tempTrianglePoint[0] = thisCopy[iStartPos];
			tempTrianglePoint[1] = thisCopy[iStartPos + 1];
			tempTrianglePoint[2] = thisCopy[iStartPos + 2];

			if( thisCopy.IsLineInsidePolygon( tempTrianglePoint[0], tempTrianglePoint[2] )  )
			{
				_vResult.push_back(ARCPolygon(tempTrianglePoint));
				thisCopy.erase( thisCopy.begin() + iStartPos + 1 );
				//thisCopy.UpdateBound(mbBox);
				iStartPos = 0;
				if( thisCopy.IsConvex() )
				{
					_vResult.push_back( thisCopy );
					bNotEnd = false;
				}
			}
			else
			{
				++iStartPos ;	
				
			}				
		}
	}

}
bool ARCPolygon::IsConvex() const{return true ;}

bool ARCPolygon::IsLineInsidePolygon(const ARCPoint2& _pt1, const ARCPoint2& _pt2)const 
{
	return Contains(_pt1) && Contains(_pt2);
}

bool ARCPolygon::IsOverlapWithOtherPolygon( const ARCPolygon& _poly ) const
{
	//quick test //assert bound is updated
	if(!mbBox.IsOverlap(_poly.mbBox))
		return false;

	for(int i=0;i<(int)_poly.size();i++)
	{
		if(i== (int)_poly.size()-1)
		{
			if( Intersects(_poly[i],_poly[0]) )
				return true;
		}
		else if( Intersects(_poly[i],_poly[i+1]) )
			return true;
	}

	return false;

}

bool ARCPolygon::UpdateBound()
{
	if(empty())
		return false;

	mbBox.Reset(*begin());
	for(int i=1;i<(int)size();++i)
	{
		mbBox.Merge( (*this)[i] );
		
	}
	return true;
}


void ARCPolygon::Build( const CPoint2008& pos, const ARCVector3& dir, double dHeadLen, double dBackLen, double dWidth )
{
	clear();
	//assert dir is normalized
	ARCVector2 widDir = dir.xy().PerpendicularLCopy();
	widDir.SetLength(dWidth*0.5);
	ARCVector2 headDir = dir.xy();
	headDir.SetLength(dHeadLen);
	ARCVector2 backDir = dir.xy();
	backDir.SetLength(-dBackLen);

	ARCVector2 p = ARCVector3(pos).xy();
	push_back( p + headDir - widDir);
	push_back( p + headDir + widDir);
	push_back( p + backDir + widDir);
	push_back( p + backDir - widDir);

	UpdateBound();

}

bool ARCPolygon::IsOverLap( const ARCPoint2& _pt1, const ARCPoint2& _pt2 ) const
{
	ARCRayIntersect intersect;	

	ARCVector2 vNorm = _pt2- _pt1;
	double dLen = vNorm.Length();
	vNorm.Normalize();

	int intersectCount  = intersect.Get( ARCRay2(_pt1,vNorm),*this);
	int nP1Cnt = intersectCount;
	int nP2Cnt = intersectCount - (std::upper_bound(intersect.rayDistList.begin(),intersect.rayDistList.end(), dLen) - intersect.rayDistList.begin());
	if(nP1Cnt != nP2Cnt)
		return true;

	return nP1Cnt&1;
}

bool ARCBoundBox::IsOverlap( const ARCBoundBox& otherBox ) const
{
	return ( m_ptTR.x > otherBox.m_ptBL.x ) && ( otherBox.m_ptTR.x > m_ptBL.x )	 &&  ( m_ptTR.y > otherBox.m_ptBL.y ) && ( otherBox.m_ptTR.y > m_ptBL.y );
}