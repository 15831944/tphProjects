#pragma once
#include "Stand.h"
#include "..\Common\path2008.h"
#include "..\Common\ARCVector.h"
class Taxiway;
#include "HoldPosition.h"

class INPUTAIRSIDE_API DeicePad :
	public Stand
{
	friend class CDeicePad3D;

public:
	typedef ref_ptr<DeicePad> RefPtr;

	static const GUID& getTypeGUID();
	DeicePad(void);
	virtual ~DeicePad(void);
	virtual ALTOBJECT_TYPE GetType()const { return ALT_DEICEBAY; } ;
	virtual CString GetTypeName()const  { return "DeicePad"; };


	const CPoint2008& GetServicePoint()const;
	const CPath2008& GetInContrain()const;
	const CPath2008& GetOutConstrain()const;

	void SetServicePoint(const CPoint2008& _pt);
	void SetInContrain(const CPath2008& _path);
	void SetOutConstrain(const CPath2008& _path);


	const double GetWinSpan()const;
	const double GetLength()const;
	void SetWinspan(const double & _winspan);
	void SetLength(const double & _length);

	ARCVector2 GetDir()const;
	
	bool IsUsePushBack()const;
	bool UsePushBack(bool _b);
	// get a copy of this object , change its name in sequence
	virtual ALTObject * NewCopy();	
	virtual ALTObjectDisplayProp* NewDisplayProp();
	virtual bool CopyData(const ALTObject* pObj);

	void SetAsStand(bool bAsStand);
	bool IsAsStand();



private :
	CPoint2008 m_servicePoint;
	CPath2008 m_inContrain;
	CPath2008 m_outConstrain;
	bool m_bUsePushback;
	double m_dWinSpan;
	double m_dLength;
	bool m_bAsStand;

	//database operation
public:

	void ReadObject(int nObjID);
	int SaveObject(int nAirportID);
	virtual void UpdateObject(int nObjID);
	virtual void DeleteObject(int nObjID);

	void UpdateInConstraint();
	void UpdateOutConstraint();



public:
	static void ExportDeicePads(int nAirportID,CAirsideExportFile& exportFile);
	void ExportDeicePad(CAirsideExportFile& exportFile);	
	void ImportObject(CAirsideImportFile& importFile);


protected:
	int m_nInconsID;
	int m_nOutconsID;

	CString GetSelectScript(int nObjID) const;
	CString GetInsertScript(int nObjID) const;
	virtual CString GetUpdateScript(int nObjID) const;
	virtual CString GetDeleteScript(int nObjID) const;
};
