// SimAndReportManager.h: interface for the CSimAndReportManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMANDREPORTMANAGER_H__EC3613AB_3263_4DCC_A584_4858C44E47E3__INCLUDED_)
#define AFX_SIMANDREPORTMANAGER_H__EC3613AB_3263_4DCC_A584_4858C44E47E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include"common\template.h"
#include<vector>
#include<map>
#include "reports\ReportType.h"
#include "common\SimResult.h"
#include "common\dataset.h"
#include "StartDate.h"	// Added by ClassView

#include "../Common/StartDate.h"

class CSimParameter;
class CSimAndReportManager :public DataSet
{
private:
	CString m_sSimName;
	CString m_sSubSimName;
	ENUM_REPORT_TYPE m_enReportType;
	std::map<ENUM_REPORT_TYPE, CString>m_mapReportTypeToFileName;
	std::map<InputFiles, CString>m_mapLogTypeToFileName;
	CString m_sCurrentReportFileName;
	//CString m_sCurrent
	std::vector< CSimResult > m_vSimResult;	// all sim result vector
	
	bool m_bModifiedInput;	// remember if the input match the log;

public:
	CSimAndReportManager();
	virtual ~CSimAndReportManager();
public:
	void SetStartDate(const CStartDate& _date);
	const CStartDate& GetStartDate() const;
	bool addNewSimResult( const CSimParameter* _pSimPara, const CString& _strPath, InputTerminal* _pInTerm );
	bool removeSimResult( void );
	//query manager data
	void GetAllSimResultName( std::vector<CString>& _vSimResultName )const;
	void GetSubSimResultName( std::vector<CString>& _vSubSimResultName )const;
	void GetReportsOfSubSimResult( const CString& _stSubSimResultName, std::vector<CString>& _vSubSimResultReports )const;

	// set manager state
	void SetCurrentSimResult(const CString& _sSubSimName );
	void SetCurrentSimResult( int _subSimIndex );
	void SetCurrentReportType( ENUM_REPORT_TYPE _enReportType );

	ENUM_REPORT_TYPE GetCurrentReportType()const { return m_enReportType;	}
	CString GetCurrentLogsFileName(InputFiles _enLogType, const CString& _strPath );
	CString GetCurrentReportFileName( const CString& _strPath );
	
	CSimItem* getSimItem( int _subSimIndex );
	CSimItem* getSimItem( const CString& _sSubSimName );
	CReportItem* getCurReportItem( void );
	void ClearAll();	
	
	int getSubSimResultCout( void );
	
	bool IsInputModified(){ return m_bModifiedInput; }
	void SetInputModified( bool _bModifiedInput ){ m_bModifiedInput = _bModifiedInput; }
	
	void SetAndSaveInputModified( const CString& _csProjPath );

	void SetUserStartTime(const ElapsedTime& _date);
	const ElapsedTime& GetUserStartTime() const;
	void SetUserEndTime(const ElapsedTime& _date);
	const ElapsedTime& GetUserEndTime() const;
		
	void SetAAStartTime(const long time) { m_nAAStartTime = time; }
	long GetAAStartTime() const { return m_nAAStartTime; }
	void SetAAEndTime(const long time) { m_nAAEndTime = time; }
	long GetAAEndTime() const { return m_nAAEndTime; }

	void SetAirsideSim(const bool bAirsideSim){ m_bAirsideSim = bAirsideSim; }
	bool IsAirsideSim(){ return m_bAirsideSim; }

	// Description: Read Data From Default File. Creat new if no file exist. Update to newer version if necessary.
	// Exception:	FileVersionTooNewError
	void loadDataSet2 (const CString& _pProjPath,int mode);

	// Exception:	DirCreateError;
	void saveDataSet2 (const CString& _pProjPath, bool _bUndo,int mode) const;
	// read and write the manger
private:

	///////////////////////////////////////////////////////////////////////////
	// dataset read and write
    virtual void clear (void);
    virtual void readData (ArctermFile& p_file);
	virtual void readObsoleteData ( ArctermFile& p_file );
    virtual void writeData (ArctermFile& p_file) const;

    virtual const char *getTitle (void) const
        { return "Sim And Report File"; }
    virtual const char *getHeaders (void) const
        { return "Sim Result One By One"; }
protected:
	CStartDate m_startDate;
	ElapsedTime m_UserStartTime, m_UserEndTime;
	long m_nAAStartTime;
	long m_nAAEndTime;
	bool m_bAirsideSim;

	// read the sim result part.
	void readSimResult ( ArctermFile& p_file );

};

#endif // !defined(AFX_SIMANDREPORTMANAGER_H__EC3613AB_3263_4DCC_A584_4858C44E47E3__INCLUDED_)
