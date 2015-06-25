#pragma once

class Vector3Base
{
public:
	union{
		struct{
			double x, y, z;	// z is the floor. 000 for 0, 100 for 1, 200 for 2
		};
		double val[3];
		double n[3];
	};
};


class Vector2Base
{
public:
	union{
		struct{
			double x, y;
		};
		double val[2];
		double n[2];
	};
};