#include "stdafx.h"
#include "mFunctor.h"

uint Factorial::ntop=6;
double Factorial::a[maxX]={1.0,1.0,2.0,6.0,24.0,120.0,720.0,};
double Factorial::operator()(uint n)
{
	int j1;	
	if ( n > maxX-1 ) return -1;
	while (ntop < n) { /* use the precalulated value for n = 0....6 */
		j1 = ntop++;
		a[n]=a[j1]*ntop;
	}
	return a[n]; /* returns the value n! as a floating point number */
}

double Ni::operator ()(uint n,uint i)
{
	static Factorial factrl;
	return factrl(n)/(factrl(i)*factrl(n-i));
}