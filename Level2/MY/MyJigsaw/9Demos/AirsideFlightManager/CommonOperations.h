#pragma once
#include "math.h"
#include <stdlib.h>


#define PI 3.14159265f

class PointFXY
{
public:
    PointFXY(){}
    PointFXY(float x, float y): m_x(x), m_y(y){}
    ~PointFXY(){}

private:
    float m_x; // x coordinate of top-left point.
    float m_y; // y coordinate of top-left point.
public:
    void SetZero(){ m_x = 0; m_y = 0; }

    float GetX() const { return m_x; }
    void SetX(float val) { m_x = val; }

    float GetY() const { return m_y; }
    void SetY(float val) { m_y = val; }

    PointFXY operator-(const PointFXY& other)
    {
        return PointFXY(m_x-other.m_x, m_y-other.m_y);
    }

    PointFXY operator+(const PointFXY& other)
    {
        return PointFXY(m_x+other.m_x, m_y+other.m_y);
    }

    void operator+=(const PointFXY& other)
    {
        m_x+=other.m_x;
        m_y+=other.m_y;
        return ;
    }
};

class CommonOperations
{
private:
    CommonOperations();
    ~CommonOperations();

public:
    static PointFXY GetRotatedPointFXY(const PointFXY& ptOri, const PointFXY ptCenter, float fAngle)
    {
        float x = (ptOri.GetX()-ptCenter.GetX())*sin(fAngle*PI/180) + 
            (ptOri.GetY()-ptCenter.GetY())*cos(fAngle*PI/180) + ptCenter.GetX();
        float y = (ptOri.GetX()-ptCenter.GetX())*cos(fAngle*PI/180) +
            (ptOri.GetY()-ptCenter.GetY())*sin(fAngle*PI/180) + ptCenter.GetY();
        return PointFXY(x, y);
    }

    static PointFXY GetRandomPointFXY(PointFXY ptMax)
    {
        float x = (float)(rand() % ((int)ptMax.GetX()));
        float y = (float)(rand() % ((int)ptMax.GetY()));
        return PointFXY(x, y);
    }

    static int GetRandom(int iMax)
    {
        return rand() % iMax;
    }
};
