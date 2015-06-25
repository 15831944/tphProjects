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

class CModelToCompare;
class MobConstWithCheckedFlag
{
public:
	MobConstWithCheckedFlag(){ m_paxType.initDefault(); m_isChecked = TRUE; }
	~MobConstWithCheckedFlag(){}

public:
	BOOL GetChecked(){ return m_isChecked; }
	void SetChecked(BOOL val) { m_isChecked = val; }
	CMobileElemConstraint GetPaxType() { return m_paxType; }
	void SetPaxType(CMobileElemConstraint val) { m_paxType = val; }
	MobConstWithCheckedFlag& operator = ( const MobConstWithCheckedFlag& other )
	{
		m_paxType = other.m_paxType;
		m_isChecked = other.m_isChecked;
		return *this;
	}
private:
	CMobileElemConstraint m_paxType;
	BOOL m_isChecked;
};

class ProcessIDWithCheckedFlag
{
public:
	ProcessIDWithCheckedFlag(){ m_procID.init(); m_isChecked = TRUE; }
	~ProcessIDWithCheckedFlag(){}

public:
	BOOL GetChecked(){ return m_isChecked; }
	void SetChecked(BOOL val) { m_isChecked = val; }
	ProcessorID GetProcID() { return m_procID; }
	void SetProcID(ProcessorID procID) { m_procID = procID; }
	ProcessIDWithCheckedFlag& operator = ( const ProcessIDWithCheckedFlag& other )
	{
		m_procID = other.m_procID;
		m_isChecked = other.m_isChecked;
		return *this;
	}
private:
	ProcessorID m_procID;
	BOOL m_isChecked;
};
class CModelParameter
{
public:
	CModelParameter(){}
	~CModelParameter (){}

public:
	const CString& GetModelUniqueName() const { return m_strUniqueModelName;}
	void SetModelUniqueName(const CString& strUniqueName){ m_strUniqueModelName = strUniqueName;}
	void SetArea(const CString& sArea){m_sArea = sArea;}
	
	const CString& GetArea() const{return m_sArea;}

	// passenger types
	std::vector<MobConstWithCheckedFlag>& GetPaxTypeWithCheckedFlagList(){ return m_vPaxType; }
	void GetPaxTpyeList(std::vector<CMobileElemConstraint>& vPaxTpye);
	int GetPaxTypeCount(){ return (int)m_vPaxType.size(); }
	void AddPaxType(MobConstWithCheckedFlag item){ m_vPaxType.push_back(item); }
	void AddPaxType(CMobileElemConstraint mobConst, BOOL isChecked = TRUE);
	void SetPaxTypeChecked(int nIndex, BOOL state = TRUE){ m_vPaxType[nIndex].SetChecked(state); }
	BOOL IsPaxTypeChecked(int nIndex){ return m_vPaxType[nIndex].GetChecked(); }
	void DeletePaxTypeByIndex(int nIndex){ m_vPaxType.erase(m_vPaxType.begin() + nIndex); };
	BOOL IsPaxTypeExist(CMobileElemConstraint paxType);

	// processor IDs
	std::vector<ProcessIDWithCheckedFlag>& GetProcIDWithCheckedFlagGroup(){ return m_vProcGroup; }
	void GetProcIDGroup(std::vector<ProcessorID>& vProcGroups);
	int GetProcIDCount(){ return (int)m_vProcGroup.size(); }
	void AddProcID(ProcessIDWithCheckedFlag item){ m_vProcGroup.push_back(item); }
	void AddProcID(ProcessorID procID, BOOL isChecked = TRUE);
	void SetProcIDChecked(int nIndex, BOOL state = TRUE){ m_vProcGroup[nIndex].SetChecked(state); }
	BOOL IsProcIDChecked(int nIndex){ return m_vProcGroup[nIndex].GetChecked(); }
	void DeleteProcIDByIndex(int nIndex){ m_vProcGroup.erase(m_vProcGroup.begin() + nIndex); };
	BOOL IsProcIDExist(ProcessorID procID);

	//from---to processors	
	void GetFromToProcs(  CReportParameter::FROM_TO_PROCS& _fromToProcs )const	{ _fromToProcs = m_fromToProcs; }
	void SetFromToProcs ( const CReportParameter::FROM_TO_PROCS& _fromToProcs ){ m_fromToProcs = _fromToProcs; }
	
	CModelParameter& operator = ( const CModelParameter& _rhs )
	{
		m_vPaxType = _rhs.m_vPaxType;
		m_vProcGroup = _rhs.m_vProcGroup;
		m_sArea = _rhs.m_sArea;
		m_fromToProcs = _rhs.m_fromToProcs;

		return *this;
	}

	void InitDefaultParameter(const CString& strProjName,CModelToCompare * pModel);

private:
	CString m_sArea;
	CReportParameter::FROM_TO_PROCS m_fromToProcs;
	std::vector<MobConstWithCheckedFlag>	m_vPaxType;
	std::vector<ProcessIDWithCheckedFlag>	m_vProcGroup;
	
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
	//void SetReportType(int nReportType) { m_nReportType = nReportType; }
	//void SetThreshold(int nThreshold) { m_nThreshold = nThreshold; }
	void SetPassagerType(int nPassagerType) { m_nPassagerType = nPassagerType; }
	//void SetReportName(const CString& strReportName)
	//{ m_strReportName = strReportName; }
	/*void SetReportParamter(const ElapsedTime& tStart, const ElapsedTime& tEnd, 
		int nInterval, int nReportType, 
		int nThreshold, int nPassagerType, const CString& strReportName);*/
//	void SetArea(const CString& sArea){m_sArea = sArea;}
	
	const ElapsedTime& GetStartTime() const { return m_tStart; }
	const ElapsedTime& GetEndTime() const { return m_tEnd; }
	const ElapsedTime& GetInterval() const { return m_tInterval; }
	 void GetInterval( long& lInterval) const{ lInterval =  m_lInterval;}
//	const CString& GetArea() const{return m_sArea;}
	//int GetReportType() const { return m_nReportType; }
	//int GetThreshold() const { return m_nThreshold; }
	int GetPassagerType() const { return m_nPassagerType; }
	//const CString& GetReportName() const { return m_strReportName; }
	/*void GetReportParamter(ElapsedTime& tStart, ElapsedTime& tEnd, 
		int& nInterval, int& nReportType, 
		int& nThreshold, int& nPassagerType, CString& strReportName) const;*/

	//from---to processors	
	//void GetFromToProcs(  CReportParameter::FROM_TO_PROCS& _fromToProcs )const	{ _fromToProcs = m_fromToProcs;};
	//void SetFromToProcs ( const CReportParameter::FROM_TO_PROCS& _fromToProcs ){ m_fromToProcs = _fromToProcs;};
	
	BOOL LoadData(const CString& strPath);

	CReportParamToCompare& operator = ( const CReportParamToCompare& _rhs )
	{
		m_tStart = _rhs.m_tStart;
		m_tEnd = _rhs.m_tEnd;
		m_tInterval = _rhs.m_tInterval;
		//m_nReportType = _rhs.m_nReportType;
		//m_nThreshold = _rhs.m_nThreshold;
		m_nPassagerType = _rhs.m_nPassagerType;
		//m_strReportName = _rhs.m_strReportName;

		//m_vPaxType = _rhs.m_vPaxType;
		//m_vProcGroups = _rhs.m_vProcGroups;
		//m_sArea = _rhs.m_sArea;
		//m_fromToProcs = _rhs.m_fromToProcs;
		m_vModelParam = _rhs.m_vModelParam;
		m_lInterval =_rhs.m_lInterval; 
		return *this;
	}

	//void SetPaxType(const std::vector<CMobileElemConstraint>& vPaxType);
	//void SetProcessorID(const std::vector<ProcessorID>& vProcGroups);
	//
	//int GetPaxType(OUT std::vector<CMobileElemConstraint>& vPaxType);
	//const std::vector<CMobileElemConstraint>& GetPaxType() const{return m_vPaxType;}
	//int GetProcessorID(OUT std::vector<ProcessorID>& vProcGroups);
	//const std::vector<ProcessorID>& GetProcessorID() const {return m_vProcGroups;}
	
	void SetModelParameter(std::vector<CModelParameter>& vModelParameter);
	std::vector<CModelParameter>& GetModelParameter();
	const std::vector<CModelParameter>& GetModelParameterConst() const { return m_vModelParam; }
	int GetModelParameterCount();
	const int GetModelParameterCountConst() const { return (int)m_vModelParam.size(); };

	bool GetModelParamByModelName(const CString& strUniqueName,CModelParameter& modelParam) const;
	void DeleteModelParameter(const CString& strModelUniqueName);
private:
	ElapsedTime m_tStart;
	ElapsedTime m_tEnd;
	ElapsedTime m_tInterval;
	//int m_nReportType;
	//int m_nThreshold;
	int m_nPassagerType;

	long m_lInterval;
	//CString m_strReportName;

	//CString m_sArea;
	//CReportParameter::FROM_TO_PROCS m_fromToProcs;
	//std::vector<CMobileElemConstraint>	m_vPaxType;
	//std::vector<ProcessorID>			m_vProcGroups;

	std::vector<CModelParameter> m_vModelParam;
};

#endif // !defined(AFX_REPORTPARAMTOCOMPARE_H__835467C2_0BF7_4976_AC30_52988D1DC5FB__INCLUDED_)
