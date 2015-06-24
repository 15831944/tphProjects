#pragma once
#include "rep_pax.h"
#include "StatisticalTools.h"

class StoreProcessorElement;
class CRetailReport;
class CRetailLogEntry;

struct RetailPaxData 
{
	ProcessorID m_procID;//shop name
	ElapsedTime m_lCheckOutServiceTime;
	ElapsedTime m_lEnterCheckOutQueueTime;
	ElapsedTime m_lCheckOutQueueTime;
	ElapsedTime m_lEnterStore;
	ElapsedTime m_lLeaveStore;
};

struct RetailShopData
{
	long m_lPaxID;
	long m_lPotentiallyBought;//sum of product count
	double m_dSales;
	bool m_bInventoryOk; //have or no
	bool m_bByPass;
	bool m_bCheckOut;
	double m_dAptRevenue;
	CString m_strProductNameList;//product name and count
};

typedef std::vector<RetailShopData*> RetailShopeDataList;

class RetailPaxElement :public ReportPaxEntry
{
public:
	RetailPaxElement(void);
	virtual ~RetailPaxElement(void);

public:
	void InitStoreElement(StoreProcessorElement* pStoreElement);
	//-------------------------------------------------------------------------
	//Summary:
	//		record passenger process at shop
	//-------------------------------------------------------------------------
	void BuildRetailShop(const ElapsedTime& p_start, const ElapsedTime& p_end);

	bool CalculateQueueTime(int nStart, int nEnd,ElapsedTime& eEnterQueueTime,ElapsedTime& eQueueTime);
	ElapsedTime CalculateServiceTime(int nStart,int nEnd);

	bool PassObject(const ElapsedTime& p_start, const ElapsedTime& p_end,int& nStartPos,int& nEndPos);

	const RetailPaxData& GetRetailPaxData()const {return m_paxData;}
private:
	RetailPaxData m_paxData;
	StoreProcessorElement* m_pStoreProcElement;
};

class StoreProcessorElement
{
public:
	StoreProcessorElement();
	~StoreProcessorElement();

	void InitStoreElement(const ProcessorID& procID,CRetailReport* pRetailReport);
	bool CalculateRetailShopData();
	void CalculateRetailShopSummaryData(const ElapsedTime& tStart, const ElapsedTime& tEnd,CStatisticalTools<double>& tempTools,double& dApt);

	void AddLogEntry(const CRetailLogEntry& logEntry);

	bool CheckOut(long ldx)const;
	bool ValidProcessor(long ldx)const;

	void writeEntry (ArctermFile& p_file);

	double GetTotalSales()const;
	CString GetIDString()const{return m_procID.GetIDString();}
private:
	RetailShopData* GetData(long nPaxID);
	void CalculatePaxRetailData();

	void Clear();

	bool GetRetailPaxElement(long lPaxID,RetailPaxElement& paxElement)const;
private:
	std::vector<CRetailLogEntry> m_vlogEntry;
	RetailShopeDataList m_shopDataList;
	ProcessorID	 m_procID;
	CRetailReport* m_pRetailReport;
	std::vector<RetailPaxElement> m_vRetailPaxElement;
};
