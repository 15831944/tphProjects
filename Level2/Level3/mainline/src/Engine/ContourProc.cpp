#include "StdAfx.h"
#include "contourproc.h"
#include "common\exeption.h"
#include "common\pollygon.h"


ContourProc::ContourProc(void)
{
	m_lHeight = 0.0;
}

ContourProc::~ContourProc(void)
{
}

int ContourProc::writeSpecialField(ArctermFile& procFile) const
{
	//save the height
	procFile.writeDouble(m_lHeight);

	return TRUE;
}

int ContourProc::readSpecialField(ArctermFile& procFile)
{
	//read 
	procFile.getFloat(m_lHeight);

	return TRUE;
}

void ContourProc::initServiceLocation(int _pathCount, const Point * _pointList)
{
	if (_pathCount < 0)
		throw new StringError ("Contour must have at least 3 point service locations");

	m_location.init (_pathCount, _pointList);

	Pollygon serviceArea;
	serviceArea.init (m_location.getCount(), m_location.getPointList());
	double m_dArea = serviceArea.calculateArea();
	if (m_dArea < 0.0)
	{
		char str[80];
		strcpy (str, "Contour m_dArea ");
		name.printID (str + strlen (str));
		strcat (str, " has no space");
		throw new StringError (str);
	}
}

Point ContourProc::GetServiceLocation () const
{
	Pollygon serviceArea;
	serviceArea.init (m_location.getCount(), m_location.getPointList());
	return serviceArea.getRandPoint();
}