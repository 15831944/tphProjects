#pragma once
#include "../Reports/ReportType.h"
#include "../Common/MultiRunReport.h"
#include "../Common/ProjectManager.h"
//struct PROJECTINFO;
enum LoadReportType
{
	load_by_system = 0,
	load_by_user
};

class CAirsideReportManager;
class Terminal;
class CReportParameter;
class OnboardReportManager;
class LandsideReportManager;


class CARCReportManager
{
public:
	CARCReportManager(void);
	~CARCReportManager(void);

public:
	void SetMultiRunsReportType(ENUM_REPORT_TYPE reportType) { m_multiRunsRportType = reportType; }
	ENUM_REPORT_TYPE GetMultiRunsReportType() { return m_multiRunsRportType; }
	std::vector<int>& GetmSelectedMultiRunsSimResults() { return m_vSelectedMultiRunsSimResults; }
	CMultiRunReport& getMultiReport(){ return m_multiReport;}

public:
	CReportParameter* GetReportPara(){ return m_pCurrentReportPara;	}
	void SetLoadReportType( LoadReportType _type );
	void SetUserLoadReportFile( const CString& _strFile );
	LoadReportType GetLoadReportType(void) const;
	const CString& GetUserLoadReportFile( void ) const;
	void SetReportPara( CReportParameter* _pPara );
	CString GetRepTitle();
	CString GetAirsideReportTitle() const;
	CString GetOnboardReportTitle() const ;
	CString GetLandsideReportTitle() const ;
	int GetSpecType();
	void GenerateReport(const PROJECTINFO& strProjPath, Terminal* pTerminal, int nFloorCount);
	
	ENUM_REPORT_TYPE GetReportType() const { return m_reportType; }
	void SetReportType(ENUM_REPORT_TYPE rpType) { m_reportType = rpType; }
	
	
	BOOL GetShowGraph() const { return m_bShowGraph; }
	void SetShowGraph(BOOL bShowGraph) { m_bShowGraph = bShowGraph; }

public:
	CAirsideReportManager * GetAirsideReportManager() const { return m_pAirsideReportManager; }
	OnboardReportManager *GetOnboardReportManager() const {return m_pOnboardReportManager;}
	LandsideReportManager *GetLandsideReportManager() const {return m_pLandsideReportManager;}


protected:
	//terminal
	ENUM_REPORT_TYPE m_reportType;

	CReportParameter* m_pCurrentReportPara;
	LoadReportType m_eLoadReportType;
	CString m_strUserLoadReportFile;

	//airside
	CAirsideReportManager *m_pAirsideReportManager;

	//multiple run 
	ENUM_REPORT_TYPE m_multiRunsRportType;
	std::vector<int> m_vSelectedMultiRunsSimResults;
	CMultiRunReport m_multiReport;

	OnboardReportManager *m_pOnboardReportManager;

	BOOL m_bShowGraph;

	LandsideReportManager *m_pLandsideReportManager;


};
