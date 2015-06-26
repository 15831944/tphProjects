#pragma once
//#include "../Inputs/Flt_cnst.h"

#include "InputAirsideAPI.h"

enum TaxiwayConstraintType
{
	TaxiwayConstraint_unknown = 0,
	WeightConstraint,
	WingSpanConstraint,

	TaxiwayConstraint_Count,
};

class CADORecordset;
class CAirsideImportFile;
class CAirsideExportFile;
/////////////////////////////////////////////////////////////////////
//
class INPUTAIRSIDE_API CTaxiwayConstraintData
{
public:
	CTaxiwayConstraintData();
	~CTaxiwayConstraintData();

public:
	void setID(int nID){ m_nID = nID; }
	int	 getID(){ return m_nID; }

	void SetFromID(int nFromID);
	int	 GetFromID();

	void SetToID(int nToID);
	int	 GetToID();

	void SetMaxNumber(long lMaxNumber);
	long GetMaxNumber();

	void SetConstraintType(TaxiwayConstraintType emType);
	TaxiwayConstraintType GetConstraintType();

private:
	int m_nID;	
	int m_nFromID;
	int m_nToID;
	long m_lMaxNumber;

	TaxiwayConstraintType m_emType;

public:
	void ReadData(CADORecordset& adoRecordset);
	void SaveData(int nProjID,TaxiwayConstraintType consType,int nTaxiwayID);
	void DeleteData();
	void UpdateData();

	void ExportData(CAirsideExportFile& exportFile,TaxiwayConstraintType enumConsType,int nTaxiwayID);
	void ImportData(CAirsideImportFile& importFile);
};


/////////////////////////////////////////////////////////////////////
//
class INPUTAIRSIDE_API CTaxiwayConstraint
{
public:
	CTaxiwayConstraint();
	~CTaxiwayConstraint();

public:
	void SetTaxiwayID(int nTaxiwayID);
	int	 GetTaxiwayID();

	void AddItem(CTaxiwayConstraintData* pNewItem);
	void DelItem(CTaxiwayConstraintData* pDelItem);

	CTaxiwayConstraintData* GetItem(int nIndex);
	int GetItemCount();

private:
	int m_nTaxiwayID;

	std::vector<CTaxiwayConstraintData *> m_vConstraintData;
	std::vector<CTaxiwayConstraintData *> m_vConstraintDataNeedDel;
public:

	void ReadData(CADORecordset& adoRecordset);
	void SaveData(int nProjID,TaxiwayConstraintType consType);
	void DeleteData(int nProjID,TaxiwayConstraintType consType);
	void UpdateData();


	void ExportData(CAirsideExportFile& exportFile,TaxiwayConstraintType enumConsType);
};

/////////////////////////////////////////////////////////////////////
//
class INPUTAIRSIDE_API CTaxiwayConstraintList
{
public:
	CTaxiwayConstraintList();
	~CTaxiwayConstraintList();

public:
	void setProjID(int nProjID){ m_nProjID = nProjID; }
	int	 getProjID(){ return m_nProjID; }

	void SetConstraintType(TaxiwayConstraintType emType);
	TaxiwayConstraintType GetConstraintType();

	void AddItem(CTaxiwayConstraint* pNewItem);
	void DelItem(CTaxiwayConstraint* pDelItem);
	void DelItem(int nIndex);

	CTaxiwayConstraint* GetItem(int nIndex);
	int GetItemCount();

private:
	int m_nProjID;
	TaxiwayConstraintType m_emType;

	std::vector<CTaxiwayConstraint *> m_vConstraint;
	std::vector<CTaxiwayConstraint *> m_vConstraintNeedDel;

public:
	void ReadData(int nprojID);
	void ReadWeightWingspanConsData(int nprojID);

	void SaveData(int nProjID);

	void DeleteData();

protected:
	void AddTaxiwayConstraintDataItem(int taxiwayID,CTaxiwayConstraintData *tcData);

public:
	static void ExportTaxiwayConstraint(CAirsideExportFile& exportFile);
	static void ImportTaxiwayConstraint(CAirsideImportFile& importFile);



	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);
	
};
