#ifndef ARC_MATH_COMMON_H
#define ARC_MATH_COMMON_H

class ARCMath 
{

private:
	static const double DEGREES_TO_RADIANS;
	static const double RADIANS_TO_DEGREES;


	ARCMath(); //cannot construct ARCMath objects
	~ARCMath();
public:
	static const double EPSILON;
	static const double NM_TO_DIST_UNITS;
	static const double DISTANCE_INFINITE;

	static double DegreesToRadians(double _dAngle)
		{ return DEGREES_TO_RADIANS*_dAngle; }
	static double RadiansToDegrees(double _dAngle)
		{ return RADIANS_TO_DEGREES*_dAngle; }


	static int RoundTo2n(int);
	static int FloorTo2n(int);

	static bool isAllNumeric(const char* buf);
	static void NextAlphaNumeric(char* buf);

	//get a rand number form [0.0, 1.0] 
	static double Randf();

};

enum {VX=0, VY=1, VZ=2, VW=3};		    // axes
enum {RED=0, GREEN=1, BLUE=2, ALPHA=3}; // colors

#endif //ARC_MATH_COMMON_H
