#include "StdAfx.h"
#include "NaturalCubicSpline.h"

void NaturalCubicSpline::calcNaturalCubic(const CPath2008* pSrcPath, CPath2008& destPath)
{
//TODO
}

/*
	calculates the closed natural cubic spline that interpolates
	x[0], x[1], ... x[n]
	The first segment is returned as
	C[0].a + C[0].b*u + C[0].c*u^2 + C[0].d*u^3 0<=u <1
	the other segments are in C[1], C[2], ...  C[n] 
*/
void ClosedNaturalCubicSpline::calcNaturalCubic(const CPath2008* pSrcPath, CPath2008& destPath, int STEPS)
{
	int nSrcPtCount = pSrcPath->getCount();

	std::vector<double> pointsX(nSrcPtCount);
	std::vector<double> pointsY(nSrcPtCount);
	
	
	for (int i = 0 ; i < nSrcPtCount; i++)
	{
		pointsX[i] = pSrcPath->getPoint(i).getX();
		pointsY[i] = pSrcPath->getPoint(i).getY();
	}

	std::vector<Cubic>vectCubicX, vectCubicY;
	calcNaturalCubic(nSrcPtCount - 1, &pointsX[0], vectCubicX);
	calcNaturalCubic(nSrcPtCount - 1 , &pointsY[0], vectCubicY);

	//delete[] pointsX; delete[] pointsY;

	std::vector<CPoint2008> destVectPoints;
	CPoint2008 pt(vectCubicX[0].eval(0) , vectCubicY[0].eval(0), 0.0);
	destVectPoints.push_back(pt);

	for (int i = 0; i < (int)vectCubicX.size(); i++)
	{
		for (int j = 1; j <= STEPS; j++)
		{
			float u = j / (float) STEPS;
			pt.setX( vectCubicX[i].eval(u) );
			pt.setY( vectCubicY[i].eval(u) );
			destVectPoints.push_back( pt );
		}
	}

	ASSERT( vectCubicX.size() * STEPS + 1 <= 1024);

	int nCount = destVectPoints.size();
	std::vector<CPoint2008> ptList;//[1024];
	ptList.resize(nCount);
	for(size_t i=0; i<destVectPoints.size(); i++)
	{
		ptList[i].setX(destVectPoints.at(i).getX());
		ptList[i].setY(destVectPoints.at(i).getY());
		ptList[i].setZ(0.0);
	}
	destPath.init(nCount, &ptList[0]);

}

void ClosedNaturalCubicSpline::calcNaturalCubic(int n, double* x, std::vector<Cubic>& vectCubic)
{
	std::vector<double> w(n+1);//new double[n+1];
	std::vector<double> v(n+1);// = new double[n+1];
	std::vector<double> y(n+1);// = new double[n+1];
	std::vector<double> D(n+1);// = new double[n+1];
	double z, F, G, H;
	int k;
/* 
	We solve the equation
	[4 1      1] [D[0]]   [3(x[1] - x[n])  ]
	|1 4 1     | |D[1]|   |3(x[2] - x[0])  |
	|  1 4 1   | | .  | = |      .         |
	|    ..... | | .  |   |      .         |
	|     1 4 1| | .  |   |3(x[n] - x[n-2])|
	[1      1 4] [D[n]]   [3(x[0] - x[n-1])]

	by decomposing the matrix into upper triangular and lower matrices
	and then back sustitution. 
*/
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


	/* now compute the coefficients of the cubics */

	for ( k = 0; k < n; k++) {
		vectCubic.push_back(  Cubic((double)x[k], D[k], 3*(x[k+1] - x[k]) - 2*D[k] - D[k+1],
			2*(x[k] - x[k+1]) + D[k] + D[k+1]) );
	}
	vectCubic.push_back( Cubic((double)x[n], D[n], 3*(x[0] - x[n]) - 2*D[n] - D[0],
		2*(x[n] - x[0]) + D[n] + D[0]) );
	
	//delete[] w, delete[] v, delete[] y, delete[]D;

	return ;

}