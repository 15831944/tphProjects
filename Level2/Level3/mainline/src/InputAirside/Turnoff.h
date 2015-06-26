#pragma once
#include "altobject.h"
#include "../Common/path2008.h"
#include <vector>
#include "IntersectionsDefine.h"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API Turnoff :	public ALTObject
{
public:
	Turnoff(void);
	~Turnoff(void);
protected:
	double m_dLaneWidth;
	InsecObjectPartVector m_vrInsecObjectPart;
	CPath2008 m_path;
	int m_nPathID;
public:
	virtual ALTOBJECT_TYPE GetType()const {return ALT_TURNOFF;} 
	virtual CString GetTypeName()const { return _T("TURNOFF");} 
public:
	const InsecObjectPartVector & GetIntersectionsPart(void);
	void SetIntersectionsPart(const InsecObjectPartVector & iopvrSet);
	double GetLaneWidth(void);
	void SetLaneWidth(double dLaneWidth);
	const CPath2008& GetPath(void)const;
	void SetPath(const CPath2008& _path);

	virtual void ReadObject(int nObjID);
	virtual int SaveObject(int nAirportID);
	virtual void UpdateObject(int nObjID);
	virtual void DeleteObject(int nObjID);

	virtual CString GetInsertScript(int nObjID) const;
	virtual CString GetUpdateScript(int nObjID) const;
	virtual CString GetDeleteScript(int nObjID) const;
	virtual CString GetSelectScript(int nObjID) const;

	static void ExportTurnoff(int nAirportID,CAirsideExportFile& exportFile);
	virtual void ExportObject(CAirsideExportFile& exportFile);
	virtual void ImportObject(CAirsideImportFile& importFile);
};
