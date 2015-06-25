#include "stdafx.h"
#include "cubic_spline.h"


struct CubicSplineBuilder
{
	static void calc_cubic( const std::vector<double>& x, std::vector<CubicFunc>& vectCubic)
	{
		
		if(x.size() <=0 ){
			vectCubic.resize(0);
			return;
		}
		if( x.size() == 1)
		{
			vectCubic.resize(1);
			vectCubic[0] = CubicFunc(x[0],0,0,0);
			return;
		}
		
		const int n = x.size()-1;
		std::vector<double> w(n+1); //double *w = w_.get_data();
		std::vector<double> v(n+1); //double *v = v_.get_data();
		std::vector<double> y(n+1); //double *y = y_.get_data();
		std::vector<double> D(n+1); //double *D = D_.get_data();

		double z, F, G, H;
		int k;

		w[1] = v[1] = z = 1.0f/4.0f;
		y[0] = z * 3 * (x[1] - x[n]);
		H = 4;
		F = 3 * (x[0] - x[n-1]);
		G = 1;
		for ( k = 1; k < n; k++) {
			v[k+1] = z = 1/(4 - v[k]);
			w[k+1] = -z * w[k];
			y[k] = z * (3*(x[k+1]-x[k-1]) - y[k-1]);
			H = H - G * w[k];
			F = F - G * y[k-1];
			G = -v[k] * G;
		}
		H = H - (G+1)*(v[n]+w[n]);
		y[n] = F - (G+1)*y[n-1];

		D[n] = y[n]/H;
		D[n-1] = y[n-1] - (v[n]+w[n])*D[n];
		for ( k = n-2; k >= 0; k--) {
			D[k] = y[k] - v[k+1]*D[k+1] - w[k+1]*D[n];
		}

		for ( k = 0; k < n; k++) 
			vectCubic.push_back(  CubicFunc((double)x[k], D[k], 3*(x[k+1] - x[k]) - 2*D[k] - D[k+1], 2*(x[k] - x[k+1]) + D[k] + D[k+1]) );
		vectCubic.push_back( CubicFunc((double)x[n], D[n], 3*(x[0] - x[n]) - 2*D[n] - D[0], 2*(x[n] - x[0]) + D[n] + D[0]) );
	}
	//
	static void calc_cubic_noclose( const std::vector<double>& x, std::vector<CubicFunc>& vectCubic)
	{		
		if(x.size() <=0 ){
			vectCubic.resize(0);
			return;
		}
		
		if(x.size() ==1)
		{
			vectCubic.resize(1);
			vectCubic[0] = CubicFunc(x[0],0,0,0);
			return;
		}

		//
		int n = x.size()-1;
		vectCubic.resize(n);

		std::vector<double> gamma(n+1);
		std::vector<double> delta(n+1);
		std::vector<double> D(n+1);
		int i;
		/* We solve the equation
		[2 1       ] [D[0]]   [3(x[1] - x[0])  ]
		|1 4 1     | |D[1]|   |3(x[2] - x[0])  |
		|  1 4 1   | | .  | = |      .         |
		|    ..... | | .  |   |      .         |
		|     1 4 1| | .  |   |3(x[n] - x[n-2])|
		[       1 2] [D[n]]   [3(x[n] - x[n-1])]

		by using row operations to convert the matrix to upper triangular
		and then back sustitution.  The D[i] are the derivatives at the knots.
		*/

		gamma[0] = 1.0f/2.0f;
		for ( i = 1; i < n; i++) {
			gamma[i] = 1/(4-gamma[i-1]);
		}
		gamma[n] = 1/(2-gamma[n-1]);

		delta[0] = 3*(x[1]-x[0])*gamma[0];
		for ( i = 1; i < n; i++) {
			delta[i] = (3*(x[i+1]-x[i-1])-delta[i-1])*gamma[i];
		}
		delta[n] = (3*(x[n]-x[n-1])-delta[n-1])*gamma[n];

		D[n] = delta[n];
		for ( i = n-1; i >= 0; i--) {
			D[i] = delta[i] - gamma[i]*D[i+1];
		}

		/* now compute the coefficients of the cubics */		
		for ( i = 0; i < n; i++) {
			vectCubic[i] = CubicFunc( x[i], D[i], 3*(x[i+1] - x[i]) - 2*D[i] - D[i+1],
				2*(x[i] - x[i+1]) + D[i] + D[i+1]);
		}
		
	}
};


void CubicSpline2::build( const std::vector<ARCVector2>& src )
{
	int num = src.size();

	m_vect_cubicX.clear();
	m_vect_cubicY.clear();
	
	std::vector<double> pointsX(num);// double *pointsX = pointsX_.get_data();
	std::vector<double> pointsY(num);// double *pointsY = pointsY_.get_data();	
	for (int i=0 ; i<num; i++)
	{
		const ARCVector2& pos = src[i];
		pointsX[i] = pos.x;
		pointsY[i] = pos.y;		
	}

	if(m_type == Closed)
	{
		CubicSplineBuilder::calc_cubic( pointsX, m_vect_cubicX);
		CubicSplineBuilder::calc_cubic( pointsY, m_vect_cubicY);		
	}
	else
	{
		CubicSplineBuilder::calc_cubic_noclose( pointsX, m_vect_cubicX);
		CubicSplineBuilder::calc_cubic_noclose( pointsY, m_vect_cubicY);		
	}
}


void CubicSpline3::build( const std::vector<ARCVector3>& src )
{
	int num = src.size();

	m_vect_cubicX.clear();
	m_vect_cubicY.clear();
	m_vect_cubicZ.clear();

	std::vector<double> pointsX(num);// double *pointsX = pointsX_.get_data();
	std::vector<double> pointsY(num);// double *pointsY = pointsY_.get_data();
	std::vector<double> pointsZ(num);// double *pointsZ = pointsZ_.get_data();
	for (int i=0 ; i<num; i++)
	{
		const ARCVector3& pos = src[i];
		pointsX[i] = pos.x;
		pointsY[i] = pos.y;
		pointsZ[i] = pos.z;
	}

	if(m_type == Closed)
	{
		CubicSplineBuilder::calc_cubic( pointsX, m_vect_cubicX);
		CubicSplineBuilder::calc_cubic( pointsY, m_vect_cubicY);
		CubicSplineBuilder::calc_cubic( pointsZ, m_vect_cubicZ);
	}
	else
	{
		CubicSplineBuilder::calc_cubic_noclose( pointsX, m_vect_cubicX);
		CubicSplineBuilder::calc_cubic_noclose( pointsY, m_vect_cubicY);
		CubicSplineBuilder::calc_cubic_noclose( pointsZ, m_vect_cubicZ);
	}
}

ARCVector3 CubicSpline3::getLinePoint( double p )const
{
	if(p  < 0)
	{
		ASSERT(false);
		return ARCVector3(0,0,0);
	}
	int nMax = (int)m_vect_cubicX.size();
	double pTotal = p * nMax;
	int i = (int)pTotal;
	if(i > nMax-1)
	{
		//assert(false);
		i = nMax-1;
	}
	return getLinePoint( i, pTotal-i );
}

ARCVector3 CubicSpline3::getLinePoint( int i, double p ) const
{	
	ARCVector3 vec; 
	if(i < (int)m_vect_cubicX.size() && i>=0)
	{
		vec.x = m_vect_cubicX[i].eval(p);
		vec.y = m_vect_cubicY[i].eval(p);
		vec.z = m_vect_cubicZ[i].eval(p);
	}
	return vec;	
}

ARCVector3 CubicSpline3::getLineDir( int i, double p ) const
{
	ARCVector3 vNorm;
	if(i < (int)m_vect_cubicX.size() && i>=0)
	{
		vNorm.x = m_vect_cubicX[i].evalD(p);
		vNorm.y = m_vect_cubicY[i].evalD(p);
		vNorm.z = m_vect_cubicZ[i].evalD(p);
		vNorm.Normalize();
	}
	return vNorm;
}

ARCVector3 CubicSpline3::getLineDir( double p ) const
{
	if(p  < 0)
	{
		ASSERT(false);
		return ARCVector3(0,0,0);
	}
	int nMax = (int)m_vect_cubicX.size();
	double pTotal = p * nMax;
	int i = (int)pTotal;
	if(i > nMax-1)
	{
		//assert(false);
		i = nMax-1;
	}
	return getLineDir( i, pTotal-i );
}

bool CubicSpline3::GetXPosRoot( double dXpos, double& p ) const
{
	double dMax = (double)m_vect_cubicX.size();
	for(int i=0;i<(int)m_vect_cubicX.size();i++)
	{
		const CubicFunc& cubicF = m_vect_cubicX[i];
		double dVal0 = cubicF.eval(0);
		double dVal1 = cubicF.eval(1);
		if(dXpos == dVal0)
		{
			p = (i+0.0)/dMax;
			return true;
		}
		if(dXpos == dVal1)
		{
			p = (i+1.0)/dMax;
			return true;
		}
		if(dXpos < dVal1 && dXpos > dVal0)
		{
			double dRoot;
			if( cubicF.getRoot(dXpos,dRoot) )
			{
				p = (i+dRoot)/dMax;
				return true;
			}
		}
	}

	return false;
}

void CubicSpline3::setLineType( int idx, LineType t )
{
	int nMaxCnt = max( max(m_vect_cubicX.size(), m_vect_cubicY.size()), m_vect_cubicZ.size() );
	if(idx<nMaxCnt)
	{
		m_vect_cubicX[idx].mLineType = t;
		m_vect_cubicY[idx].mLineType = t;
		m_vect_cubicZ[idx].mLineType = t;
	}
}

LineType CubicSpline3::getLineType( int idx ) const
{
	if(idx<(int)m_vect_cubicX.size())
	{
		return m_vect_cubicX[idx].mLineType;
	}
	return Cubic_Spline;
}

//
ARCVector2 CubicSpline2::getLinePoint( double  p )const
{
	if(p  < 0)
	{
		ASSERT(false);
		return ARCVector2(0,0);
	}
	int nMax = (int)m_vect_cubicX.size();
	double pTotal = p * nMax;
	int i = (int)pTotal;
	if(i > nMax-1)
	{
		//assert(false);
		i = nMax-1;
	}
	return getLinePoint( i, pTotal-i );
}

ARCVector2 CubicSpline2::getLinePoint( int i, double p ) const
{	
	
	ARCVector2 vec; 
	if(i < (int)m_vect_cubicX.size() && i>=0)
	{		
		vec.x = m_vect_cubicX[i].eval(p);
		vec.y = m_vect_cubicY[i].eval(p);		
	}
	return vec;	
}

ARCVector2 CubicSpline2::getLineDir( double p ) const
{
	if(p  < 0)
	{
		ASSERT(false);
		return ARCVector2(0,0);
	}
	int nMax = (int)m_vect_cubicX.size();
	double pTotal = p * nMax;
	int i = (int)pTotal;
	if(i > nMax-1)
	{
		//assert(false);
		i = nMax-1;
	}
	return getLineDir( i, pTotal-i );
}

ARCVector2 CubicSpline2::getLineDir( int i, double p ) const
{
	ARCVector2 vNorm;
	if(i<(int)m_vect_cubicX.size()&&i>=0)
	{
		vNorm.x = m_vect_cubicX[i].evalD(p);
		vNorm.y = m_vect_cubicY[i].evalD(p);	
		vNorm.Normalize();
	}
	return vNorm;
}

void CubicSpline2::GetPointList( int nPtCnt, std::vector<ARCVector2>& vOut )const
{
	vOut.resize(nPtCnt);
	for(int i=0;i<nPtCnt;i++)
	{
		double dStep = i/double(nPtCnt);
		vOut[i] = getLinePoint(dStep);
	}
}

void CubicSpline2::setLineType( int idx, LineType t )
{
	int nMaxCnt =  max(m_vect_cubicX.size(), m_vect_cubicY.size()) ;
	if(idx<nMaxCnt)
	{
		m_vect_cubicX[idx].mLineType = t;
		m_vect_cubicY[idx].mLineType = t;		
	}
}

LineType CubicSpline2::getLineType( int idx ) const
{
	if(idx<(int)m_vect_cubicX.size())
	{
		return m_vect_cubicX[idx].mLineType;
	}
	return Cubic_Spline;
}

bool CubicFunc::getRoot( double dval, double& dRoot )const
{
	CubicFunc cubicF = CubicFunc(_a - dval, _b,_c,_d);
	double dR1 = 0;
	double dR2 = 1;
    
	double dVal1 = cubicF.eval(dR1);
	double dVal2 = cubicF.eval(dR2);

	if(dVal1 == 0 )
	{
		dRoot = dR1;
		return true;
	}
	if(dVal2 == 0)
	{
		dRoot = dR2;
		return true;
	}

	do
	{
		double dMid = (dR1+dR2)*0.5;
		double dValMid = cubicF.eval(dMid);
		if( dValMid == 0)
		{
			dRoot = dMid;
			return true;
		}
			

		if( dValMid* dVal1 <0)
		{
			dR2 = dMid;
		}
		else if(dValMid * dVal2 <0)
		{
			dR1 = dMid;
		}
		else
		{
			return false;
		}

		dVal1 = cubicF.eval(dR1);
		dVal2 = cubicF.eval(dR2);
		
	}while(abs(dR1-dR2) > ARCMath::EPSILON);
	
	dRoot =  (dR1+dR2)*0.5;
	return true;
}

double CubicFunc::evalLine( double u ) const
{
	return (_d+_c+_b)*u+_a;
}

double CubicFunc::evalLineD( double u ) const
{
	return _d+_c+_b;
}

double CubicFunc::evalD( double u ) const
{
	return (mLineType==Cubic_Spline)?evalCubicD(u):evalLineD(u);
}