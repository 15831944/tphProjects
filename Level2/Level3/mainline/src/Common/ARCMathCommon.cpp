#include "stdafx.h"
#include "ARCMathCommon.h"
#include <limits>

const DistanceUnit ARCMath::DEGREES_TO_RADIANS=1.74532925199433E-002;
const DistanceUnit ARCMath::RADIANS_TO_DEGREES=5.72957795130823E+001;
const DistanceUnit ARCMath::EPSILON=0.001;
const DistanceUnit ARCMath::NM_TO_DIST_UNITS=1853001;
const DistanceUnit ARCMath::DISTANCE_INFINITE = (std::numeric_limits<DistanceUnit>::max)() * 0.5;

#define isNumeric(x)  ((x>=48&&x<=57)||x==43||x==45||x==46)

int ARCMath::RoundTo2n(int i)
{
	int res =1;
	while (res < i)
		res *= 2;
	return res;
}

int ARCMath::FloorTo2n(int i)
{
	int res = 1;
	while (res <= i)
		res *= 2;
	return res;
}

bool ARCMath::isAllNumeric( const char* buf )
{
	int len = (int) strlen(buf);
	for(int i=0; i<len; i++) {
		if(!isNumeric(buf[i]))
			return false;
	}
	return true;
}

void ARCMath::NextAlphaNumeric( char* buf )
{
	int len = (int) strlen(buf);
	if(len==1) {
		if(buf[0]=='Z')
			strcpy(buf, "AA");
		else
			buf[0]++;
	}
	else if(len==2) {
		if(buf[1]=='Z') {
			if(buf[0]!='Z') {
				buf[0]++;
				buf[1]='A';
			}
			else
				strcat(buf, "_COPY");
		}
		else {
			buf[1]++; //increment last
		}
	}
	else {
		strcat(buf, "_COPY");
	}
}

double ARCMath::Randf()
{
	return (double)rand()/RAND_MAX;
}
