// ReportItem.h: interface for the CReportItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REPORTITEM_H__C6F126FF_E81A_4B2D_9DE1_C624D0F9A272__INCLUDED_)
#define AFX_REPORTITEM_H__C6F126FF_E81A_4B2D_9DE1_C624D0F9A272__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class InputTerminal;
class ArctermFile;
class CSimAutoReportPara;
#include <map>
#include <vector>
#include "reports\ReportType.h"
//////////////////////////////////////////////////////////////////////////
class CReportParameter;
class InputTerminal;
class CSimParameter;
class CReportItem  
{
public:
	CReportItem();
	virtual ~CReportItem();
	CReportItem(  const CString& _strDirectory, const CString& _strPara, const CString& _strResult, int _iType );
private:
	CString m_strParaFileName;
	CString m_strResultFileName;
	CString m_strReportDirectoryName;
	int		m_iReportType;
	bool	m_bRealCreate;	
	
public:
	// set & get
	void setReportType( int _type ) { m_iReportType = _type; }
	int  getReportType( void ) const { return m_iReportType; }
	void setParaFileName( const CString& _strName ) { m_strParaFileName = _strName;	}
	const CString& getParaFileName( void ) const { return m_strParaFileName; }
	void setReultFileName( const CString& _strName ) { m_strResultFileName = _strName; }
	const CString& getResultFileName( void ) const { return m_strResultFileName; }
	void setReportDirectoryName(  const CString& _strName ) { m_strReportDirectoryName = _strName; }
	const CString& getReportDirectoryName( void ) const { return m_strReportDirectoryName; }
	void setCreateFlag( bool _flag ) { m_bRealCreate = _flag; }
	bool getCreateFlag( void ) const { return m_bRealCreate; }

	bool initReportItem( InputTerminal* _pInTerm , const CString& _strPath );

	void readFromFile( ArctermFile& file );
	void writeToFile(ArctermFile& file ) const;
	void SaveReportParaToFile( const CString& _strProjectPath,CReportParameter* m_pReportPara );

	//Note: the caller must delete return pointer, otherwise ,memory leak will occur
	CReportParameter* GetReoprtParaFromFile(  const CString& _strProjectPath,InputTerminal* _pInTerm );
	
private:
	CString getReportString( int _type );
};

//////////////////////////////////////////////////////////////////////////
typedef std::map<ENUM_REPORT_TYPE, CReportItem> ReportTypeToNameMap;
//////////////////////////////////////////////////////////////////////////

class CReportSet
{
public:
	CReportSet(){};
	virtual ~CReportSet(){};
	CReportSet( const CString& _strDirect );

private:
	ReportTypeToNameMap m_mapReporTypeToName;
	CString m_strDirectoryName;			// path
public:
	void setDirectoryName( const CString& _strName ) { m_strDirectoryName = _strName; }
	const CString& getDirectoryName( void ) const { return m_strDirectoryName;	}
	void readFromFile( ArctermFile& file );
	void writeToFile( ArctermFile& file ) const ;

	int getReportItemCount( void ) const { return m_mapReporTypeToName.size(); }
	void addReportItem( int _reportType );
	void removeReportItem( int _reportType );
	bool getReportItem( int _reportType, CReportItem& _item );
	void getAllReortName( std::vector<CString>& _reportNames ) const ;	
	
	bool initReportSet( const CSimParameter* _pSimPara, InputTerminal* _pInTerm, const CString& _strPath );
	bool isReportRealExist( int _reportType ) const ;
	const std::map<ENUM_REPORT_TYPE, CString> getReportMap( void ) const;
	CReportItem* operator[]( int _type );
private:
};
#endif // !defined(AFX_REPORTITEM_H__C6F126FF_E81A_4B2D_9DE1_C624D0F9A272__INCLUDED_)
