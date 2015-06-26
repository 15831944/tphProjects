#pragma once
#include "../Database/dbelement.h"
#include "InputAirsideAPI.h"

class CAirsideImportFile;
class CAirsideExportFile;

enum INTERVENTIONMETHOD
{
	IM_SPEEDCTRL = 1,
	IM_SIDESTEP,
	IM_GOAROUND
};


class INPUTAIRSIDE_API CCtrlInterventionSpecData :
	public DBElement
{
public:
	CCtrlInterventionSpecData(void);
	virtual ~CCtrlInterventionSpecData(void);

public:
	void SetRunwayID(int nRunwayID);
	int GetRunwayID()const;
	void SetMarkingIndex(int nMarkingIndex);
	int GetMarkingIndex();
	void SetPriorityOne(INTERVENTIONMETHOD enumInterventionMethod);
	INTERVENTIONMETHOD GetPriorityOne()const;  
	void SetPriorityTwo(INTERVENTIONMETHOD enumInterventionMethod);
	INTERVENTIONMETHOD GetPriorityTwo()const;
	void SetPriorityThree(INTERVENTIONMETHOD enumInterventionMethod);
	INTERVENTIONMETHOD GetPriorityThree()const;

	int GetPriority(INTERVENTIONMETHOD enumInterventionMethod)const;

	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);
	void Save(int nParentID);

	//DBElement
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void GetInsertSQL(int nProjID, CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;

private:
	int m_RunwayID;
	int m_MarkingIndex;
	INTERVENTIONMETHOD m_PriorityOne;
	INTERVENTIONMETHOD m_PriorityTwo;
	INTERVENTIONMETHOD m_PriorityThree;
};
