#pragma once
//#include "..\AirsideInput\SQLExecutor.h"
#include "..\Common\Flt_cnst.h"

#include "InputAirsideAPI.h"

class CAACompositeCat;
class CAACompCatManager;
class InputTerminal;
class CAirsideExportFile;
class CAirsideImportFile;
class CAirportDatabase;
/////////////////////////////////////////////////////////////
//CTaxiInboundNEC
/////////////////////////////////////////////////////////////
class INPUTAIRSIDE_API CTaxiInboundNEC
{
public:
	CTaxiInboundNEC();
	virtual ~CTaxiInboundNEC();

public:
	int GetID() { return m_nID; }
	void SetID(int nID) { m_nID = nID; }

	int GetProjID() { return m_nProjID; }
	void SetProjID(int nID) { m_nProjID = nID; }

	// flight type
	FlightConstraint& GetFltType() { return m_fltType; }
	void SetFltType(const FlightConstraint& fltType) { m_fltType = fltType; }
	//CString& GetFltType() { return m_fltType; }
	//void SetFltType(const CString fltType) { m_fltType = fltType; }

	float GetNormalSpeed() { return m_fNormalSpeed; }
	void SetNormalSpeed(float fValue) { m_fNormalSpeed = fValue; }

	float GetMaxSpeed() { return m_fMaxSpeed; }
	void SetMaxSpeed(float fValue) { m_fMaxSpeed = fValue; }

	float GetIntersectionBuffer() { return  m_fIntersectionBuffer; }
	void SetIntersectionBuffer(float fValue) { m_fIntersectionBuffer = fValue; }

	float GetFuelBurn() { return m_fFuelBurn; }
	void SetFuelBurn(float fValue) { m_fFuelBurn = fValue; }

	float GetCostPerHour() { return m_fCostPerHour; }
	void SetCostPerHour(float fValue) { m_fCostPerHour = fValue; }

protected:
	int m_nID;
	int m_nProjID;
	FlightConstraint	m_fltType;
	//CString m_fltType;
	float m_fMaxSpeed;
	float m_fNormalSpeed;
	float m_fIntersectionBuffer;
	float m_fFuelBurn;
	float m_fCostPerHour;
protected:
	float m_fMinSeparation;
	float m_fMaxSeparation;
public:
	float GetMinSeparation() { return  m_fMinSeparation; }
	void SetMinSeparation(float fValue) { m_fMinSeparation = fValue; }

	float GetMaxSeparation() { return  m_fMaxSeparation; }
	void SetMaxSeparation(float fValue) { m_fMaxSeparation = fValue; }
protected:
	float m_acceleration  ;
	float m_deceleration  ;
	float m_dLongitudinalSep;
	float m_dStaggeredSep;

public:
	float GetAcceleration() { return m_acceleration ;} 
	void SetAcceleration(float fvalue) { m_acceleration = fvalue ;} 
	float GetDeceleration() { return m_deceleration ;} 
	void SetDeceleration(float fvalue) { m_deceleration = fvalue ;} 

	float GetLongitudinalSeperation();
	void SetLongitudinalSeperation(float fvalue);
	float GetStaggeredSeperation();
	void SetStaggeredSeperation(float fvalue);

};

typedef std::vector<CTaxiInboundNEC> TaxiInboundNECList;

/////////////////////////////////////////////////////////////
//CTaxiInbound
/////////////////////////////////////////////////////////////
class INPUTAIRSIDE_API CTaxiInboundNECManager// : public CSQLExecutor
{
public:
	CTaxiInboundNECManager();
	virtual ~CTaxiInboundNECManager();
public:
	virtual void ReadData(CAirportDatabase* pAirportDB);
	virtual void SaveData();
	virtual void DeleteData();

	CTaxiInboundNEC* GetRecordByID(int nID);
	CTaxiInboundNEC* GetRecordByIdx(int nIdx);

	void AddRecord(CTaxiInboundNEC& tiNEC);
	void DeleteRecord(int nItem);
	void UpdateItem(CTaxiInboundNEC& tiNEC);

	 int GetCount() { return (int)m_vTaxiInboundNEC.size(); }

public:
	void SetProjID(int nProjID) { m_nProjID = nProjID; }
	int  GetProjID(){	return m_nProjID; };

	void SetCompCatManager(CAACompCatManager* pManager) { m_pCompCatManager = pManager; }
	CAACompCatManager* GetCompCatManager() { return m_pCompCatManager; }
	void SetInputTerminal(InputTerminal* pInputTerminal) { m_pInputTerminal = pInputTerminal; }
	void SortFlightType();
protected:
	virtual CString GetSelectScript() const;
	virtual CString GetUpdateScript() const;
	virtual CString GetInsertScript() const;
	virtual CString GetDeleteScript() const;
	virtual void Execute(){};
protected:
	int m_nProjID;
	InputTerminal*	m_pInputTerminal;
	TaxiInboundNECList m_vTaxiInboundNEC;

	CAACompCatManager* m_pCompCatManager;

public:

	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);
	void ImportAddRecord(CTaxiInboundNEC& tiNEC,CString strFltType);
};

/////////////////////////////////////////////////////////////
//CTaxiInbound
/////////////////////////////////////////////////////////////
class INPUTAIRSIDE_API CTaxiInbound //: public CSQLExecutor
{
public:
	CTaxiInbound(CAirportDatabase* pAirportDB);
	CTaxiInbound(CAACompCatManager* pManager,bool flag);
	virtual ~CTaxiInbound(void);

public:
	virtual void ReadData(int nProjID);
	virtual void SaveData();
	virtual void DeleteData();

	CTaxiInboundNEC* GetNECDataByID(int nNECID);

	void AddRecord(CTaxiInboundNEC& tiNEC);
	void DeleteRecord(int nItem);
public:
	int GetID() const { return m_nID; }
	void SetID(int nID) { m_nID = nID; }

	int GetProjID() const { return m_nProjID; }
	void SetProjID(int nID) { m_nProjID = nID; }

	float GetValueOfTaxiway() const { return m_fTaxiway; }
	void SetValueOfTaxiway(float fValue) { m_fTaxiway = fValue; }

	float GetValueOfRunway() const { return m_fRunway; }
	void SetValueOfRunway(float fValue) { m_fRunway = fValue; }

	CTaxiInboundNECManager* GetTaxiInboundNECList() { return &m_TaxiInboundNECManager; }

	void SetCompCatManager(CAACompCatManager* pManager) { m_pCompCatManager = pManager; }
	CAACompCatManager* GetCompCatManager() { return m_pCompCatManager; }

	void SetInputTerminal(InputTerminal* pInputTerminal) { m_TaxiInboundNECManager.SetInputTerminal(pInputTerminal); }

protected:
	virtual CString GetSelectScript(int nProjID) const;
	virtual CString GetUpdateScript() const;
	virtual CString GetInsertScript() const;
	virtual CString GetDeleteScript() const;
	virtual void Execute(){};
protected:
	int m_nID;
	int m_nProjID;
	float m_fTaxiway;
	float m_fRunway;
	CTaxiInboundNECManager m_TaxiInboundNECManager;

	CAACompCatManager* m_pCompCatManager;
	CAirportDatabase* m_pAirportDB;

public:
	static void ImportTaxiInbounds(CAirsideImportFile& importFile);
	static void ExportTaxiInbounds(CAirsideExportFile& exportFile, CAirportDatabase* pAirportDB);
	void ImportData(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);

	double GetFuelBurnByFlightType(AirsideFlightDescStruct& _flttype) ;
};
