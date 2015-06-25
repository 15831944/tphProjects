#pragma once
#include "parameters.h"
class ARCEventLog ;
class ResourceDesc;
class AIRSIDEREPORT_API CReportTaxiwaySegment
{
public:
	int m_FormNodeID ;
	int m_ToNodeID ;
	CString m_strFromName ;
	CString m_strToName ;
	double m_dFromDist;
	double m_dToDist;

	CReportTaxiwaySegment(int _fromID , int _toID);
	void InitNodeDist(int nTaxiwayID);
};
class AIRSIDEREPORT_API CTaxiwaySelect
{
protected:
	int m_TaxiwayID ;
	CString m_taxiWayName ;
public:
	typedef std::vector<CReportTaxiwaySegment>::iterator TY_DATA_ITER ;
protected:
	std::vector<CReportTaxiwaySegment> m_vData ;
public:
	CTaxiwaySelect() ;
	void AddIntersectionPair(int _fromID , const CString& _from ,int _toId , const CString& _to) ;
	void SetTaxiwayID(int _taxiid) { m_TaxiwayID = _taxiid ;} ;
	int GetTaxiwayID() { return m_TaxiwayID ;} ;

	CString GetTaxiwayName() { return m_taxiWayName ;} ;
	void SetTaxiwayName(const CString& _name) { m_taxiWayName = _name ;} ;
	std::vector<CReportTaxiwaySegment>* GetIntersectionSegment( ) { return &m_vData ;} ;
public:
	BOOL IsSegInParaDefine(double delayFromDis,double delayToDis) ;
};
class AIRSIDEREPORT_API CTaxiwayDelayParameters :
	public CParameters
{
public:
	CTaxiwayDelayParameters(void);
	~CTaxiwayDelayParameters(void) { cleardata() ;};
public:
	void LoadParameter() {};
	void InitParameter(CXTPPropertyGrid * pGrid){} ;
	void UpdateParameter() {} ;

	//write and load parameter from file
	virtual void WriteParameter(ArctermFile& _file);
	virtual void ReadParameter(ArctermFile& _file);
	virtual CString GetReportParamName();
public:
	typedef std::vector<CTaxiwaySelect*>  TY_TAXIWAY_DATA ;
	typedef TY_TAXIWAY_DATA::iterator TY_TAXIWAY_DATA_ITER ;
	enum subType { ARRIVAL ,DEPARTURE,TOTAL} ;
protected:
	std::vector<CTaxiwaySelect*> m_TaxiwayData ;
	std::vector<CTaxiwaySelect*>m_vTaxiwayDataForShow;
public:
	CTaxiwaySelect* AddTaxiway(int _taxiwayid) ;
	CTaxiwaySelect*  AddTaxiwayForShow(int _taxiwayid);
	CTaxiwaySelect* GetTaxiwayForShowItem(int nIdx) {ASSERT(nIdx >= 0 && nIdx < GetCount()); return m_vTaxiwayDataForShow[nIdx];}
	void ClearTaxiwayDataForShow(){m_vTaxiwayDataForShow.clear();}
	int GetCount()const{return (int)m_vTaxiwayDataForShow.size();}
	TY_TAXIWAY_DATA* GetTaxiwayData() { return &m_TaxiwayData ;} ;
public:
	void cleardata() ;
public:
	//if no taxiway defined , return TRUE .
	//find the taixway by taxiway id , find it return true else return FALSE.
	//_FindResult , the point which find . if do not find or has no taxiway beed defined , it will be set to NULL . (out)
	BOOL CheckTheTaxiWayIsSelect(int _taxiID,CTaxiwaySelect** _FindResult) ;
public:
public:
	 BOOL ExportFile(ArctermFile& _file) ;
	 BOOL ImportFile(ArctermFile& _file) ;
};
