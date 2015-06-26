#include "stdafx.h"
#include "AirRoutePoint.h"

//////////////////////////////////////////////////////////////////////////
//class ARWaypoint
ARPointBase::ARPointBase()
{

}

ARPointBase::~ARPointBase()
{

}

//////////////////////////////////////////////////////////////////////////
//class ARWaypoint
ARWaypoint::ARWaypoint(int nID)
{
    m_nID = nID;
    m_lMinSpeed		= -1;
    m_lMaxSpeed		= -1;
    m_lMinHeight	= -1;
    m_lMaxHeight	= -1;	
    m_DepartType = NextWaypoint;
    m_lDegree = 0;
    m_HeadingType = None;
    m_lVisDistance = 0;
    m_nSequenceNum = 0;
}
ARWaypoint::ARWaypoint( const ARWaypoint& leftWayPt)
{
    m_nID = leftWayPt.m_nID;
    m_wayPoint = leftWayPt.m_wayPoint;

    m_lMinSpeed = leftWayPt.m_lMinSpeed;
    m_lMaxSpeed = leftWayPt.m_lMaxSpeed;
    m_lMinHeight = leftWayPt.m_lMinHeight;
    m_lMaxHeight = leftWayPt.m_lMaxHeight;
    m_DepartType = leftWayPt.m_DepartType;
    m_lDegree = leftWayPt.m_lDegree;
    m_HeadingType = leftWayPt.m_HeadingType;
    m_lVisDistance = leftWayPt.m_lVisDistance;
    m_ExtentPoint = leftWayPt.m_ExtentPoint;
    m_nSequenceNum = leftWayPt.m_nSequenceNum;
}
ARWaypoint::~ARWaypoint()
{

}

void ARWaypoint::SetSequenceNum(int nSequenceNum)
{
    m_nSequenceNum = nSequenceNum;
}

int ARWaypoint::GetSequenceNum(void)
{
    return m_nSequenceNum;
}

void ARWaypoint::setMinSpeed(double lSpeed)
{
    m_lMinSpeed = lSpeed;
}
void ARWaypoint::setMaxSpeed(double lSpeed)
{ 
    m_lMaxSpeed = lSpeed;
}
double ARWaypoint::getMinSpeed() const
{ 
    return m_lMinSpeed;
}
double ARWaypoint::getMaxSpeed() const
{ 
    return m_lMaxSpeed;
}
void ARWaypoint::setMinHeight(double lMinHeight)
{ 
    m_lMinHeight = lMinHeight;
}
void ARWaypoint::setMaxHeight(double lMaxHeight)
{
    m_lMaxHeight = lMaxHeight;
}
double ARWaypoint::getMinHeight() const
{ 
    return m_lMinHeight;
}
double ARWaypoint::getMaxHeight() const
{ 
    return m_lMaxHeight;
}

void ARWaypoint::setDepartType(DepartType type)
{
    m_DepartType = type;
}

void ARWaypoint::setHeadingType(HeadingType type)
{
    m_HeadingType = type;
}

void ARWaypoint::setDegrees(long lDegrees)
{
    m_lDegree = lDegrees;
}

void ARWaypoint::setVisDistance(double lDistance)
{
    m_lVisDistance = lDistance ;	 
}

ARWaypoint::DepartType ARWaypoint::getDepartType()
{
    return m_DepartType;
}

ARWaypoint::HeadingType ARWaypoint::getHeadingType()
{
    return m_HeadingType;
}

long ARWaypoint::getDegrees()
{
    return m_lDegree;
}

double ARWaypoint::getVisDistance()
{
    return  m_lVisDistance;
}


void ARWaypoint::UpdateData()
{
    CString strSQL = _T("");
    strSQL.Format(_T("UPDATE ARWAYPOINTPROP\
                     SET WAYPOINTID =%d, MinSpeed =%f, MaxSpeed =%f, MinHeight =%f, MaxHeight =%f,\
                     DepartFor =%d,HeadingDegree =%d, Headingtype = %d, VisDistance = %f, SEQUENCENUM = %d\
                     WHERE ID = %d"),
                     m_wayPoint.getID(),m_lMinSpeed,m_lMaxSpeed,m_lMinHeight,m_lMaxHeight,m_DepartType,m_lDegree,m_HeadingType, m_lVisDistance,m_nSequenceNum,
                     m_nID);
    CADODatabase::ExecuteSQLStatement(strSQL);
}

void ARWaypoint::DeleteData()
{
    CString strSQL = _T("");
    strSQL.Format(_T("DELETE FROM ARWAYPOINTPROP WHERE ID = %d"),m_nID);
    CADODatabase::ExecuteSQLStatement(strSQL);
}
void ARWaypoint::SaveData(int nAirRouteID)
{
    CString strSQL = _T("");
    strSQL.Format(_T("INSERT INTO ARWAYPOINTPROP\
                     (WAYPOINTID, AirRouteID, MinSpeed, MaxSpeed, MinHeight, MaxHeight,DepartFor,HeadingDegree, Headingtype, VisDistance, SEQUENCENUM)\
                     VALUES (%d,%d,%f,%f,%f,%f,%d,%d,%d,%f,%d)"),
                     m_wayPoint.getID(),nAirRouteID,m_lMinSpeed,m_lMaxSpeed,m_lMinHeight,m_lMaxHeight,int(m_DepartType),m_lDegree,int(m_HeadingType),m_lVisDistance,m_nSequenceNum);

    m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}

ARWaypoint& ARWaypoint::operator=( const ARWaypoint& leftWayPt )
{
    m_nID = leftWayPt.m_nID;
    m_wayPoint = leftWayPt.m_wayPoint;

    m_lMinSpeed = leftWayPt.m_lMinSpeed;
    m_lMaxSpeed = leftWayPt.m_lMaxSpeed;
    m_lMinHeight = leftWayPt.m_lMinHeight;
    m_lMaxHeight = leftWayPt.m_lMaxHeight;
    m_DepartType = leftWayPt.m_DepartType;
    m_HeadingType = leftWayPt.m_HeadingType;
    m_lDegree = leftWayPt.m_lDegree;
    m_lVisDistance = leftWayPt.m_lVisDistance;
    m_nSequenceNum = leftWayPt.m_nSequenceNum;
    m_ExtentPoint = leftWayPt.m_ExtentPoint;
    return *this;

}

int ARWaypoint::getID()const
{
    return m_nID;
}

void ARWaypoint::setID( int nID )
{
    m_nID = nID;
}

const AirWayPoint& ARWaypoint::getWaypoint() const
{
    return m_wayPoint;
}

AirWayPoint& ARWaypoint::getWaypoint()
{
    return m_wayPoint;
}
double ARWaypoint::getDefaultAltitude()
{
    double dAltitude = 0.0;
    if (m_lMinHeight == -1 && m_lMaxHeight == -1)
    {
        dAltitude =	(m_wayPoint.GetHighLimit()+m_wayPoint.GetLowLimit())/2;
    }
    else if (m_lMinHeight == -1)
    {
        dAltitude = (m_wayPoint.GetLowLimit() + m_lMaxHeight)/2;
    }
    else if (m_lMaxHeight == -1)
    {
        dAltitude = (m_wayPoint.GetHighLimit() + m_lMinHeight)/2;
    }
    else
    {
        dAltitude = (m_lMinHeight + m_lMaxHeight)/2;
    }

    return dAltitude;

}
//int m_nID;

//ALTObject m_wayPoint;

//long m_lMinSpeed;
//long m_lMaxSpeed;
//long m_lMinHeight;
//long m_lMaxHeight;
void ARWaypoint::ExportObject(CAirsideExportFile& exportFile)
{
    // 	exportFile.getFile().writeInt(m_nID);
    // 	exportFile.getFile().writeInt(m_wayPoint.getID());
    // 	exportFile.getFile().writeInt((int)m_lMinSpeed);
    // 	exportFile.getFile().writeInt((int)m_lMaxSpeed);
    // 	exportFile.getFile().writeInt((int)m_lMinHeight);
    // 	exportFile.getFile().writeInt((int)m_lMaxHeight);
    // 	exportFile.getFile().writeInt(int(m_DepartType));
    // 	exportFile.getFile().writeInt(m_lDegree);
    // 	exportFile.getFile().writeInt(int(m_HeadingType));
    // 	exportFile.getFile().writeInt((int)m_lVisDistance);
    // 	exportFile.getFile().writeInt(m_nSequenceNum);
    // 
    // 	exportFile.getFile().writeLine();

}

void ARWaypoint::ImportObject(CAirsideImportFile& importFile,int nAirRouteID)
{
    // 	int nOldID = -1;
    // 	importFile.getFile().getInteger(nOldID);
    // 	int nOldWaypointID = -1;
    // 	importFile.getFile().getInteger(nOldWaypointID);
    // 
    // 	int nNewWaypointID = importFile.GetObjectNewID(nOldWaypointID);
    // 	m_wayPoint.setID(nNewWaypointID);
    // 
    // 	importFile.getFile().getInteger((short&)m_lMinSpeed);
    // 	importFile.getFile().getInteger((short&)m_lMaxSpeed);
    // 	importFile.getFile().getInteger((short&)m_lMinHeight);
    // 	importFile.getFile().getInteger((short&)m_lMaxHeight);
    // 
    // 	int nDepartType = 0;
    // 	importFile.getFile().getInteger(nDepartType);
    // 	m_DepartType = (DepartType)nDepartType;
    // 
    // 	importFile.getFile().getInteger(m_lDegree);
    // 
    // 	int nHeadingType = 0;
    // 	importFile.getFile().getInteger(nHeadingType);
    // 	m_HeadingType = (HeadingType)nHeadingType;
    // 
    // 	importFile.getFile().getInteger((short&)m_lVisDistance);
    // 
    // 	if (importFile.getVersion() > 1026)
    // 	{
    // 		importFile.getFile().getInteger(m_nSequenceNum);
    // 	}
    // 
    // 	importFile.getFile().getLine();
    // 
    // 	SaveData(nAirRouteID);
    // 
    // 	importFile.AddAirRouteARWaypointIndexMap(nOldID,m_nID);

}

void ARWaypoint::setExtentPoint(CPoint2008 extentPoint)
{
    m_ExtentPoint = extentPoint;
}
CPoint2008 ARWaypoint::getExtentPoint() const
{
    return m_ExtentPoint;
}
