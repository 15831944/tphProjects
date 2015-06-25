// ReportParamToCompare.h: interface for the ReportParamToCompare class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REPORTPARAMTOCOMPARE_H__835467C2_0BF7_4976_AC30_52988D1DC5FB__INCLUDED_)
#define AFX_REPORTPARAMTOCOMPARE_H__835467C2_0BF7_4976_AC30_52988D1DC5FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\common\elaptime.h"
#include <vector>
#include "../Inputs/MobileElemConstraint.h"
#include <CommonData/procid.h>
#include "../Reports/ReportParameter.h"

typedef enum 
{
	REPORT_TYPE_DETAIL = 0,
	REPORT_TYPE_SUMMARY = 1
} ENUM_REPORT_DETAIL;

class CModelToCompare;
class CModelParameter
{
public:
	CModelParameter(){}
	~CModelParameter (){}

public:
	const CString& GetModelUniqueName() const { return m_strUniqueModelName;}
	void SetModelUniqueName(const CString& strUniqueName){ m_strUniqueModelName = strUniqueName;}

	void SetPaxType(const std::vector<CMobileElemConstraint>& vPaxType);
	void SetArea(const CString& sArea){m_sArea = sArea;}
	
	const CString& GetArea() const{return m_sArea;}
	int GetPaxType(std::vector<CMobileElemConstraint>& vPaxType);

	//from---to processors	
	void GetFromToProcs(  CReportParameter::FROM_TO_PROCS& _fromToProcs )const	{ _fromToProcs = m_fromToProcs;};
	void SetFromToProcs ( const CReportParameter::FROM_TO_PROCS& _fromToProcs ){ m_fromToProcs = _fromToProcs;};
	
	int GetProcessorID(std::vector<ProcessorID>& vProcGroups);	
	

	void SetProcessorID(const std::vector<ProcessorID>& vProcGroups);
	CModelParameter& operator = ( const CModelParameter& _rhs )
	{
		m_vPaxType = _rhs.m_vPaxType;
		m_vProcGroups = _rhs.m_vProcGroups;
		m_sArea = _rhs.m_sArea;
		m_fromToProcs = _rhs.m_fromToProcs;

		return *this;
	}

	void InitDefaultParameter(const CString& strProjName,CModelToCompare * pModel);

protected:
private:
	CString m_sArea;
	CReportParameter::FROM_TO_PROCS m_fromToProcs;
	std::vector<CMobileElemConstraint>	m_vPaxType;
	std::vector<ProcessorID>			m_vProcGroups;
	
	CString m_strUniqueModelName;
};

class CReportParamToCompare  
{
public:
	CReportParamToCompare();
	CReportParamToCompare(const CReportParamToCompare& _other) { *this=_other;}
	virtual ~CReportParamToCompare();
	
	void SetStartTime(const ElapsedTime& tStart) { m_tStart = tStart; }
	void SetEndTime(const ElapsedTime& tEnd) { m_tEnd = tEnd; }
	void SetInterval(const ElapsedTime& tInterval) { m_tInterval = tInterval; }
	void SetInterval(long interval){ m_lInterval = interval;}
	
	void SetPassagerType(int nPassagerType) { m_nPassagerType = nPassagerType; }
	
	
	const ElapsedTime& GetStartTime() const { return m_tStart; }
	const ElapsedTime& GetEndTime() const { return m_tEnd; }
	const ElapsedTime& GetInterval() const { return m_tInterval; }
	void GetInterval( long& lInterval) const{ lInterval =  m_lInterval;}

	int GetPassagerType() const { return m_nPassagerType; }
// 	void SetSubType(int nSubType){m_nSubType = nSubType ;}
// 	int GetSubType(){ return m_nSubType;}
	
	BOOL LoadData(const CString& strPath);

	CReportParamToCompare& operator = ( const CReportParamToCompare& _rhs )
	{
		m_tStart = _rhs.m_tStart;
		m_tEnd = _rhs.m_tEnd;
		m_tInterval = _rhs.m_tInterval;
		
		m_nPassagerType = _rhs.m_nPassagerType;
		
		m_vModelParam = _rhs.m_vModelParam;
		m_lInterval =_rhs.m_lInterval;
		m_nType = _rhs.m_nType;
		return *this;
	}

	
	
	void SetModelParameter(std::vector<CModelParameter>& vModelParameter);
	void GetModelParameter(std::vector<CModelParameter>& vModelParam);
	int GetModelParameterCount();

	ENUM_REPORT_DETAIL  GetReportDetail() const { return m_nType; }
	void SetReportDetail(ENUM_REPORT_DETAIL nType) { m_nType = nType; }

	bool GetModelParamByModelName(const CString& strUniqueName,CModelParameter& modelParam) const;
	void DeleteModelParameter(const CString& strModelUniqueName);
private:
	ElapsedTime m_tStart;
	ElapsedTime m_tEnd;
	ElapsedTime m_tInterval;
	int m_nPassagerType;
	long m_lInterval;
	ENUM_REPORT_DETAIL		m_nType;
//	int   m_nSubType;
	std::vector<CModelParameter> m_vModelParam;
};

#endif // !defined(AFX_REPORTPARAMTOCOMPARE_H__835467C2_0BF7_4976_AC30_52988D1DC5FB__INCLUDED_)
