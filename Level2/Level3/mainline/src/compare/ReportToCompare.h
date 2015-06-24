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
class CReportToCompare  
{
public:
	CReportToCompare();
	
	CReportToCompare(const CReportToCompare& _other) { //*this=_other;}
		m_strName = _other.m_strName;
		m_nReportCategory = _other.m_nReportCategory;
		m_reportParam = _other.m_reportParam;
	}
	
	virtual ~CReportToCompare();

	const CString& GetName() const{return m_strName;}
	void SetName(const CString& strName){m_strName = strName;}

	void SetCategory(int nReportCategory) { m_nReportCategory = nReportCategory; }
	int GetCategory() const { return m_nReportCategory; }

	void SetParameter(const CReportParamToCompare& reportParam) { m_reportParam = reportParam; }
	const CReportParamToCompare& GetParameter() const { return m_reportParam; }

	BOOL LoadData( const CString& strPath );
	
	void DeleteModelParameter(const CString& strModelUniqueName);

	CReportToCompare& operator= ( const CReportToCompare& _rhs )
	{
		m_strName = _rhs.m_strName;
		m_nReportCategory = _rhs.m_nReportCategory;
		m_reportParam = _rhs.m_reportParam;
		return *this;
	}

	std::vector<CString>	m_vstrOutput;

private:
	int						m_nReportCategory;
	CString					m_strName;
	CReportParamToCompare	m_reportParam;
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
	virtual const char* getHeaders() const{return "Name, Category, Start, End, Interval, Passange Type, Processor Type";}
	
	
	int GetReports(OUT std::vector<CReportToCompare>& vReports);
	void SetReports(const std::vector<CReportToCompare>& vReports);

	void SetModels(const std::vector<CModelToCompare *>& vModel );
	CModelToCompare* GetModelByUniqueName(const CString& strUniqueName);
	
protected:
	std::vector<CReportToCompare>	m_vReports;
	std::vector<CModelToCompare *>  m_vModel;
};

#endif // !defined(AFX_REPORTTOCOMPARE_H__F52B6C8D_A668_4D02_9C6E_719C285CD02A__INCLUDED_)
