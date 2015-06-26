#pragma once
#include "altobject.h"

#include "../Common/Point2008.h"
#include "../Common/DistanceLineLine.h"

#include "Taxiway.h"

class INPUTAIRSIDE_API CStartPosition :
	public ALTObject
{
	friend class ALTObjectInput;

public:
	typedef ref_ptr<CStartPosition> RefPtr;
	static const GUID& getTypeGUID();
public:
	CStartPosition(void);
	virtual ~CStartPosition(void);

	explicit CStartPosition(const CStartPosition&);

	// following are some polymorphic methods that derived from ALTObject
	virtual ALTOBJECT_TYPE GetType(void) const { return ALT_STARTPOSITION;}
	virtual CString GetTypeName() const {return _T("StartPositions");}

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



	// let's define CStartPosition' own methods
	void SetTaxiwayID(int taxiwayID);
	int  GetTaxiwayID() const;
	void SetTaxiwayDivision(DistanceUnit fDivision);
	DistanceUnit GetTaxiwayDivision();
	void SetRadius(DistanceUnit radius);
	DistanceUnit GetRadius();

	bool GetServicePoint(CPoint2008& pt);

	void SetMarking(const std::string& marking);
	const std::string& GetMarking() const;

	//calculate marking pos and rotate
	void getMarkingPos(const CPath2008& path, ARCPoint3& pos, double& dAngle, double& dScale);
private:
	int m_nTaxiwayID;
	DistanceUnit m_fTaxiwayDivision;
	DistanceUnit m_fRadius;
	std::string m_strMarking;

public:
	// load data from SQL-result to member variables
	void ReadRecord(CADORecordset& adoRecordset);
};
