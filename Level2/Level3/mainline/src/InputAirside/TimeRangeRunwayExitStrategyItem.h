#pragma once
#include <vector>
#include <algorithm>
#include "../Common/TimeRange.h"
#include "StrategyType.h"
#include "InputAirsideAPI.h"
#include <algorithm>

class CAirsideExportFile;
class CAirsideImportFile;

class INPUTAIRSIDE_API RunwayExitPercentItem
{
public: 
	RunwayExitPercentItem(int nTimeRangeItemID);

	~RunwayExitPercentItem(){};

public:	
	int m_nID;
	int m_nTimeRangeItemID;
	int m_nRunwayID;
	int m_nRunwayMarkIndex;
	int m_nExitID;
	float m_fPercent;

	int getID(){	return m_nID; }
	int getExitID(){	return m_nExitID; }
	void SetTimeRangeID(int nID){	m_nTimeRangeItemID = nID; }
	void SetBacktrack(bool bBacktrack){ m_bBacktrackOp = bBacktrack; }
	bool IsBacktrack(){ return m_bBacktrackOp; }

	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile,int nTimeRangeItemID);
public:
	void SaveData();
	void UpdateData();
	void DeleteData();
private:
	bool m_bBacktrackOp;
};

typedef std::vector<RunwayExitPercentItem*> RunwayExitPercentVector;
typedef RunwayExitPercentVector::iterator RunwayExitPercentIter;

class INPUTAIRSIDE_API RunwayExitStrategyPercentItem
{
public:
	RunwayExitStrategyPercentItem(int nRunwayID, int nRunwayMark);
	~RunwayExitStrategyPercentItem();

public:
	bool IsSame(int nRunwayID, int nRunwayMark);
	int getRunwayID();
	int getMarkIndex();

	void AddItem(RunwayExitPercentItem * pItem);
	const RunwayExitPercentVector& getItems();

	//just clear the vector
	void clear();
	//delete data from database and clear 
	void DeleteData();
	void RefreshRunwayExitPercentItems(int nTimeRangeID);
	bool checkExitPercent();
protected:
	int m_nRunwayID;
	int m_nRunwayMark;

	RunwayExitPercentVector m_vItems;
};

typedef std::vector<RunwayExitStrategyPercentItem*> RunwayMarkExitStrategyVector;
typedef RunwayMarkExitStrategyVector::iterator RunwayMarkExitStrategyIter;

class TaxiSegmentData
{
public:
	TaxiSegmentData()
		:m_iTaxiwayID(-1)
		,m_iStartNode(-1)
		,m_iEndNode(-1)
	{

	}

	bool operator == (const TaxiSegmentData& segData)
	{
		if (m_iTaxiwayID != segData.m_iTaxiwayID)
			return false;
		
		if (m_iStartNode != segData.m_iStartNode)
			return false;
		
		if (m_iEndNode != segData.m_iEndNode)	
			return false;
		
		return true;
	}

	CString MakeDatabaseString()
	{
		CString strDatabase;
		strDatabase.Format(_T("%d,%d,%d"),m_iTaxiwayID,m_iStartNode,m_iEndNode);
		return strDatabase;
	}

	void ParseDatabaseString(const CString& strSegData)
	{
		CString strDatabase = strSegData;
		CString strLeft;
		int iPos = strSegData.Find(',');
		if (iPos == -1)
			return;

		strLeft = strSegData.Left(iPos);
		m_iTaxiwayID = atoi(strLeft);

		strDatabase = strDatabase.Right(strSegData.GetLength() - iPos - 1);
		iPos = strDatabase.Find(',');
		if (iPos == -1)
			return;
		CString strMid = strDatabase.Left(iPos);
		m_iStartNode = atoi(strMid);

		CString strRight = strDatabase.Right(strDatabase.GetLength() - iPos - 1);
		m_iEndNode = atoi(strRight);
	}
	int m_iTaxiwayID;
	int m_iStartNode;
	int m_iEndNode;
};

class RunwayChangeConditionData
{
public:
	RunwayChangeConditionData()
		:m_nID(-1)
		,m_bOperationOnTaxiway(false)
		,m_bArrStandOccupied(false)
	{

	}
	~RunwayChangeConditionData()
	{

	}
	void SetOperationStatus(bool bOperation)
	{
		m_bOperationOnTaxiway = bOperation;
	}
	bool GetOperationStatus()const
	{
		return m_bOperationOnTaxiway;
	}

	void SetArrStandOccupied(bool bOccupied)
	{
		m_bArrStandOccupied = bOccupied;
	}

	bool GetArrStandOccupied()const
	{
		return m_bArrStandOccupied;
	}

	void AddTaxiCondtion(const TaxiSegmentData& segData)
	{
		m_vTaxiwaySegData.push_back(segData);
	}

	void DeleteTaxiCondition(const TaxiSegmentData& segData)
	{
		std::vector<TaxiSegmentData>::iterator iter = std::find(m_vTaxiwaySegData.begin(),m_vTaxiwaySegData.end(),segData);
		if (iter != m_vTaxiwaySegData.end())
		{
			m_vTaxiwaySegData.erase(iter);
		}
	}

	int getTaxiConditionCount() const{return static_cast<int>(m_vTaxiwaySegData.size());}

	TaxiSegmentData *getTaxiCondition(int nIndex)
	{
		ASSERT( nIndex >= 0 && nIndex < static_cast<int>(m_vTaxiwaySegData.size()) );
		if( nIndex >= 0 && nIndex < static_cast<int>(m_vTaxiwaySegData.size()))
			return &(m_vTaxiwaySegData.at(nIndex));
		
		return NULL;
	}

	CString MakeDatabaseString()
	{
		CString strDatabase;
		for (size_t i = 0; i < m_vTaxiwaySegData.size(); i++)
		{
			TaxiSegmentData& taxiwaySegData = m_vTaxiwaySegData[i];
			CString strSeg = taxiwaySegData.MakeDatabaseString();
			if (strDatabase.IsEmpty())
			{
				strDatabase = strSeg;
			}
			else
			{
				strDatabase += ";";
				strDatabase += strSeg;
			}
		}
		return strDatabase;
	}

	void ParseDatabaseString(const CString& strDatabaseString)
	{
		CString strRight = strDatabaseString;
		int iPos = strRight.Find(';');
		while(iPos != -1)
		{
			CString strLeft = strRight.Left(iPos);
			TaxiSegmentData taxiwaySegData;
			taxiwaySegData.ParseDatabaseString(strLeft);
			m_vTaxiwaySegData.push_back(taxiwaySegData);

			strRight = strRight.Right(strRight.GetLength() - iPos -1);

			iPos = strRight.Find(';');
		}

		//last one
		if (strRight.IsEmpty() == false)
		{
			TaxiSegmentData taxiwaySegData;
			taxiwaySegData.ParseDatabaseString(strRight);
			m_vTaxiwaySegData.push_back(taxiwaySegData);
		}
	}

	void ReadData(int nParentID);
	void SaveData(int nParentID);
	void UpdateData();
	void DeleteData();
private:
	int  m_nID;
	bool m_bOperationOnTaxiway;
	bool m_bArrStandOccupied;
	std::vector<TaxiSegmentData> m_vTaxiwaySegData;
};

class INPUTAIRSIDE_API RunwayExitPriorityItem
{
public: 
	RunwayExitPriorityItem(int nTimeRangeItemID);

	~RunwayExitPriorityItem(){};

public:	
	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile,int nTimeRangeItemID);

	void SaveData();
	void UpdateData();
	void DeleteData();

	int getID() const { return m_nID; }
	int getTimeRangeItemID() const { return m_nTimeRangeItemID; }
	int getRunwayID() const { return m_nRunwayID; }
	int getRunwayMarkIndex() const { return m_nRunwayMarkIndex; }
	int getExitID() const { return m_nExitID; }
	int getPriorityIndex() const { return m_nPriorityIndex; }

	bool IsBacktrack() const { return m_bBacktrack; }
	void setBacktrack(bool val) { m_bBacktrack = val; }

	void SetTimeRangeID(int nTimeRangeID) { m_nTimeRangeItemID = nTimeRangeID; }
	void setPriorityIndex(int val) { m_nPriorityIndex = val; }

	RunwayChangeConditionData& GetConditionData() {return m_conditionData;}
public:
	int  m_nID;
	int  m_nTimeRangeItemID;
	int  m_nRunwayID;
	int  m_nRunwayMarkIndex;
	int  m_nExitID;
	int  m_nPriorityIndex;
	bool m_bBacktrack;

	RunwayChangeConditionData m_conditionData;
};

typedef std::vector<RunwayExitPriorityItem*> RunwayExitStrategyPriorityVector;
typedef RunwayExitStrategyPriorityVector::iterator RunwayExitStrategyPriorityIter;

class INPUTAIRSIDE_API RunwayExitStrategyPriorityItem
{
public:
	RunwayExitStrategyPriorityItem(int nRunwayID, int nRunwayMark);
	~RunwayExitStrategyPriorityItem();

public:
	bool IsSame(int nRunwayID, int nRunwayMark) const { return (m_nRunwayID == nRunwayID && m_nRunwayMark == nRunwayMark); }
	int getRunwayID() const { return m_nRunwayID; }
	int getMarkIndex() const { return m_nRunwayMark; }

	void AddItem(RunwayExitPriorityItem* pItem);
	void DeleteItem(RunwayExitPriorityItem* pItem);
	const RunwayExitStrategyPriorityVector& getItems();

	void MoveUp(RunwayExitPriorityItem* pItem);
	void MoveDown(RunwayExitPriorityItem* pItem);
	void MoveTop(RunwayExitPriorityItem* pItem);
	void MoveBottom(RunwayExitPriorityItem* pItem);

	void InsertItem(RunwayExitPriorityItem* pItem, int nIndex);
	bool ExchangeItem(RunwayExitPriorityItem* pItem, int nIndex);

	void ResortPriorities();

	//just clear the vector
	void clear();
	//delete data from database and clear 
	void DeleteData();
	void RefreshRunwayExitPriorities(int nTimeRangeID);

protected:

	int m_nRunwayID;
	int m_nRunwayMark;

	RunwayExitStrategyPriorityVector m_vItems;

	RunwayExitStrategyPriorityVector m_vDelItems;
};

typedef std::vector<RunwayExitStrategyPriorityItem*> RunwayExitStrategyPriorityiesVector;
typedef RunwayExitStrategyPriorityiesVector::iterator RunwayExitStrategyPrioritiesIter;


class  INPUTAIRSIDE_API TimeRangeRunwayExitStrategyItem
{
public:
	enum StrategyMode
	{
		ProbabilityBased = 0,
		PriorityBased,
	};
	TimeRangeRunwayExitStrategyItem(int nStandStrategyID);
	~TimeRangeRunwayExitStrategyItem(void);

	void ReadData();
	void SaveData();
	void UpdateData();
	void DeleteData();

	void RemoveAll(); // only remove the data in memory

	TimeRange GetTimeRange(){	return m_dtTimeRange; }
	void SetTimeRange(TimeRange& val){	m_dtTimeRange = val; }

	void setID(int nID){ m_nID = nID;}
	int getID(){ return m_nID;}
	void SetStandStrategyID(int nID){ m_nStandStrategyID = nID; }

	void setStrategyType(RunwayExitStrategyType rwStrategyType){	m_emStrategyType = rwStrategyType; }
	RunwayExitStrategyType getStrategyType(){	return m_emStrategyType; }

	StrategyMode getStrategyManagedExitMode() const { return m_strategyManagedExitMode; }
	void setStrategyManagedExitMode(StrategyMode val) { m_strategyManagedExitMode = val; }

	ElapsedTime getStartTime(){	return m_dtTimeRange.GetStartTime(); }
	ElapsedTime getEndTime(){	return m_dtTimeRange.GetEndTime(); }
	void setStartTime(ElapsedTime etStart){	m_dtTimeRange.SetStartTime(etStart); }
	void setEndTime(ElapsedTime etEnd){	m_dtTimeRange.SetEndTime(etEnd); }


	void AddRunwayExitPercentItem(RunwayExitPercentItem* pItem);
	void DeleteRunwayExitPercentItem(int nRunwayID,int nRunwayMark);

	void AddRunwayExitPriority(RunwayExitPriorityItem* pItem);
	void DeleteRunwayExitPriority(int nRunwayID,int nRunwayMark);

	BOOL checkExitPercent();

	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile,int nStandStrategyID);


	RunwayExitStrategyPercentItem* FindRunwaExitPercentItem(int nRunwayID, int nRunwayMark);
	RunwayExitStrategyPercentItem* GetRunwayExitPercentItem(int nRunwayID, int nRunwayMark);
	RunwayExitStrategyPriorityItem* FindRunwayExitPriorityItem( int nRunwayID, int nRunwayMark );
	RunwayExitStrategyPriorityItem* GetRunwayExitPriorityItem( int nRunwayID, int nRunwayMark );

	RunwayMarkExitStrategyVector& GetRunwayMarkExitItems() { return m_vRunwayMarkItems; }
	RunwayExitStrategyPriorityiesVector& GetRunwayExitPriorities() { return m_vPriorityItems;}

	void DeleteRunwayMarkExitItem(RunwayExitStrategyPercentItem * pItem);
	void DeleteRunwayMarkExitPriorities(RunwayExitStrategyPriorityItem * pItem);

	void DeleteDataFromDatabase();

protected:
	void SortPriorities();
	void SaveVectorData(int nID);

	void RefreshRunwayExitPercentItems(int nTimeRangeID);
	void RefreshRunwayExitAllPriorities( int nTimeRangeID );
	void AddToProbabilityStructure(RunwayExitPercentItem* pItem);
	void AddToPrioritiesStructure(RunwayExitPriorityItem* pItem);
private:
	int m_nID;
// 	int m_nFltTypeItemID;
	int m_nStandStrategyID;
	TimeRange m_dtTimeRange;
	RunwayExitStrategyType m_emStrategyType;

	StrategyMode m_strategyManagedExitMode;
	RunwayMarkExitStrategyVector m_vRunwayMarkItems;
	RunwayExitStrategyPriorityiesVector m_vPriorityItems;
};
