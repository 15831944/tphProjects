#pragma once
#include "..\Database\ADODatabase.h"
#include "..\Database\DBElementCollection_Impl.h"

#include "InputAirsideAPI.h"

enum DirectionalityConstraintType
{
	Direction_Bearing =0,
	Direction_Reciprocal
};

class CADORecordset;
class CAirsideExportFile;
class CAirsideImportFile;

class INPUTAIRSIDE_API DirectionalityConstraintItem : public DBElement
{
public:
	DirectionalityConstraintItem(void);
	virtual ~DirectionalityConstraintItem(void);

public:
	virtual void InitFromDBRecordset(CADORecordset& recordset);

	int  GetTaxiwayID();
	void SetTaxiwayID(int nID);
	int  GetSegFromID();
	void SetSegFromID(int nID);
	int  GetSegToID();
	void SetSegToID(int nID);
	double GetBearingDegree();
	void  SetBearingDegree(double degree);
	void SetDirConsType(DirectionalityConstraintType emType);
	DirectionalityConstraintType GetDirConsType();

	void SetAirportID(int nAirportID){	m_nAirportID = nAirportID ; }

public:
	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);

protected:

	virtual void GetInsertSQL(int nParentID,CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;
	virtual void GetSelectSQL(int nID,CString& strSQL)const;

protected:
	int					m_nAirportID;
	int					m_nTaxiwayID;
	int					m_nSegFromID;
	int					m_nSegToID;
	double				m_dBearingDegree;
	DirectionalityConstraintType	m_emType;
};



class INPUTAIRSIDE_API DirectionalityConstraints : public DBElementCollection < DirectionalityConstraintItem >
{
public:
	DirectionalityConstraints();
	virtual ~DirectionalityConstraints(void);

public:
	virtual void ReadData(int nParentID);
	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;
	static void ExportDirectionalityConstraints(CAirsideExportFile& exportFile);
	static void ImportDirectionalityConstraints(CAirsideImportFile& importFile);
private:
	//export dll symbols
	void DoNotCall();
	
};
