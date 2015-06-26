#pragma once

//#include "..\AirsideInput\SQLExecutor.h"
#include "..\Common\Flt_cnst.h"
#include "InputAirsideAPI.h"

class InputTerminal;
class CAACompositeCat;
class CAACompCatManager;
class CAirsideExportFile;
class CAirsideImportFile;
class CAirportDatabase;
/////////////////////////////////////////////////////////////
//CTaxiOutboundNEC
/////////////////////////////////////////////////////////////
class INPUTAIRSIDE_API CTaxiOutboundNEC
{
public:
	CTaxiOutboundNEC();
	virtual ~CTaxiOutboundNEC(void);

public:
	int GetID() { return m_nID; }
	void SetID(int nID) { m_nID = nID; }

	int GetProjID() { return m_nProjID; }
	void SetProjID(int nID) { m_nProjID = nID; }

	FlightConstraint& GetFltType() { return m_fltType; }
	void SetFltType(const FlightConstraint& fltType) { m_fltType = fltType; }
	//CString& GetFltType() { return m_fltType; }
	//void SetFltType(const CString& fltType) { m_fltType = fltType; }

	float GetNormalSpeed() { return m_fNormalSpeed; }
	void SetNormalSpeed(float fValue) { m_fNormalSpeed = fValue; }

	float GetMaxSpeed() { return m_fMaxSpeed; }
	void SetMaxSpeed(float fValue) { m_fMaxSpeed = fValue; }

	float GetIntersectionBuffer() { return  m_fIntersectionBuffer; }
	void SetIntersectionBuffer(float fValue) { m_fIntersectionBuffer = fValue; }

	float GetMinSeparation() { return  m_fMinSeparation; }
	void SetMinSeparation(float fValue) { m_fMinSeparation = fValue; }

	float GetMaxSeparation() { return  m_fMaxSeparation; }
	void SetMaxSeparation(float fValue) { m_fMaxSeparation = fValue; }

	float GetFuelBurn() { return m_fFuelBurn; }
	void SetFuelBurn(float fValue) { m_fFuelBurn = fValue; }

	float GetCostPerHour() { return m_fCostPerHour; }
	void SetCostPerHour(float fValue) { m_fCostPerHour = fValue; }


protected:
	int m_nID;
	int m_nProjID;
	FlightConstraint m_fltType;
	//CString m_fltType;
	float m_fNormalSpeed;
	float m_fMaxSpeed;
	float m_fIntersectionBuffer;
	float m_fMinSeparation;
	float m_fMaxSeparation;
	float m_fFuelBurn;
	float m_fCostPerHour;
protected:
	float m_acceleration  ;
	float m_deceleration  ;
	float m_dLongitudinalSep;
	float m_dStaggeredSep;

public:
	float GetAcceleration() { return m_acceleration ;} ;
	void SetAcceleration(float fvalue) { m_acceleration = fvalue ;} ;
	float GetDeceleration() { return m_deceleration ;} ;
	void SetDeceleration(float fvalue) { m_deceleration = fvalue ;} ;

	float GetLongitudinalSeperation();
	void SetLongitudinalSeperation(float fvalue);
	float GetStaggeredSeperation();
	void SetStaggeredSeperation(float fvalue);

};

typedef std::vector<CTaxiOutboundNEC> TaxiOutboundNECList;

/////////////////////////////////////////////////////////////
//CTaxiOutbound
/////////////////////////////////////////////////////////////
class INPUTAIRSIDE_API CTaxiOutbound //: public CSQLExecutor
{
public:
	CTaxiOutbound(CAirportDatabase* pAirportDB);
	virtual ~CTaxiOutbound();
public:
	virtual void ReadData(int nProjID);

	void AddRecord(CTaxiOutboundNEC& toNEC);
	void DeleteRecord(CTaxiOutboundNEC& toNEC);
	void DeleteRecord(int nItem);
	void UpdateRecord(CTaxiOutboundNEC& toNEC);

	void AddToList(CTaxiOutboundNEC& toNEC);
	void DeleteFromList(CTaxiOutboundNEC& toNEC);
	void UpdateFromList(CTaxiOutboundNEC& toNEC);

public:
	int GetProjID() const { return m_nProjID; }
	void SetProjID(int nID) { m_nProjID = nID; }

	int GetCount() { return (int)m_vTaxiOutboundNEC.size(); }

	CTaxiOutboundNEC* GetRecordByID(int nID);
	CTaxiOutboundNEC* GetRecordByIdx(int nIdx);

	void SetInputTerminal(InputTerminal* pInputTerminal) { m_pInputTerminal = pInputTerminal; }

	void SortFlightType();

protected:
	virtual CString GetSelectScript(int nProjID) const;
	virtual CString GetUpdateScript() const;
	virtual CString GetInsertScript() const;
	virtual CString GetDeleteScript() const;
	virtual void Execute(){};

protected:
	int m_nProjID;
	InputTerminal*	m_pInputTerminal;
	TaxiOutboundNECList m_vTaxiOutboundNEC;
	CAirportDatabase* m_pAirportDB;
public:
	static void ImportTaxiOutbounds(CAirsideImportFile& importFile);
	static void ExportTaxiOutbounds(CAirsideExportFile& exportFile, CAirportDatabase* pAirportDB);
	void ExportData(CAirsideExportFile& exportFile);
	void ImportAddRecord(CTaxiOutboundNEC& toNEC,CString strFltType);

		double GetFuelBurnByFlightType(AirsideFlightDescStruct& _flttype) ;
};
