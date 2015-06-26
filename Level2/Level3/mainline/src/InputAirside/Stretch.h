#pragma once
#include "altobject.h"
#include "../Common/path2008.h"
#include <vector>
#include "../Database/DBElement.h"
class Intersections;
class VehiclePoolParking;
class IntersectionNode;

class INPUTAIRSIDE_API Stretch:public ALTObject
{

friend class CStretch3D;

public:
	typedef ref_ptr<Stretch> RefPtr;
	static const GUID& getTypeGUID();


	enum {
		PositiveDirection =0,
		NegativeDirection,
	};
	
	Stretch(void);
	Stretch(const Stretch&);
	Stretch& operator =(const Stretch&);

	~Stretch(void);
protected:
	CPath2008 m_path;
	double m_dLaneWidth;
	int m_iLaneNum;

	int m_nPathID;
public:
	const CPath2008& GetPath(void)const;
	void SetPath(const CPath2008& _path);
    CPath2008& getPath();

	double GetLaneWidth(void)const;
	void SetLaneWidth(double dLaneWidth);

	int GetLaneNumber(void)const;
	void SetLaneNumber(int iLaneNum);

	//get lane direction
	int GetLaneDir(int idx)const;
	//is the lane stretch to the intersection
	bool IsAInLaneToIntersection(int laneNo, const Intersections& intersection)const;
	// is the lane stretch out from the intersection
	bool IsAOutLaneFromIntersection(int laneNo, const Intersections& intersection)const;
	////
	//bool IsAInLaneToVehiclePool(int laneNo, const VehiclePoolParking& pool)const;
	//bool IsAOutLaneFromVehiclePool(int laneNo, const VehiclePoolParking& pool)const;
	std::vector<IntersectionNode> GetIntersectNodes() const;

	void UpdatePath();

public:
	virtual ALTOBJECT_TYPE GetType()const {return ALT_STRETCH;} 
	virtual CString GetTypeName()const { return _T("STRETCH");} 

	virtual void ReadObject(int nObjID);
	virtual int SaveObject(int nAirportID);
	virtual void UpdateObject(int nObjID);
	virtual void DeleteObject(int nObjID);

	virtual CString GetInsertScript(int nObjID) const;
	virtual CString GetUpdateScript(int nObjID) const;
	virtual CString GetDeleteScript(int nObjID) const;
	virtual CString GetSelectScript(int nObjID) const;

	static void ExportStretch(int nAirportID,CAirsideExportFile& exportFile);
	virtual void ExportObject(CAirsideExportFile& exportFile);
	virtual void ImportObject(CAirsideImportFile& importFile);

	virtual bool CopyData(const ALTObject* pObj);

	virtual ALTObject * NewCopy();
	virtual ALTObjectDisplayProp* NewDisplayProp();
};
