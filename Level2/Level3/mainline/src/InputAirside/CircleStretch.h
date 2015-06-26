#pragma once
#include "altobject.h"
#include "../Common/Path2008.h"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API CircleStretch :
	public ALTObject
{
	friend class CCircleStretch3D;
public:
	CircleStretch(void);
	CircleStretch(const CircleStretch&);
	~CircleStretch(void);
	CircleStretch& operator=(const CircleStretch& other);
	static const GUID& getTypeGUID();
protected:
	int m_iLaneNum;
	double m_dLaneWidth;
	CPath2008 m_path;
	int m_nPathID;
public:
	const CPath2008& GetPath(void)const;
	void SetPath(const CPath2008& _path);
	CPath2008& getPath(void);

	double GetLaneWidth(void)const;
	void SetLaneWidth(double dLaneWidth);

	int GetLaneNumber(void)const;
	void SetLaneNumber(int iLaneNumber);

	void UpdatePath();

public:
	virtual ALTOBJECT_TYPE GetType()const {return ALT_CIRCLESTRETCH;} 
	virtual CString GetTypeName()const { return _T("CIRCLE STRETCH");} 

	virtual void ReadObject(int nObjID);
	virtual int SaveObject(int nAirportID);
	virtual void UpdateObject(int nObjID);
	virtual void DeleteObject(int nObjID);

	virtual CString GetInsertScript(int nObjID) const;
	virtual CString GetUpdateScript(int nObjID) const;
	virtual CString GetDeleteScript(int nObjID) const;
	virtual CString GetSelectScript(int nObjID) const;

	//static void ExportStretch(int nAirportID,CAirsideExportFile& exportFile);
	//virtual void ExportObject(CAirsideExportFile& exportFile);
	//virtual void ImportObject(CAirsideImportFile& importFile);

	virtual bool CopyData(const ALTObject* pObj);

	virtual ALTObject * NewCopy();



};
