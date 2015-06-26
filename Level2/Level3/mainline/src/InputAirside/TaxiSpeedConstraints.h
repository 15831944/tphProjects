#pragma once

#include "Database/DBElement.h"
#include "Database/DBElementCollection_Impl.h"
#include "Common/FLT_CNST.H"
#include "IntersectionNode.h"
#include "InputAirsideAPI.h"

class CAirsideExportFile;
class CAirsideImportFile;

class CAirportDatabase;


#define TAXIWAY_NODE_INVALID     (-1)



class INPUTAIRSIDE_API TaxiwaySpeedConstraintDataItem : public DBElement
{
public:
	TaxiwaySpeedConstraintDataItem();
	virtual ~TaxiwaySpeedConstraintDataItem();

	// DB
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void GetInsertSQL(int nParentID,CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;
//	virtual void GetSelectSQL(int nID,CString& strSQL)const;

	// Import & Export
	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);

	// data member access
	int GetFromNodeID() const { return m_nFromNodeID; }
	void SetFromNodeID(int val) { m_nFromNodeID = val; InitFromNodeName(); }
	int GetToNodeID() const { return m_nToNodeID; }
	void SetToNodeID(int val) { m_nToNodeID = val; InitToNodeName(); }
	double GetMaxSpeed() const { return m_dMaxSpeed; } // UNIT: kts
	void SetMaxSpeed(double val) { m_dMaxSpeed = val; } // UNIT: kts

	CString GetMaxSpeedString() const; // UNIT: kts
	double GetMaxSpeedAsCMpS() const; // UNIT: CMpS

	CString GetFromNodeName() const { return m_strFromNodeName; }
	CString GetToNodeName() const { return m_strToNodeName; }

protected:
	void InitFromNodeName();
	void InitToNodeName();

private:
	int m_nFromNodeID;
	int m_nToNodeID;
	double m_dMaxSpeed; // UNIT: kts, sometime it needs unit conversion.
	CString m_strFromNodeName;
	CString m_strToNodeName;
};

class INPUTAIRSIDE_API TaxiwaySpeedConstraintDataList : public DBElementCollection<TaxiwaySpeedConstraintDataItem>
{
public:
	TaxiwaySpeedConstraintDataList(void);
	~TaxiwaySpeedConstraintDataList(void);

	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;

// 	static void ExportData(CAirsideExportFile& exportFile);
// 	static void ImportData(CAirsideImportFile& importFile);

};




class INPUTAIRSIDE_API TaxiwaySpeedConstraintFlttypeItem : public DBElement
{
public:
	TaxiwaySpeedConstraintFlttypeItem();
	virtual ~TaxiwaySpeedConstraintFlttypeItem();

	// DB
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void GetInsertSQL(int nParentID,CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;
//	virtual void GetSelectSQL(int nID,CString& strSQL)const;
	virtual void SaveData(int nParentID);
	void DeleteData();


	// Import & Export
// 	void ExportData(CAirsideExportFile& exportFile);
// 	void ImportData(CAirsideImportFile& importFile);

	void RefreshFlightTypeAirportDB(CAirportDatabase* pAirportDB);

	FlightConstraint& GetFlightType() { return m_flightType; }
	const FlightConstraint& GetFlightType() const { return m_flightType; }
	void SetFlightType(const FlightConstraint& val);

	TaxiwaySpeedConstraintDataList& GetSpeedConstraints() { return m_SpeedConstraints; }
	const TaxiwaySpeedConstraintDataList& GetSpeedConstraints() const { return m_SpeedConstraints; }

	CString GetFltTypeName() const;

	void InitFltTypeString();
	void SetFltTypeString(CString val);

	BOOL CheckAllRoutesValid(const IntersectionNodeList& sortedNodeList, CString& strError);

protected:
	CString GetFltTypeString() const { return m_strFltType; }

	BOOL FindNodeIndex( const IntersectionNodeList& sortedNodeList, int& nNodeIndex, int nNodeID );
	BOOL IsConflicted( const IntersectionNodeList& sortedNodeList, TaxiwaySpeedConstraintDataItem* pConstraintDataItem1, TaxiwaySpeedConstraintDataItem* pConstraintDataItem2, CString& strError );
private:
	CString          m_strFltType;
	FlightConstraint m_flightType;
	TaxiwaySpeedConstraintDataList m_SpeedConstraints;
};

class INPUTAIRSIDE_API TaxiwaySpeedConstraintFlttypeList : public DBElementCollection<TaxiwaySpeedConstraintFlttypeItem>
{
public:
	TaxiwaySpeedConstraintFlttypeList(void);
	~TaxiwaySpeedConstraintFlttypeList(void);

	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;
// 	static void ExportData(CAirsideExportFile& exportFile);
// 	static void ImportData(CAirsideImportFile& importFile);

	void RefreshAirpotDB(CAirportDatabase* pAirportDB);
};



class INPUTAIRSIDE_API TaxiwaySpeedConstraintTaxiwayItem : public DBElement
{
public:
	TaxiwaySpeedConstraintTaxiwayItem();
	virtual ~TaxiwaySpeedConstraintTaxiwayItem();

	// DB
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void GetInsertSQL(int nParentID,CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;
//	virtual void GetSelectSQL(int nID,CString& strSQL)const;
	virtual void SaveData(int nParentID);
	void DeleteData();

	// data member access
	int GetTaxiwayID() const { return m_nTaxiwayID; }
	void SetTaxiwayID(int val) { m_nTaxiwayID = val; InitStandString(); }
	CString GetTaxiwayString() const { return m_strTaxiwayString; }

	TaxiwaySpeedConstraintFlttypeList& GetConstraintFltType() { return m_ConstraintFltType; }
	const TaxiwaySpeedConstraintFlttypeList& GetConstraintFltType() const { return m_ConstraintFltType; }
	void RefreshAirpotDB(CAirportDatabase* pAirportDB) { m_ConstraintFltType.RefreshAirpotDB(pAirportDB); }

	BOOL CheckAllRoutesValid(CString& strError);
protected:
	void InitStandString();

private:
	int     m_nTaxiwayID;
	CString m_strTaxiwayString;

	TaxiwaySpeedConstraintFlttypeList m_ConstraintFltType;

	class TaxiwayNodeSorter
	{
	public:
		TaxiwayNodeSorter(int nTaxiwayID)
			: m_nTaxiwayID(nTaxiwayID)
		{

		}
		bool operator()(const IntersectionNode& lhs, const IntersectionNode& rhs);
	private:
		int m_nTaxiwayID;
	};
};


class INPUTAIRSIDE_API TaxiSpeedConstraints : public DBElementCollection<TaxiwaySpeedConstraintTaxiwayItem>
{
public:
	TaxiSpeedConstraints(void);
	~TaxiSpeedConstraints(void);

	void ReadData(int nProjID, CAirportDatabase* pAirportDB);
	void RefreshAirpotDB(CAirportDatabase* pAirportDB);

	BOOL CheckAllRoutesValid(CString& strError);
private:
	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;
	virtual void ReadData(int nProjID);
// 	static void ExportData(CAirsideExportFile& exportFile);
// 	static void ImportData(CAirsideImportFile& importFile);
};
// template DBElementCollection<TaxiwaySpeedConstraintDataItem>;
// template DBElementCollection<TaxiwaySpeedConstraintFlttypeItem>;
// template DBElementCollection<TaxiwaySpeedConstraintTaxiwayItem>;
