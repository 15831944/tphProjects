#pragma once
#include "altobject.h"
#include "../Common/path2008.h"
#include <vector>
#include "..\Database\ADODatabase.h"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API StopSign:public ALTObject
{
public:
	StopSign(void);
	~StopSign(void);
	static const GUID& getTypeGUID();

protected:
	CPath2008 m_path;
	int m_nPathID;
	double m_dRotation;
	int m_nIntersection;
	int m_nStretchID;
public:
	const CPath2008& GetPath(void)const;
	void SetPath(const CPath2008& _path);

	const int GetIntersectionNodeID(void)const;
	void SetIntersectionNodeID(const int nIntersection);

	const int GetStretchID(void)const;
	void SetStretchID(const int nStretchID);

	double GetRotation(void)const;
	void SetRotation(double dRotation);
    virtual bool CopyData(const ALTObject* pObj);

public:
	virtual ALTOBJECT_TYPE GetType()const {return ALT_STOPSIGN;} 
	virtual CString GetTypeName()const { return _T("STOPSIGN");} 

	virtual void ReadObject(int nObjID);
	virtual int SaveObject(int nAirportID);
	virtual void UpdateObject(int nObjID);
	virtual void DeleteObject(int nObjID);

	virtual CString GetInsertScript(int nObjID) const;
	virtual CString GetUpdateScript(int nObjID) const;
	virtual CString GetDeleteScript(int nObjID) const;
	virtual CString GetSelectScript(int nObjID) const;

	static void ExportStopSign(int nAirportID,CAirsideExportFile& exportFile);
	virtual void ExportObject(CAirsideExportFile& exportFile);
	virtual void ImportObject(CAirsideImportFile& importFile);

	virtual ALTObject * NewCopy();
	virtual ALTObjectDisplayProp* NewDisplayProp();

};
