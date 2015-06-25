#pragma once
#include "../../Common/point2008.h"
#include "../../InputAirside/ALTObject.h"

typedef struct StaticRunwayLine
{
	CPoint2008 pt1;
	CString strName1;
	CPoint2008 pt2;
	CString strName2;
}StaticRunwayLine;
// CRunwayImageStatic

class CRunwayImageStatic : public CStatic
{
	DECLARE_DYNAMIC(CRunwayImageStatic)

public:
	CRunwayImageStatic(int nProjID);
	virtual ~CRunwayImageStatic();
	void GetRunway(int nAirportID, const ALTObjectList& vRunways);
protected:
	int m_nProjID;
	std::vector <StaticRunwayLine> m_vrRunwayLines;	

	double m_xMin;
	double m_xMax;
	double m_yMin;
	double m_yMax;
	double m_xOrg;
	double m_yOrg;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};


