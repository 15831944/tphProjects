#pragma once
#include "altobject.h"
#include "../Common/path2008.h"
#include <vector>
#include "IntersectionsDefine.h"

class INPUTAIRSIDE_API  LineParking :	public ALTObject
{
public:
	LineParking(void);
	~LineParking(void);
protected:
	double m_dSpotWidth;
	int m_iSpotNum;
	InsecObjectPartVector m_vrInsecObjectPart;
	CPath2008 m_path;
	int m_nPathID;
public:
	virtual ALTOBJECT_TYPE GetType()const {return ALT_LINEPARKING;} 
	virtual CString GetTypeName()const { return _T("LINEPARKING");} 

	const InsecObjectPartVector & GetIntersectionsPart(void);
	void SetIntersectionsPart(const InsecObjectPartVector & iopvrSet);
	const CPath2008& GetPath(void)const;
	void SetPath(const CPath2008& _path);
	double GetSpotWidth(void);
	void SetSpotWidth(double dSpotWidth);
	int GetSpotNumber(void);
	void SetSpotNumber(int iSpotNum);

	virtual void ReadObject(int nObjID);
	virtual int SaveObject(int nAirportID);
	virtual void UpdateObject(int nObjID);
	virtual void DeleteObject(int nObjID);

	virtual CString GetInsertScript(int nObjID) const;
	virtual CString GetUpdateScript(int nObjID) const;
	virtual CString GetDeleteScript(int nObjID) const;
	virtual CString GetSelectScript(int nObjID) const;

	static void ExportLineParking(int nAirportID,CAirsideExportFile& exportFile);
	virtual void ExportObject(CAirsideExportFile& exportFile);
	virtual void ImportObject(CAirsideImportFile& importFile);
};
