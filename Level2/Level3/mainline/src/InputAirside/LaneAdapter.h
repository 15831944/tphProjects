#pragma once
#include "altobject.h"
#include "../Common/path.h"
#include <vector>
#include "IntersectionsDefine.h"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API LaneAdapter :public ALTObject
{
public:
	LaneAdapter(void);
	~LaneAdapter(void);
public:
	virtual ALTOBJECT_TYPE GetType()const {return ALT_LANEADAPTER;} 
	virtual CString GetTypeName()const { return _T("LANEADAPTER");} 

protected:
	InsecObjectPartVector m_vrInsecObjectPart;
public:
	const InsecObjectPartVector & GetIntersectionsPart(void);
	void SetIntersectionsPart(const InsecObjectPartVector & iopvrSet);

	virtual void ReadObject(int nObjID);
	virtual int SaveObject(int nAirportID);
	virtual void UpdateObject(int nObjID);
	virtual void DeleteObject(int nObjID);

	virtual CString GetInsertScript(int nObjID) const;
	virtual CString GetUpdateScript(int nObjID) const;
	virtual CString GetDeleteScript(int nObjID) const;
	virtual CString GetSelectScript(int nObjID) const;

	static void ExportLaneAdapter(int nAirportID,CAirsideExportFile& exportFile);
	virtual void ExportObject(CAirsideExportFile& exportFile);
	virtual void ImportObject(CAirsideImportFile& importFile);
};
