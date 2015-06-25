#include "StdAfx.h"
#include ".\biziercurve.h"


int BizierCurve::ntop=6;
double BizierCurve::a[33]={1.0,1.0,2.0,6.0,24.0,120.0,720.0};
double BizierCurve::factrl(int n)
{	
	int j1;
	if (n < 0){
		printf("\nNegative factorial in routine FACTRL\n");
		return 0;
	}
	if (n > 32){
		printf("\nFactorial value too large in routine FACTRL\n");
		return 0;
	}
	while (ntop < n) 
	{ /* use the precalulated value for n = 0....6 */
		j1 = ntop++;
		a[n]=a[j1]*ntop;
	}
	return a[n]; /* returns the value n! as a floating point number */
}
double BizierCurve:: Ni(int n,int i)
{
	double ni;
	ni = factrl(n)/(factrl(i)*factrl(n-i));
	return ni;
}
double BizierCurve::Basis(int n,int i,double t)
{
	double basis;
	double ti; /* this is t^i */
	double tni; /* this is (1 - t)^(n-i) */

	/* handle the special cases to avoid domain problem with pow */

	if (t==0.0 && i == 0) ti=1.0; else ti = pow(t,i);
	if (n==i && t==1.0) tni=1.0; else tni = pow((1-t),(n-i));
	basis = Ni(n,i)*ti*tni; /* calculate Bernstein basis function */
	return basis;
}
void BizierCurve::GenCurvePoints(const std::vector<Point> & input,std::vector<Point> & output,int nOutCounts ){
	output.clear();
	int ncount=input.size();
	if(ncount<2)return;
	double step=1.0/(double )(nOutCounts-1);
	double t=0.0;

	//generate the points
	for(int i=0;i<nOutCounts;i++){
		if((1.0-t)<5e-6)t=1.0;
		Point p;
		p.setX(0.0);p.setY(0.0);p.setZ(0.0);		
		for(int j=0;j<ncount;j++){                 
			p.setX(p.getX()+Basis(ncount-1,j,t)*input[j].getX());
			p.setY(p.getY()+Basis(ncount-1,j,t)*input[j].getY());
			p.setZ(p.getZ()+Basis(ncount-1,j,t)*input[j].getZ());
		}
		output.push_back(p); 
		t+=step;             
	}  
}

void BizierCurve::GenCurvePoints(const std::vector<CPoint2008> & input,std::vector<CPoint2008> & output,int nOutCounts)
{
	output.resize(nOutCounts);
	int ncount=input.size();
	if(ncount<2)return;
	double step=1.0/(double )(nOutCounts-1);
	double t=0.0;

	//generate the points
	for(int i=0;i<nOutCounts;i++){
		if((1.0-t)<5e-6)t=1.0;
		CPoint2008 p;
		p.setX(0.0);p.setY(0.0);p.setZ(0.0);		
		for(int j=0;j<ncount;j++){                 
			p.setX(p.getX()+Basis(ncount-1,j,t)*input[j].getX());
			p.setY(p.getY()+Basis(ncount-1,j,t)*input[j].getY());
			p.setZ(p.getZ()+Basis(ncount-1,j,t)*input[j].getZ());
		}
		output[i] = p; 
		t+=step;             
	}  
}

void BizierCurve::GenCurvePoints(const std::vector<ARCVector3> & input,std::vector<ARCVector3> & output,int nOutCounts)
{
	output.clear();
	int ncount=input.size();
	if(ncount<2)return;
	double step=1.0/(double )(nOutCounts-1);
	double t=0.0;

	//generate the points
	for(int i=0;i<nOutCounts;i++){
		if((1.0-t)<5e-6)t=1.0;
		ARCVector3 p(0,0,0);				
		for(int j=0;j<ncount;j++){                 
			p[VX]=(p[VX]+Basis(ncount-1,j,t)*input[j][VX]);
			p[VY]=(p[VY]+Basis(ncount-1,j,t)*input[j][VY]);
			p[VZ]=(p[VZ]+Basis(ncount-1,j,t)*input[j][VZ]);
		}
		output.push_back(p); 
		t+=step;             
	}  

}
