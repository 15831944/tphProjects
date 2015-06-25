// SimResult.h: interface for the CSimResult class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMRESULT_H__82985A84_8C54_4D1F_AD69_5E0E50002D51__INCLUDED_)
#define AFX_SIMRESULT_H__82985A84_8C54_4D1F_AD69_5E0E50002D51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "common\template.h"
#include "common\reportitem.h"
#include "common\elaptime.h"
#include "assert.h"
#include "common\ProjectManager.h"

class CSimItem
{
public:
	CSimItem();
	~CSimItem();
	CSimItem( const CString& _strPath,const CString& _strSubSimName );
private:
	std::map< InputFiles , CString > m_logs;
	CReportSet m_reports;
	CString m_strLogDirectoryName;
	CString m_strSubSimName;
	
	ElapsedTime m_timeBegin;
	ElapsedTime m_timeEnd;

public:
	const CString& getLogDirName( void ) const { return m_strLogDirectoryName; }
	const CString& getSubSimName( void ) const { return m_strSubSimName; }
	ElapsedTime getBeginTime( void ) const { return m_timeBegin; }
	ElapsedTime getEndTime( void ) const { return m_timeEnd; }

	void setLogDirName( const CString& _strName ) { m_strLogDirectoryName = _strName; }
	void setSubSimName( const CString& _strName ) { m_strSubSimName = _strName;	}
	void setBeginTime( ElapsedTime _beginTime ) { m_timeBegin = _beginTime;	}
	void setEndTime( ElapsedTime _endTime ) { m_timeEnd = _endTime;	}

	bool getLogsName( int _iLogsType, CString& _strLogsName );
	const CReportSet& getReportSet( void )  const { return m_reports; }
	CReportSet* GetReportSet( void ) { return &m_reports;	}
	const std::map< InputFiles , CString >& getLogsMap( void )  const { return m_logs; }
	
	void readFromFile( ArctermFile& file);
	void writeToFile( ArctermFile& file ) const;

	bool initSimItem( const CSimParameter* _pSimPara,InputTerminal* _pInTerm, const CString& _strPath ) ;
	bool isReportRealExist( int _ReportType ) const;
private:
	void initLogsName();
};

//////////////////////////////////////////////////////////////////////////
class CSimResult  
{
public:
	CSimResult();
	virtual ~CSimResult();
	CSimResult( const CSimResult& _anthoer );
	CSimResult& operator = ( const CSimResult& _anthoer );
private:
	std::vector<CSimItem> m_vSimResults;

public:
	const CSimItem& getAt( int _index ) const
	{
		assert( _index>=0 && (unsigned)_index<m_vSimResults.size() );
		return m_vSimResults[_index];
	}

	CSimItem* At( int _index )
	{
		assert( _index>=0 && (unsigned)_index<m_vSimResults.size() );
		return &m_vSimResults[_index];
	}

	int getCount( void ) const { return m_vSimResults.size(); }

	void readFromFile( ArctermFile& file );
	void writeToFile( ArctermFile& file ) const ;

	bool initSimResult( const CSimParameter* _pSimPara, InputTerminal* _pInTerm, const CString& _strPath);
};

#endif // !defined(AFX_SIMRESULT_H__82985A84_8C54_4D1F_AD69_5E0E50002D51__INCLUDED_)
