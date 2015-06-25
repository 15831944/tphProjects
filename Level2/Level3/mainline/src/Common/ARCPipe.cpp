#include "StdAfx.h"

#include "ARCPipe.h"
#include "path2008.h"

ARCPipe::ARCPipe( const CPath2008& pts, double width, bool bLoop/* = false*/ )
{	
	Init(pts,width, bLoop);
}

ARCPipe::ARCPipe( const ARCPath3&  pts, double width, bool bLoop/* = false*/ )
{
	Init(pts.getPath2008(),width, bLoop);
}

ARCPipe::ARCPipe( const CPath2008& pts, double dStartWidth, double dEndWidth )
{
	Init(pts,dStartWidth, dEndWidth);
}

ARCPipe::ARCPipe( const ARCPath3&  pts, double dStartWidth, double dEndWidth )
{
	Init(pts.getPath2008(), dStartWidth, dEndWidth);
}
// Voffset = Voffset1 + Voffset2
// = -(dHalfWidth/sinTheta)*V1/length(V1) + (dHalfWidth/sinTheta)*V2/length(V2)
// = (V2/length(V2) - V1/length(V1))*(dHalfWidth/sinTheta)
static CPoint2008 GetOffsetToCenterPoint(const CPoint2008& pt1, const CPoint2008& pt2, const CPoint2008& pt3, double dHalfWidth)
{
	CPoint2008 V1(pt1, pt2); V1.setZ(0.0);
	CPoint2008 V2(pt2, pt3); V2.setZ(0.0);

	V1.length(1.0);
	V2.length(1.0);
	double cosTheta = V1*V2;

	CPoint2008 V1_norm = V1.perpendicular();

	double sinTheta = V1_norm*V2;
	if (abs(sinTheta) < 1e-6/*ARCMath::EPSILON*/) // two vector is parallel or negative
	{
		return V1_norm*dHalfWidth;
	}

	return CPoint2008(V2 - V1)*(dHalfWidth/sinTheta);
}

void ARCPipe::Init( const CPath2008&pts, double width, bool bLoop/* = false*/ )
{
	m_dWidth = width;
	m_dEndWidth = width;

	if(pts.getCount()<2)return;

	const int c = pts.getCount();
	const double dHalfWidth = width*0.5;

	{
		CPoint2008 c0(pts[0], pts[1]);
		CPoint2008 p0 = c0.perpendicular();
		p0.length( dHalfWidth );
		m_sidePath1.push_back( pts[0] + p0 );
		m_sidePath2.push_back( pts[0] - p0 );
		m_centerPath.push_back(pts[0]);
	}

	for(int j=1; j<c-1; j++)
	{
		CPoint2008 ptOffset = GetOffsetToCenterPoint(pts[j-1], pts[j], pts[j+1], dHalfWidth);
		m_sidePath1.push_back(pts[j] + ptOffset);
		m_sidePath2.push_back(pts[j] - ptOffset);
		m_centerPath.push_back(pts[j]);
	}

	if (!(bLoop && c>=3))
	{
		CPoint2008 c0(pts[c-2], pts[c-1]);
		CPoint2008 p0 = c0.perpendicular();
		p0.length( dHalfWidth );
		m_sidePath1.push_back(pts[c-1] + p0);
		m_sidePath2.push_back(pts[c-1] - p0);
		m_centerPath.push_back(pts[c-1]);
	}
	else // do loop calculation
	{
		CPoint2008 ptOffsetLast = GetOffsetToCenterPoint(pts[c-2], pts[c-1], pts[0], dHalfWidth);
		m_sidePath1.push_back(pts[c-1] + ptOffsetLast);
		m_sidePath2.push_back(pts[c-1] - ptOffsetLast);
		m_centerPath.push_back(pts[c-1]);

		CPoint2008 ptOffsetFirst = GetOffsetToCenterPoint(pts[c-1], pts[0], pts[1], dHalfWidth);
		m_sidePath1.push_back(pts[0] + ptOffsetFirst);
		m_sidePath2.push_back(pts[0] - ptOffsetFirst);
		m_centerPath.push_back(pts[0]);

		// modify first point
		m_sidePath1[0] = pts[0] + ptOffsetFirst;
		m_sidePath2[0] = pts[0] - ptOffsetFirst;
// 		m_centerPath[0] = pts[0];
	}

}

void ARCPipe::Init( const CPath2008& pts, double dStartWidth, double dEndWidth )
{
	m_dWidth = dStartWidth;
	m_dEndWidth = dEndWidth;

	if(pts.getCount()<2)return;

	const int c = pts.getCount();
	const double dStartHalfWidth = m_dWidth*0.5;
	const double dEndHalfWidth = m_dEndWidth*0.5;
	const double dWidthChangeRate = (dEndHalfWidth - dStartHalfWidth)/pts.GetTotalLength();

	double dCurLen = 0.0;
	double dHalfWidth = dStartHalfWidth;
	{
		CPoint2008 c0(pts[0], pts[1]);
		c0.setZ(0.0);
		CPoint2008 p0 = c0.perpendicular();
		p0.length( dStartHalfWidth );
		m_sidePath1.push_back( pts[0] + p0 );
		m_sidePath2.push_back( pts[0] - p0 );
		m_centerPath.push_back(pts[0]);
	}

	for(int j=1; j<c-1; j++)
	{
		double dLen = pts[j].distance(pts[j-1]);
		dHalfWidth += dWidthChangeRate*dLen;
		CPoint2008 ptOffset = GetOffsetToCenterPoint(pts[j-1], pts[j], pts[j+1], dHalfWidth);
		m_sidePath1.push_back(pts[j] + ptOffset);
		m_sidePath2.push_back(pts[j] - ptOffset);
		m_centerPath.push_back(pts[j]);
	}

	CPoint2008 c0(pts[c-2], pts[c-1]);
	CPoint2008 p0 = c0.perpendicular();
	p0.length( dHalfWidth );
	m_sidePath1.push_back(pts[c-1] + p0);
	m_sidePath2.push_back(pts[c-1] - p0);
	m_centerPath.push_back(pts[c-1]);
}
// void ARCPipe::Init( const CPath2008&pts, double width )
// {
// 	m_dWidth = width;
// 
// 	if(pts.getCount()<2)return;
// 
// 	const int c = pts.getCount();
// 	const double dHalfWidth = width*0.5;
// 	{
// 		CPoint2008 c0(pts[0], pts[1]);
// 		CPoint2008 p0 = c0.perpendicular();
// 		p0.length( dHalfWidth );
// 		m_sidePath1.push_back( ARCVector3(p0 + pts[0]) );
// 		p0 = c0.perpendicularY();
// 		p0.length( dHalfWidth );
// 		m_sidePath2.push_back( ARCVector3(p0 + pts[0]) );
// 		//vBisectOffsets.push_back(0.0);
// 		m_centerPath.push_back(pts[0]);
// 	}
// 
// 	//calculate bisect lines for middle segments
// 	for(int j=1; j<c-1; j++) {
// 		CPoint2008 c0(pts[j-1], pts[j]);
// 		CPoint2008 c1(pts[j], pts[j+1]);
// 
// 		CPoint2008 p0 = c0.perpendicular();
// 		p0.length( 1.0 );
// 		CPoint2008 p1 = c1.perpendicular();
// 		p1.length( 1.0 );
// 		p1+=p0;
// 		p1*=0.5; //avg
// 
// 		//find angle between bisect line and c1
// 		double dCosVal = p1.GetCosOfTwoVector( c1 );
// 		double dAngle = acos( dCosVal );
// 		double dAdjustedWidth = dHalfWidth / sin( dAngle );
// 		//vBisectOffsets.push_back(0.01 * dHalfWidth / tan( dAngle ));
// 		p1.length( dAdjustedWidth );
// 		m_sidePath1.push_back(pts[j] + p1);
// 
// 		p0 = c0.perpendicularY();
// 		p0.length( 1.0 );
// 		p1 = c1.perpendicularY();
// 		p1.length( 1.0 );
// 		p1+=p0;
// 		p1*=0.5;
// 		p1.length( dAdjustedWidth );
// 		m_sidePath2.push_back(pts[j] + p1);
// 
// 		m_centerPath.push_back(pts[j]);
// 	}
// 
// 	CPoint2008 c0(pts[c-2], pts[c-1]);
// 	CPoint2008 p0 = c0.perpendicular();
// 	p0.length( dHalfWidth );
// 	m_sidePath1.push_back(p0 + pts[c-1]);
// 	p0 = c0.perpendicularY();
// 	p0.length( dHalfWidth );
// 	m_sidePath2.push_back(p0 + pts[c-1]);
// 
// 	m_centerPath.push_back(pts[c-1]);
// }