#pragma once



typedef double DistanceUnit;

class ARCMath 
{

private:
	static const DistanceUnit DEGREES_TO_RADIANS;
	static const DistanceUnit RADIANS_TO_DEGREES;


	ARCMath(); //cannot construct ARCMath objects
	~ARCMath();
public:
	static const DistanceUnit EPSILON;
	static const DistanceUnit NM_TO_DIST_UNITS;
	static const DistanceUnit DISTANCE_INFINITE;

	static DistanceUnit DegreesToRadians(DistanceUnit _dAngle)
		{ return DEGREES_TO_RADIANS*_dAngle; }
	static DistanceUnit RadiansToDegrees(DistanceUnit _dAngle)
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