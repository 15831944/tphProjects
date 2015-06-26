#pragma once

#include "AirsideImportExportManager.h"
#include "..\Database\ADODatabase.h"
#include "../Database/DBElement.h"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API CStretchSetOption : public DBElement
{
public:
	CStretchSetOption(void);
	CStretchSetOption(int nProjID);
	~CStretchSetOption(void);

public:
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	void GetInsertSQL(CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;
	virtual void GetSelectSQL(int nID,CString& strSQL)const;
	void ReadData(int nID);
	void SaveDatas();

	void ImportData(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);
	static void ExportStretchSetOption(CAirsideExportFile& exportFile);
	static void ImportStretchSetOption(CAirsideImportFile& importFile);

	int GetProjID(void) const;
	void SetProjID(int nProjID);

	void SetDrive(int strDrive);
	int GetDrive(void) const;

	void SetLaneWidth(double dLaneWidth);
	double GetLaneWidth(void) const;

	void SetLaneNumber(int iLaneNumber);
	int GetLaneNumber(void) const;

private:
	
	int m_nProjID;
	int m_strDrive;
	double m_dLaneWidth;
	int m_iLaneNumber; 
};
