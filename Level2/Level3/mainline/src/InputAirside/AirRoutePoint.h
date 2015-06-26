#pragma once
#include "InputAirsideAPI.h"
#include "AirWayPoint.h"

class ARPointBase
{
protected:
    ARPointBase();
    virtual ~ARPointBase();
};

class INPUTAIRSIDE_API ARWaypoint
{
public:
    enum DepartType{ NextWaypoint = 0, Heading };
    enum HeadingType{ Aligned = 0, Direct ,None };
public:

    ARWaypoint(int nID = -1);
    ~ARWaypoint();

    ARWaypoint(const ARWaypoint&);
protected:
    //AirWayPoint m_pWaypoint;
    int m_nID;

    AirWayPoint m_wayPoint;

    double m_lMinSpeed;
    double m_lMaxSpeed;
    double m_lMinHeight;
    double m_lMaxHeight;

    DepartType m_DepartType;
    HeadingType m_HeadingType;
    long m_lDegree;
    double m_lVisDistance;
    CPoint2008 m_ExtentPoint;
    int m_nSequenceNum;


public:
    // get a copy of this object , change its name in sequence
    //virtual ALTObject * NewCopy();

    int getID()const;
    void setID(int nID);

    void setMinSpeed(double lSpeed);
    void setMaxSpeed(double lSpeed);

    void SetSequenceNum(int nSequenceNum);
    int GetSequenceNum(void);

    double getMinSpeed() const;
    double getMaxSpeed() const;

    void setMinHeight(double lMinHeight);
    void setMaxHeight(double lMaxHeight);

    double getMinHeight() const;
    double getMaxHeight() const;

    void setWaypoint(const AirWayPoint& altobj){ m_wayPoint = altobj;}
    AirWayPoint& getWaypoint();
    const AirWayPoint& getWaypoint() const;

    void setDepartType(DepartType type);
    void setHeadingType(HeadingType type);
    void setDegrees(long lDegrees);
    void setVisDistance(double lDistance);
    void setExtentPoint( CPoint2008 extentPoint);

    DepartType getDepartType();
    HeadingType getHeadingType();
    long getDegrees();
    double getVisDistance();
    CPoint2008 getExtentPoint() const;


    double getDefaultAltitude();


    ARWaypoint&  operator =(const ARWaypoint& );

    //database operation
public:
    void UpdateData();
    void DeleteData();
    void SaveData(int nAirRouteID);

public:
    void ImportObject(CAirsideImportFile& importFile,int nAirRouteID);
    void ExportObject(CAirsideExportFile& exportFile);

};