#include "math.h"
#include "AirsideFlightInfo.h"
#define PI 3.14159265f
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
};
