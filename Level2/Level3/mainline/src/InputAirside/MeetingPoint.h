#pragma once
#include "altobject.h"

#include "../Common/Point2008.h"
#include "../Common/DistanceLineLine.h"

#include "Taxiway.h"


class INPUTAIRSIDE_API CMeetingPoint :
	public ALTObject
{
	friend class ALTObjectInput;

public:
	typedef ref_ptr<CMeetingPoint> RefPtr;


	static const GUID& getTypeGUID();
public:
	CMeetingPoint(void);
	virtual ~CMeetingPoint(void);

	explicit CMeetingPoint(const CMeetingPoint&);

	// following are some polymorphic methods that derived from ALTObject
	virtual ALTOBJECT_TYPE GetType(void) const { return ALT_MEETINGPOINT;}
	virtual CString GetTypeName() const {return _T("MeetingPoint");}

	virtual ALTObject* NewCopy(void);
	virtual bool CopyData(const ALTObject* pObj);

public:
	// DB
	void ReadObject(int nObjID);
	int SaveObject(int nAirportID);
	virtual void UpdateObject(int nObjID);
	virtual void DeleteObject(int nObjID);

	// SQL
	CString GetSelectScript(int nObjID) const;
	//CString GetSelectScriptForPath2008(int nObjID) const;
	CString GetInsertScript(int nObjID) const;
	virtual CString GetUpdateScript(int nObjID) const;
	virtual CString GetDeleteScript(int nObjID) const;



	// let's define CMeetingPoint' own methods
	void SetTaxiwayID(int taxiwayID);
	int  GetTaxiwayID() const;
	void SetTaxiwayDivision(DistanceUnit fDivision);
	DistanceUnit GetTaxiwayDivision();
	void SetRadius(DistanceUnit radius);
	DistanceUnit GetRadius();

	bool GetServicePoint(CPoint2008& pt);

	void SetMarking(const std::string& marking);
	void SetPath(const CPath2008& _path );
	const std::string& GetMarking() const;

	//calculate marking pos and rotate
	void getMarkingPos(const CPath2008& path, ARCPoint3& pos, double& dAngle, double& dScale);

	CPath2008* GetPath(); 
private:
	int m_nTaxiwayID;
	DistanceUnit m_fTaxiwayDivision;
	DistanceUnit m_fRadius;
	std::string m_strMarking;
	CPath2008 m_Path ;
	int m_PathID ;
public:
	// load data from SQL-result to member variables
	void ReadObject(CADORecordset& adoRecordset);
};
