#pragma once
//#include "InputAirsideDll.h"
#include "ALTObject.h"
#include "Common\Point2008.h"
#include "Common\Path2008.h"
#include "InputAirsideAPI.h"



class INPUTAIRSIDE_API AirsideParkSpot: public ALTObject
{
public:
	AirsideParkSpot();
	const CPoint2008& GetServicePoint()const{ return m_servicePoint; }
	const CPath2008& getLeadInLine()const{ return m_inContrain; }
	const CPath2008& getLeadOutLine()const{ return m_outConstrain; }
	BOOL IsPushBack()const{ return m_bPushBack; }

	void SetServicePoint(const CPoint2008& pos ){ m_servicePoint = pos; }
	void SetLeadInLine(const CPath2008& line){ m_inContrain = line; }
	void SetLeadOutLine(const CPath2008& line){ m_outConstrain = line; }
	void SetPushBack(BOOL b){ m_bPushBack = b; }

	void OnRotate(double  degree, const ARCVector3& vCenter);
	void DoOffset(DistanceUnit dx, DistanceUnit dy, DistanceUnit dz);
	void Rotate(double degree);

	virtual bool IsAirportElement()const{ return true; }
	virtual ALTObjectDisplayProp* NewDisplayProp();

	void ReadObject(int nObjID);
	int SaveObject(int nAirportID);
	virtual void UpdateObject(int nObjID);
	virtual void DeleteObject(int nObjID);

	virtual CString getPropTable()const=0;
	virtual CString getViewTable()const=0;


	CString GetSelectScript(int nObjID) const;

	CString GetInsertScript(int nObjID) const;

	CString GetUpdateScript(int nObjID) const;
	CString GetDeleteScript(int nObjID) const;

	void  SaveInOutConstrain();


	bool CopyData( const ALTObject* pObj );

	void InitFromDBRecordset(CADORecordset& recordset);


protected:
	CPoint2008 m_servicePoint;
	CPath2008 m_inContrain;
	CPath2008 m_outConstrain;
	BOOL m_bPushBack;

	int m_nInConstrainId;
	int m_nOutConstrainId;
};



class INPUTAIRSIDE_API AirsidePaxBusParkSpot : public AirsideParkSpot
{
public:
	typedef ref_ptr<AirsidePaxBusParkSpot> RefPtr;

	virtual CString GetTypeName()const;
	virtual ALTOBJECT_TYPE GetType()const;
	virtual ALTObject * NewCopy();

	virtual CString getPropTable()const;
	virtual CString getViewTable()const;
};



