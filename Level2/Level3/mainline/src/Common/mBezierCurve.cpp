#include "stdafx.h"
#include "mBezierCurve.h"


double mBezierCurve::Basis(int n,int i,double t) const
{
	static Ni ni;
	double basis;
	double ti; /* this is t^i */
	double tni; /* this is (1 - t)^(n-i) */

	/* handle the special cases to avoid domain problem with pow */

	if (t==0.0 && i == 0) ti=1.0; else ti = pow(t,i);
	if (n==i && t==1.0) tni=1.0; else tni = pow((1-t),(n-i));
	basis = ni(n,i)*ti*tni; /* calculate Bernstein basis function */
	return basis;
}

Point mBezierCurve::GetMidPoint(double t)  const
{
	Point pt;
	int nCount = m_pKeyPoints->size();
	for(int i=0; i<nCount; i++ )
	{
		pt += (*m_pKeyPoints)[i] * Basis(nCount-1,i,t) ;
	}

	return pt;
}

double mBezierCurve::GetLength( int n )const 
{
	double length = 0;
	Point pt = *( *m_pKeyPoints ).begin();
	if(n<1)return 0;
	for( int i=0;i<n-1;i++)
	{		
		Point nextPt = GetMidPoint( i/n );
		length += pt.distance( nextPt );
		pt = nextPt;
	}
	return length;
}
