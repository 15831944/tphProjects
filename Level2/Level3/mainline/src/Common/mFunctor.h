#ifndef __MATH_FUNCTOR_
#define __MATH_FUNCTOR_
#include <functional>
#include <limits>
#include <cmath>
typedef unsigned int uint; 
 
struct Factorial : public std::unary_function<uint,double>{
	const static int maxX = 40;
	static double a[maxX];
	static uint ntop;
	double operator()(uint n);
};

struct Ni : public std::binary_function<uint,uint,double>
{
	double operator()(uint n,uint i);
};



#endif