#pragma once

class Vector2i
{
public:
	Vector2i(){}
	Vector2i(int _x, int _y){ x = _x; y=_y; }

	int x;
	int y;


	Vector2i& operator+=(const Vector2i& other)
	{
		x+= other.x;
		y+= other.y;
		return *this;
	}
	Vector2i& operator-=(const Vector2i& other)
	{
		x-= other.x;
		y-= other.y;
		return *this;
	}

	Vector2i operator -(const Vector2i& other)const
	{ 		
		return Vector2i(x - other.x, y - other.y );
	}

	Vector2i operator +(const Vector2i& other)const
	{ 
		return Vector2i(x + other.x, y + other.y );
	}

	bool operator!=(const Vector2i& other)const
	{
		return (x!=other.x) || (y!=other.y);
	}

	Vector2i operator-()const
	{
		return Vector2i(-x,-y);
	}
	bool operator==(const Vector2i& other)const
	{
		return !((*this)!=other);
	}
};
