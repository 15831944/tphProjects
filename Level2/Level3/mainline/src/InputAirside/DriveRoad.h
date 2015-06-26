#pragma once
#include "altobject.h"
#include "../Common/path2008.h"
#include <vector>
#include "InputAirsideAPI.h"

enum RoadDirection
{
	IN_Dir = 0,
	OUT_Dir
};

class IntersectionNode;

class INPUTAIRSIDE_API DriveRoad : public ALTObject
{
public:
	DriveRoad(int nParentID);
	~DriveRoad(void);

public:
	const CPath2008& GetPath(void)const;
	void SetPath(const CPath2008& _path);

	double GetLaneWidth(void);
	void SetLaneWidth(double dLaneWidth);

	int GetLaneNumber(void);
	void SetLaneNumber(int nLaneNum);

	//get lane direction
	RoadDirection GetRoadDir();
	void SetRoadDir(RoadDirection _direction);

	std::vector<IntersectionNode> GetIntersectNodes() const;

	void UpdatePath();

public:
	virtual ALTOBJECT_TYPE GetType()const {return ALT_DRIVEROAD;} 
	virtual CString GetTypeName()const { return _T("Drive Road");} 

	virtual void ReadObject(int nObjID);
	virtual int SaveObject(int nAirportID);
	virtual void UpdateObject(int nObjID);
	virtual void DeleteObject(int nObjID);

	virtual CString GetInsertScript(int nObjID) const;
	virtual CString GetUpdateScript(int nObjID) const;
	virtual CString GetDeleteScript(int nObjID) const;
	virtual CString GetSelectScript(int nObjID) const;

	virtual bool CopyData(const ALTObject* pObj);

	virtual ALTObject * NewCopy();

private:
	CPath2008 m_path;
	double m_dLaneWidth;
	int m_nLaneNum;
	RoadDirection m_enumDirection;
	int m_nParentID;
	int m_nPathID;

};


class INPUTAIRSIDE_API DriveRoadList
{
public:
	DriveRoadList(int nParentID);
	~DriveRoadList();

	void AddDriveRoad(DriveRoad* pRoad);
	int GetRoadCount();
	DriveRoad* GetRoad(int nIdx);
	void DelDriveRoad(DriveRoad* pRoad);

	void ReadDriveRoadList();
	void SaveDriveRoadList(int nAirportID);
	void DeleteDriveRoadList();

	void CopyData(DriveRoadList& otherRoadList);
	void ClearList();
private:
	std::vector<DriveRoad*> m_vDriveRoadList;
	std::vector<DriveRoad*> m_vDelDriveRoadList;
	int m_nParentID;
};