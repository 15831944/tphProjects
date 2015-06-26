#pragma once
#include "altobject.h"
#include <iostream>
#include <vector>
#include "..\Common\path2008.h"
#include "InputAirsideAPI.h"
using namespace std;

class INPUTAIRSIDE_API Contour :
	public ALTObject
{
public:
	Contour(void);
	virtual ~Contour(void);

	virtual ALTOBJECT_TYPE GetType()const { return ALT_CONTOUR; } ;
	virtual CString GetTypeName()const  { return "Contour"; };
	static const GUID& getTypeGUID();
	//ref_ptr<Contour> m_pBrother;
	//ref_ptr<Contour> m_pChild;

public:
	typedef ref_ptr<Contour> RefPtr;

	std::vector<Contour::RefPtr> m_pChild;
	virtual bool CopyData(const ALTObject* pObj);	
private:
	CPath2008 m_path;
	double m_dAlt;

	int m_nParentID;

	//database operation
public:
	// get a copy of this object , change its name in sequence
	//virtual ALTObject * NewCopy();
	virtual ALTObjectDisplayProp* NewDisplayProp();

	const CPath2008& GetPath()const;
	CPath2008& GetPath();
	void SetPath(const CPath2008& _path);

	void SetAltude(double alt);
	double GetAltitude();

	void SetParentID(int nParentID);
	int GetParentID(void);

	int GetChildCount();
	Contour* GetChildItem(int nIndex);

	void ReadObject(int nObjID);
	int SaveObject(int nAirportID);
	virtual void UpdateObject(int nObjID);
	virtual void DeleteObject(int nObjID);

	void UpdatePath();

protected:
	int m_nPathID;

	CString GetSelectScript(int nObjID) const;
	CString GetInsertScript(int nObjID) const;
	virtual CString GetUpdateScript(int nObjID) const;
	virtual CString GetDeleteScript(int nObjID) const;

	void ReadChildObject(int nParentObjID);
	CString GetSelectChildScript(int nParentObjID) const;

public:
	static void ExportContours(CAirsideExportFile& exportFile);

	void ExportContour(CAirsideExportFile& exportFile);
	void ImportObject(CAirsideImportFile& importFile);
};
