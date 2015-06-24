#include "StdAfx.h"
#include ".\billboardproc.h"
#include "proclist.h"


BillboardProc::BillboardProc(void)
{
	m_dHeight =  100.0;
//	m_dExposureAreaRadius =  0.0;
	m_nType = 0;
	m_strText = _T("No text");
	m_strBitmapPath = _T("No bitmap");
//	m_dCoefficient = 0.0;
	m_dThickness = 100.0;
//	m_nAngle = 90;

	m_ibmpModStamp = 0;
	m_iTextModeStamp = 0;
	//gamma,bata,mu
	m_gammaDistribution.init(2.00,1.00,0.00);
}

BillboardProc::BillboardProc( const BillboardProc& aProc )
{
	*this = aProc;
}

BillboardProc& BillboardProc::operator=( const BillboardProc& rhs )
{
	Processor::operator=(rhs);
	BillboardProp::operator=(rhs);
	return *this;
}

BillboardProc::~BillboardProc(void)
{
}
void BillboardProc::initServiceLocation (int pathCount, const Point *pointList)
{
	if (pathCount <= 1)
		throw new StringError ("Line Processors must have at least 2 point service locations");

	m_location.init (pathCount, pointList);
	UpdateMinMax();
}

int BillboardProc::readSpecialField(ArctermFile& procFile)
{
	procFile.getFloat(m_dHeight );
	procFile.getFloat(m_dThickness);
//	procFile.writeFloat( (float)m_dExposureAreaRadius );
//	procFile.getInteger(m_nAngle);
	procFile.getInteger(m_nType);

	char chText[1024];
	procFile.getField (chText, 1024);
	m_strText = chText;

	char chPath[1024];
	procFile.getField (chPath, 1024);
	m_strBitmapPath = chPath;

//	procFile.getFloat(m_dCoefficient );
	
	//int nPathCount;
	//procFile.getInteger(nPathCount);
	//Point ExposurePoint[MAX_POINTS]; 
	//for (int i =0; i <nPathCount; i++)
	//{
	//	Point tempPoint;
	//	procFile.getPoint(tempPoint);
	//	ExposurePoint[i] = tempPoint;
	//}
	//m_pathExpsure.init(nPathCount,ExposurePoint);

	return 1;
}

int BillboardProc::writeSpecialField(ArctermFile& procFile) const
{
	procFile.writeFloat((float)m_dHeight );
	procFile.writeFloat((float) m_dThickness);
//	procFile.writeFloat( (float)m_dExposureAreaRadius );
//	procFile.writeInt(m_nAngle);
	procFile.writeInt(m_nType);
	procFile.writeField(m_strText );
	procFile.writeField(m_strBitmapPath );
//	procFile.writeField(m_strTextOrPath);
//	procFile.writeFloat( (float)m_dCoefficient );

	//int nPathCount = m_pathExpsure.getCount();
	//procFile.writeInt(nPathCount);
	//for (int i =0;i <nPathCount;i++)
	//{
	//	procFile.writePoint(m_pathExpsure.getPoint(i));
	//}

	return 1;
}

bool BillboardProc::GetIntersectionWithLine(const Point& startPoint,const Point& endPoint,Point& StopPoint,Point& directionPoint,double& distanceToBillboard, double& dAngel)
{
//	ASSERT(m_pTotalPath.getCount() > 3);
	if(m_pTotalPath.getCount() <= 3)
		return false;

	Path path = m_pTotalPath;
    //calc that wether has intersection
	Line testLine (startPoint, endPoint);
	Line polySegment (path[0], path[path.getCount()-1]);
	
	Point itersecPointList[MAX_PATH];
	int nPointFound = 0;
	int numPoints = path.getCount();
	for (short i = 0; i < numPoints - 1; i++)
	{
		polySegment.init (path[i], path[i+1]);
		Point tempPoint;
		if (testLine.intersects (polySegment,tempPoint))
		{
			itersecPointList[nPointFound] = tempPoint;
			nPointFound = nPointFound + 1;
		}
	}
	if (nPointFound == 0) //no intersection
		return false;
	
	//clac the point and angel
	Path* pSeviceLoactionPath = serviceLocationPath();
	Point servicePoint1 = pSeviceLoactionPath->getPoint(0);
	Point servicePoint2 = pSeviceLoactionPath->getPoint(1);
	//get billboard middle point
	Point BillboardMiddlepoint((servicePoint1.getX()+servicePoint2.getX())/2,
		(servicePoint1.getY()+servicePoint2.getY())/2,servicePoint1.getZ());
	
	ARCVector2 startVec2(startPoint);
	ARCVector2 endVec2(endPoint);
	ARCVector2 moveline(startVec2,endVec2);//(ARCVector2(startPoint),ARCVector2(endPoint));
	
	//random value ,adopt gamma random algorithm,the value returned range 1 to 6
	double dRandomvalue = m_gammaDistribution.getRandomValue()/6.0;
//	int nRandomValue = static_cast<int>((dRandomvalue/6.0)*100);

	if (nPointFound == 1) //has a intersection point
	{
		//ARCVector2 startVec2(startPoint);
		//ARCVector2 endVec2(endPoint);
		//ARCVector2 moveline(startVec2,endVec2);//(ARCVector2(startPoint),ARCVector2(endPoint));
		//clac the stop position
		StopPoint.setX((endPoint.getX() - itersecPointList[0].getX()) *dRandomvalue + itersecPointList[0].getX());
		StopPoint.setY((endPoint.getY() - itersecPointList[0].getY()) *dRandomvalue + itersecPointList[0].getY());
		StopPoint.setZ(endPoint.getZ());

		ARCVector2 BillboardMiddleVec2(BillboardMiddlepoint);
		ARCVector2 billboardLine(ARCVector2(StopPoint),BillboardMiddleVec2);//(ARCVector2(endPoint),ARCVector2(BillboardMiddlepoint));
	
		dAngel = moveline.GetAngleOfTwoVector(billboardLine);
		distanceToBillboard = endPoint.distance(BillboardMiddlepoint);

		//clac the direction point
		directionPoint = StopPoint.GetDistancePoint(BillboardMiddlepoint,50.0);
	}
	else
	{	
			{
				
				//clac intersection point line's direction
				ARCVector2 itersecVec1(itersecPointList[0]);
				ARCVector2 itersecVec2(itersecPointList[1]);
				
				if(itersecVec1.DistanceTo(startVec2) > itersecVec2.DistanceTo(startVec2))
				{
					Point tempPoint = itersecPointList[0];
					itersecPointList[0] = itersecPointList[1];
					itersecPointList[1] = tempPoint;				
				}

				//get stop position

				StopPoint.setX((itersecPointList[1].getX() - itersecPointList[0].getX()) *dRandomvalue + itersecPointList[0].getX());
				StopPoint.setY((itersecPointList[1].getY() - itersecPointList[0].getY()) *dRandomvalue + itersecPointList[0].getY());
				StopPoint.setZ(itersecPointList[1].getZ());
				
				//clac the direction point
				directionPoint = StopPoint.GetDistancePoint(BillboardMiddlepoint,50.0);
//				directionPoint = BillboardMiddlepoint;
			}

//			Point intersecMiddlePoint((itersecPointList[0].getX()+itersecPointList[1].getX())/2,
//				(itersecPointList[0].getY()+itersecPointList[1].getY())/2,itersecPointList[1].getZ());
			//ARCVector2 startVec2(startPoint);
			//ARCVector2 endVec2(endPoint);
			//ARCVector2 moveline(startVec2,endVec2);//(ARCVector2(startPoint),ARCVector2(endPoint));

//			ARCVector2 moveline(ARCVector2(startPoint),ARCVector2(endPoint));
//			ARCVector2 billboardLine(ARCVector2(intersecMiddlePoint),ARCVector2(BillboardMiddlepoint));

			ARCVector2 intersecMiddleVec2(StopPoint);
			ARCVector2	billboardMiddleVec2(BillboardMiddlepoint);
			
			ARCVector2 billboardLine;
			billboardLine.reset( ARCVector2(StopPoint), ARCVector2(BillboardMiddlepoint) );		
			dAngel = moveline.GetAngleOfTwoVector(billboardLine);


			distanceToBillboard = StopPoint.distance(BillboardMiddlepoint);
	}

		
	return TRUE;
}

bool BillboardProc::InitWholeExposurePath()//Path& path)
{
	Path* pPath = GetExpsurePath();
	int nPathCount = pPath->getCount();
	if (nPathCount < 2)//must have three point
		return false;

	//clac the start and end point of the exposure path
	Point firstPoint = pPath->getPoint(0);
	Point lastPoint = pPath->getPoint(nPathCount - 1);

	Path* pSeviceLoactionPath = serviceLocationPath();
	int nSevicePathCount = pSeviceLoactionPath->getCount();
	if (nSevicePathCount != 2)
		return false;

	Point servicePoint1 = pSeviceLoactionPath->getPoint(0);
	Point servicePoint2 = pSeviceLoactionPath->getPoint(1);

	Point pointList[MAX_PATH];
	if (servicePoint1.distance(firstPoint)< servicePoint1.distance(lastPoint))
	{
		pointList[0].SetPoint(servicePoint1);
		pointList[nPathCount+1].SetPoint(servicePoint2);
	}
	else
	{
		pointList[0].SetPoint(servicePoint2);
		pointList[nPathCount+1].SetPoint(servicePoint1);
	}

	for (int i =0; i <nPathCount; i++)
	{
		pointList[i+1].SetPoint(pPath->getPoint(i));
	}
	m_pTotalPath.clear();
	m_pTotalPath.init(nPathCount+2,pointList);

	return true;
}

void BillboardProc::UpdateMinMax()
{
	Processor::UpdateMinMax();
	GetMinMax(m_min, m_max, m_pTotalPath);
}
//bool ClacIntersectionWholeExposureAreaWithLine()

