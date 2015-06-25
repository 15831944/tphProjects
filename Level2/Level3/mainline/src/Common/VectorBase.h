#pragma once

typedef double DistanceUnit;

//
class Vector3Base
{
public:
	union{
		struct{
			DistanceUnit x, y, z;	// z is the floor. 000 for 0, 100 for 1, 200 for 2
		};
		DistanceUnit val[3];
		DistanceUnit n[3];
	};
};


class Vector2Base
{
public:
	union{
		struct{
			DistanceUnit x, y;
		};
		DistanceUnit val[2];
		DistanceUnit n[2];
	};
};