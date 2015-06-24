#pragma once
#include "basereport.h"
#include "RetailElement.h"
class CRetailLogEntry;

struct ProcessorMap
{
	std::vector<int> m_processorMap;
	StoreProcessorElement m_storeElement;//store define in parameter
};

class CRetailReport :
	public CBaseReport
{
public:
	CRetailReport(Terminal* _pTerm, const CString& _csProjPath);
	virtual ~CRetailReport(void);

	virtual int GetReportFileType (void) const { return RetailReportFile; };
	const ElapsedTime& GetStartTime()const {return m_startTime;}
	const ElapsedTime& GetEndTime()const {return m_endTime;}
	bool GetRetailElement(long nPaxID,RetailPaxElement& logEntry);
	bool GetProductName(int nProductID,CString& sProductName)const;

protected:
	virtual int GetSpecFileType (void) const { return RetailSpecFile; };
	virtual const char *GetTitle (void) const { return "Retail Activity Report"; };
	virtual void GenerateSummary( ArctermFile& p_file );
	virtual void GenerateDetailed( ArctermFile& p_file );
	virtual void InitParameter( const CReportParameter* _pPara );
private:
	void BuildIntervalRetailData(long lStart,long lEnd,CStatisticalTools<double>& _tools);
	void BuildProcessorMap();
	void GenerateResult();
	void InitRetailData(const CRetailLogEntry& retailEntry);
	void writeEntry (ArctermFile& p_file);


private:
	std::vector<ProcessorMap> m_processorMap;

};
