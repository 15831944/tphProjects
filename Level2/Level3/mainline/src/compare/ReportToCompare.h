// ReportToCompare.h: interface for the CReportToCompare class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REPORTTOCOMPARE_H__F52B6C8D_A668_4D02_9C6E_719C285CD02A__INCLUDED_)
#define AFX_REPORTTOCOMPARE_H__F52B6C8D_A668_4D02_9C6E_719C285CD02A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "ReportParamToCompare.h"
#include "../common/dataset.h"
#include <vector>

//using namespace std;
class CModelToCompare;
class CCmpReportParameter;
class CModelsManager;

class CReportToCompare  
{
public:
	CReportToCompare();
	
	CReportToCompare(const CReportToCompare& _other) { //*this=_other;}
		m_strName = _other.m_strName;
		m_nReportCategory = _other.m_nReportCategory;
		m_reportParam = _other.m_reportParam;
		m_isChecked = _other.m_isChecked;
		m_nType = _other.m_nType;
	}
	
	virtual ~CReportToCompare();

	const CString& GetName() const{return m_strName;}
	void SetName(const CString& strName){m_strName = strName;}

	void SetCategory(int nReportCategory) { m_nReportCategory = nReportCategory; }
	int GetCategory() const { return m_nReportCategory; }

	void SetParameter(const CReportParamToCompare& reportParam) { m_reportParam = reportParam; }
	const CReportParamToCompare& GetParameter() const { return m_reportParam; }
	BOOL GetChecked() const { return m_isChecked; }
	void SetChecked(BOOL val) { m_isChecked = val; }

	int  GetReportType() const { return m_nType; }
	void SetReportType(int nType) { m_nType = nType; }

	BOOL LoadData( const CString& strPath );
	
	void DeleteModelParameter(const CString& strModelUniqueName);

	CReportToCompare& operator= ( const CReportToCompare& _rhs )
	{
		m_strName = _rhs.m_strName;
		m_nReportCategory = _rhs.m_nReportCategory;
		m_reportParam = _rhs.m_reportParam;
		m_nType = _rhs.m_nType;
		return *this;
	}

	std::vector<CString>	m_vstrOutput;

private:
	int						m_nReportCategory;
	CString					m_strName;
	CReportParamToCompare	m_reportParam;
	BOOL					m_isChecked;
	int						m_nType; // 0: detail  1: summary
};


class CReportToCompareDataSet : public DataSet
{
public:
	CReportToCompareDataSet();
	virtual ~CReportToCompareDataSet();
	
	virtual void clear();
	virtual void readData(ArctermFile& p_file);
	virtual void readObsoleteData(ArctermFile& p_file);
	virtual void writeData(ArctermFile& p_file) const;
	virtual const char* getTitle() const{return "Report Information";}
	virtual const char* getHeaders() const{return "Name, Category, Start, End, Interval, Passenger Type, Processor Type";}
	
	
	int GetReports(OUT std::vector<CReportToCompare>& vReports);
	void SetReports(const std::vector<CReportToCompare>& vReports);
	int GetReportCount(){ return (int)m_vReports.size(); }

	void SetInputParam(CModelsManager *pModelsManager );
	CModelToCompare* GetModelByUniqueName(const CString& strUniqueName);
	
protected:
	std::vector<CReportToCompare>	m_vReports;
	CModelsManager *m_pModelsManager;
};

#endif // !defined(AFX_REPORTTOCOMPARE_H__F52B6C8D_A668_4D02_9C6E_719C285CD02A__INCLUDED_)
