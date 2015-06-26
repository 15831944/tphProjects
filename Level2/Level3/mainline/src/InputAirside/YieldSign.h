#pragma once
#include "altobject.h"
#include "../Common/path2008.h"
#include <vector>
#include "../Database/DBElement.h"

class INPUTAIRSIDE_API YieldSign:public ALTObject
{
public:
	YieldSign(void);
	~YieldSign(void);

	static const GUID& getTypeGUID();
protected:
	CPath2008 m_path;
	int m_nPathID;
	double m_dRotation;
	int m_nIntersect;
	int m_nStretchID;
public:
	const CPath2008& GetPath(void)const;
	void SetPath(const CPath2008& _path);

	const int GetIntersectionNodeID(void)const;
	void SetIntersectionNodeID(const int nIntersect);

	const int GetStretchID(void)const;
	void SetStretchID(const int nStretchID);

	double GetRotation(void)const;
	void SetRotation(double dRotation);

public:
	virtual ALTOBJECT_TYPE GetType()const {return ALT_YIELDSIGN;} 
	virtual CString GetTypeName()const { return _T("YIELDSIGN");} 

	virtual void ReadObject(int nObjID);
	virtual int SaveObject(int nAirportID);
	virtual void UpdateObject(int nObjID);
	virtual void DeleteObject(int nObjID);

	virtual CString GetInsertScript(int nObjID) const;
	virtual CString GetUpdateScript(int nObjID) const;
	virtual CString GetDeleteScript(int nObjID) const;
	virtual CString GetSelectScript(int nObjID) const;

	static void ExportYieldSign(int nAirportID,CAirsideExportFile& exportFile);
	virtual void ExportObject(CAirsideExportFile& exportFile);
	virtual void ImportObject(CAirsideImportFile& importFile);

	virtual ALTObject * NewCopy();	
	virtual bool CopyData(const ALTObject* pObj);
};
