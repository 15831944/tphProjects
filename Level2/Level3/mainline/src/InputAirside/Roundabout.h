#pragma once
#include "altobject.h"
#include "../Common/path.h"
#include <vector>
#include "IntersectionsDefine.h"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API Roundabout :	public ALTObject
{
public:
	Roundabout(void);
	~Roundabout(void);
protected:
	double m_dLaneWidth;
	int m_iLaneNum;
	InsecObjectPartVector m_vrInsecObjectPart;
public:
	virtual ALTOBJECT_TYPE GetType()const {return ALT_ROUNDABOUT;} 
	virtual CString GetTypeName()const { return _T("ROUNDABOUT");} 
public:
	double GetLaneWidth(void);
	void SetLaneWidth(double dLaneWidth);
	int GetLaneNumber(void);
	void SetLaneNumber(int iLaneNum);
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

	static void ExportRoundabout(int nAirportID,CAirsideExportFile& exportFile);
	virtual void ExportObject(CAirsideExportFile& exportFile);
	virtual void ImportObject(CAirsideImportFile& importFile);
};
