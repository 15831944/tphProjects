#pragma once
#include "altobject.h"
#include "../Common/path.h"
#include "../Common/Path2008.h"
#include <vector>
#include "IntersectionsDefine.h"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API Intersections :	public ALTObject
{
public:
	typedef ref_ptr<Intersections> RefPtr;
	static const GUID& getTypeGUID();
	Intersections(void);
	Intersections(const Intersections&);
	Intersections& operator = (const Intersections&);
	
	~Intersections(void);
protected:
	InsecObjectPartVector m_vrInsecObjectPart;
public:
	const InsecObjectPartVector & GetIntersectionsPart(void)const;
	InsecObjectPartVector & GetIntersectionsPart(void);
	void SetIntersectionsPart(const InsecObjectPartVector & iopvrSet);

	CPoint2008 GetPosition();

public:
	virtual ALTOBJECT_TYPE GetType()const {return ALT_INTERSECTIONS;} 
	virtual CString GetTypeName()const { return _T("INTERSECTIONS");} 

	virtual void ReadObject(int nObjID);
	virtual int SaveObject(int nAirportID);
	virtual void UpdateObject(int nObjID);
	virtual void DeleteObject(int nObjID);

	virtual CString GetInsertScript(int nObjID) const;
	virtual CString GetUpdateScript(int nObjID) const;
	virtual CString GetDeleteScript(int nObjID) const;
	virtual CString GetSelectScript(int nObjID) const;
	bool HasObject(int nObjID)const;

	static void ExportIntersections(int nAirportID,CAirsideExportFile& exportFile);
	virtual void ExportObject(CAirsideExportFile& exportFile);
	virtual void ImportObject(CAirsideImportFile& importFile);
};