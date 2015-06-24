// ConveyorWaitLenghtReport.h: interface for the CConveyorWaitLenghtReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONVEYORWAITLENGHTREPORT_H__D0EA70B9_A568_47E3_BBF5_D19D4B21C33C__INCLUDED_)
#define AFX_CONVEYORWAITLENGHTREPORT_H__D0EA70B9_A568_47E3_BBF5_D19D4B21C33C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseReport.h"
class CIntervalStat;
class CConveyorWaitLenghtReport : public CBaseReport  
{
public:
	CConveyorWaitLenghtReport(Terminal* _pTerm, const CString& _csProjPath);
	virtual ~CConveyorWaitLenghtReport();
public:
	
	virtual int GetReportFileType (void) const { return ConveyorWaitLengthReport; };
	virtual int GetSpecFileType (void) const { return ConveyorWaitLengthSpecFile; };
    virtual const char *GetTitle (void) const { return "Conveyor Wait Length Report"; };
	virtual void InitParameter( const CReportParameter* _pPara );
	virtual void GenerateSummary( ArctermFile& p_file );
	virtual void GenerateDetailed( ArctermFile& p_file );
public:
	
private:
	void initIntervalStats();
	void calculateQueueLengths ( const ProcessorID* _pProcID );
	void writeEntries (const char *p_id, ArctermFile& p_file) const;
	bool isInterestedEvent(const ProcEventStruct& _event )const;
	
	void updateIntervalStats (void);
    void writeAverages (const ProcessorID *id, ArctermFile& p_file) const;
	
private:
	int* m_pLengthList;
    int m_nIntervalCount;
	CIntervalStat* m_pIntervalStats;

};

#endif // !defined(AFX_CONVEYORWAITLENGHTREPORT_H__D0EA70B9_A568_47E3_BBF5_D19D4B21C33C__INCLUDED_)
