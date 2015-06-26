#pragma once
#include "../Database/dbelement.h"
#include "IntersectionNode.h"
#include "Common/ARCPath.h"
#include "InputAirsideAPI.h"

/*****************************************/
/*   point of intersected stretch point  */
/*****************************************/
class IntersectionNodesInAirport;
class CPath2008;

class INPUTAIRSIDE_API IntersectedStretchPoint
{
	friend class IntersectedStretch;
public:
	IntersectedStretchPoint(void);
	IntersectedStretchPoint(int nItemID, DistanceUnit distToIntersect);
	//set/get distance to intersected node
	DistanceUnit GetDistToIntersect()const;
	void SetDistToIntersect(DistanceUnit disToIntersect);

	//set/get id of intersected node
	int GetIntersectItemID()const;
	void SetIntersectItemID(int nItemID);

	DistanceUnit GetDistToUser()const;
	void SetDisToUser(const DistanceUnit disToUser);

	bool IsAtSameSide(const IntersectedStretchPoint& otherStretch)const;

protected:
	int m_nIntersectItemID;
	DistanceUnit m_dDisToUser;
	DistanceUnit m_dDistToIntersected;
};

/************************************************************************/
/*                 IntersectedStretch                                   */
/************************************************************************/

class INPUTAIRSIDE_API IntersectedStretch /*:public DBElement*/
{
public:
	IntersectedStretch(void);
	IntersectedStretch(const IntersectedStretch&);
	IntersectedStretch& operator =(const IntersectedStretch& );
	explicit IntersectedStretch(const IntersectionNode& node);
	~IntersectedStretch(void);

public:
	//set 
    void SetFristIntersectedStretchPoint(const IntersectedStretchPoint& point);
    void SetSecondIntersectedStretchPoint(const IntersectedStretchPoint& point);
	void SetIntersectionNode(const IntersectionNode& node);

    //get
	IntersectedStretchPoint& GetFristIntersectedStretchPoint();
	IntersectedStretchPoint& GetSecondIntersectedStretchPoint();
	IntersectionNode& GetIntersectionNode();
 
	//get stretch path
	CPath2008 GetStretchPathOfFirstPoint();
	CPath2008 GetStretchPathOfSecondPoint();

	// get position of points 
	CPoint2008 GetFristIntersectedStretchPointPos();
	CPoint2008 GetSecondIntersectedStretchPointPos();

	ARCVector3 GetFristStretchDir();
	ARCVector3 GetSecondStretchDir();

	// get distance of point in path
	DistanceUnit GetFristPointDisInPath();
	DistanceUnit GetSecondPointDisInPath();

	// get extent distance of point 
	CPoint2008 GetExtentFristPoint();
	CPoint2008 GetExtentSecondPoint();

	// get width of two intersectItem
	DistanceUnit GetFristWidth();
	DistanceUnit GetSecondWidth();
	DistanceUnit GetMinWidth();

	int GetObject1ID();

	int GetObject2ID();

	// get point to control stretch path
	BOOL GetControlPoint(std::vector<CPoint2008>& vPoint);

	// generate the smooth stretch path
	ARCPath3 GenSmoothStretchPath();

    bool IsOverLapWith( const IntersectedStretch& otherWay ) const;

	void SetID(int id);
	int GetID()const;

public:
	virtual void GetSelectSQL(int nID,CString& strSQL)const;
	virtual void GetInsertSQL(int nParentID,CString& strSQL)const;
	virtual void GetUpdateSQL(CString& strSQL)const;
	virtual void GetDeleteSQL(CString& strSQL)const;
	void ReadData( CADORecordset& recordset,const IntersectionNodesInAirport& vnodes );
    void InitFromDBRecordset( CADORecordset& recordset );
public:
	virtual void ExportData(CAirsideExportFile& exportFile);
	virtual void ImportData(CAirsideImportFile& importFile);
	void SaveDataToDB();
    static void ReadAirportStretchs(int nAirportID ,	const IntersectionNodesInAirport& vnodes,  std::vector<IntersectedStretch>& vStretches);

	void DeleteData();
	void SaveData(int nParentID);

protected:
	int m_nID;

	IntersectionNode m_intersectioNode;
	IntersectedStretchPoint m_fristPoint;
	IntersectedStretchPoint m_secondPoint;
	ARCPath3 m_stretchPath;
    
};

/************************************************************************/
/*                     IntersectedStretchInAirport                      */
/************************************************************************/

class INPUTAIRSIDE_API IntersectedStretchInAirport
{
public:
	IntersectedStretchInAirport(void);
	~IntersectedStretchInAirport();
	static void CalculateIntersectedStretches(const IntersectionNodesInAirport& nodelist, IntersectedStretchInAirport& newStretchInAirport);
	//void CalculateIntersectedStretchesOfNode(const IntersectionNode& node/*, IntersectedStretchInAirport& m_intersectedStretchInAirport*/);
	//void CalculateStretchOfNode(const IntersectionNode& node/*, IntersectedStretchInAirport& m_intersectedStretchInAirport*/,const DistanceUnit m_dSegma,IntersectItem& item1, IntersectItem& item2);
    void AddItem(const IntersectedStretch& intersectedStretch);
	void UpdateStretch(int nARPID,const IntersectionNodesInAirport& vNodes, const std::vector<int>& vChangeNodeIndexs);
	int GetCount()const;
    IntersectedStretch& ItemAt(int index);
	const IntersectedStretch& ItemAt(int index)const;
	int FindOverLapStretch(const IntersectedStretch& stretch);
public:
	void SaveData(int nAirportID);
	void ReadData(int nAirportID);
	void ReadData( int nARPID, const IntersectionNodesInAirport& vnodes );
	
	static void ExportData(CAirsideExportFile& exportFile);
	static void ImportData(CAirsideImportFile& importFile);
protected:
	std::vector<IntersectedStretch> m_vIntersectedStretches;
};



