#pragma once
#include "altobject.h"

#include "..\Common\path2008.h"
#include "HoldPosition.h"
#include "StandLeadInLine.h"
#include "StandLeadOutLine.h"

#define STAND_ID_ALL			(-2)

class ARCVector3;
class Taxiway;

class INPUTAIRSIDE_API Stand :
	public ALTObject
{
friend class CStand3D;

public:
	typedef ref_ptr<Stand> RefPtr;
public:
	static const GUID& getTypeGUID();


	Stand(void);
	virtual ~Stand(void);

	virtual ALTOBJECT_TYPE GetType()const { return ALT_STAND; } ;
	virtual CString GetTypeName()const  { return "Stand"; };


	const CPoint2008& GetServicePoint()const;
	//const Path& GetInContrain()const;
	//const Path& GetOutConstrain()const;

	void SetServicePoint(const CPoint2008& _pt);
	//void SetInContrain(const Path& _path);
	//void SetOutConstrain(const Path& _path);

	/*bool IsUsePushBack()const;
	bool UsePushBack(bool _b);*/
	// get a copy of this object , change its name in sequence
	virtual ALTObject * NewCopy();
	virtual ALTObjectDisplayProp* NewDisplayProp();

	void OnRotate(DistanceUnit dx, const ARCVector3& vCenter);
	void DoOffset(DistanceUnit dx, DistanceUnit dy, DistanceUnit dz);

	//Path GetLeadLine( LeadLineType ltype, int nidx ) const;

	StandLeadInLineList& GetLeadInLineList();
	StandLeadOutLineList& GetLeadOutLineList();
	//Lead In/Out Line
	StandLeadInLineList m_vLeadInLines;
	StandLeadOutLineList m_vLeadOutLines;

	BOOL IsAllLinesValid() const;

	virtual bool CopyData(const ALTObject* pObj);

private:
	CPoint2008 m_servicePoint;
	//Path m_inContrain;
	//Path m_outConstrain;
	//bool m_bUsePushback;


	//database operation
public:

	void ReadObject(int nObjID);
	int SaveObject(int nAirportID);
	virtual void UpdateObject(int nObjID);
	virtual void DeleteObject(int nObjID);

	void UpdateServicePoint();
	//void UpdateInConstraint();
	//void UpdateOutConstraint();

	static void ExportStands(int nAirportID,CAirsideExportFile& exportFile);
	void ExportStand(CAirsideExportFile& exportFile);

	void ImportObject(CAirsideImportFile& importFile);

	IntersectionNodeList GetIntersectNodes()const; 	
	

protected:

	//int m_nInconsID;
	//int m_nOutconsID;

	CString GetSelectScript(int nObjID) const;
	CString GetInsertScript(int nObjID) const;
	virtual CString GetUpdateScript(int nObjID) const;
	virtual CString GetDeleteScript(int nObjID) const;

};
