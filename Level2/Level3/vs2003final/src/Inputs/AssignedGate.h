#pragma once
#include <CommonData/procid.h>
#include "FLIGHT.H"
#include "GateAssignmentConstraints.h"
#include "Stand2GateConstraint.h"
#include "FlightForGateAssign.h"

class ParkingStandBufferList;
class ALTObjectID;

class INPUTS_TRANSFER CAssignGate
{
public:
	virtual void GetToolTips(int _nIdx, CString& _csToolTips, StringDictionary* _pStrDict );
	virtual bool BeAbletoTake(ARCFlight* pflight,const ElapsedTime& tStart, const ElapsedTime& tEnd, ParkingStandBufferList* pStandBufferList);
	virtual bool OverlapStandErrorMessage(ARCFlight* pflight,const ElapsedTime& tStart, const ElapsedTime& tEnd,CString& strError);
	virtual bool StandBufferErrorMessage(ARCFlight* pflight,const ElapsedTime& tStart, const ElapsedTime& tEnd, ParkingStandBufferList* pStandBufferList,CString& strError);
	virtual void RemoveSelectedFlight(std::vector<CFlightOperationForGateAssign>& _vectFlight);
	virtual void RemoveAssignedFlight(CFlightOperationForGateAssign& pFlight);
	void SetFlightSelected( int _nIndex, bool _bSel );
	void AddFlight( CFlightOperationForGateAssign& _flight);
	int GetFlightIdx(CFlightOperationForGateAssign& _flight);
	virtual void SaveFlightAssignment();
	CFlightOperationForGateAssign& GetFlight( int _nIndex );
	const CFlightOperationForGateAssign& GetFlight( int _nIndex ) const;
	void RemoveFlight( int _nIndex );
	int GetFlightCount(void);
	virtual CString GetName(void);

	//to get assigned flights in the time range
	void GetFlightsInTimeRange(ElapsedTime tStart, ElapsedTime tEnd, std::vector<ARCFlight*>& vFlights);

	CAssignGate();
	virtual ~CAssignGate(void);

	/////////////////need del
	//bool SetFlightArrTime( int _idx, ElapsedTime _arrTime );
	//bool SetFlightDepTime( int _idx, ElapsedTime _depTime );
	//bool setFlightServeTime( int _idx, ElapsedTime _serveTime );
	//////////////////

	void SetParent(CGateAssignmentMgr * _pParent);

	virtual bool isLess(CAssignGate* pObj){return false;}
protected:
	CGateAssignmentMgr * m_pParent;
	std::vector<CFlightOperationForGateAssign> m_vectAssignedFlight;

};

class INPUTS_TRANSFER CAssignStand : public CAssignGate
{
public:
	CAssignStand();
	virtual ~CAssignStand(void);
public:
	virtual void SaveFlightAssignment();
	virtual void RemoveSelectedFlight(std::vector<CFlightOperationForGateAssign>& _vectFlight);
	virtual void RemoveAssignedFlight(CFlightOperationForGateAssign& pFlight);
	virtual void GetToolTips(int _nIdx, CString& _csToolTips, StringDictionary* _pStrDict );
	const ALTObjectID& GetStandID(void)const{ return m_StandID; }
	virtual CString GetName(void);
	void SetALTObjectID(const ALTObjectID& _objID){ m_StandID = _objID; }

	bool operator < (const CAssignStand& _gate) const;
	bool operator > (const CAssignStand& _gate) const;
	virtual bool BeAbletoTake(ARCFlight* pflight,const ElapsedTime& tStart, const ElapsedTime& tEnd, ParkingStandBufferList* pStandBufferList);
	virtual bool OverlapStandErrorMessage(ARCFlight* pflight,const ElapsedTime& tStart, const ElapsedTime& tEnd,CString& strError);
	virtual bool StandBufferErrorMessage(ARCFlight* pflight,const ElapsedTime& tStart, const ElapsedTime& tEnd, ParkingStandBufferList* pStandBufferList,CString& strError);
	virtual bool isLess(CAssignGate* pObj);
	int GetStandIdx();
	void SetStandIdx(int idx);

protected:
	ALTObjectID m_StandID; 
	int m_nStandIdx;
};

class INPUTS_TRANSFER CAssignTerminalGate : public CAssignGate
{
public:
	CAssignTerminalGate();
	virtual ~CAssignTerminalGate(void);
public:
	virtual void SaveFlightAssignment();

	virtual void RemoveSelectedFlight(std::vector<CFlightOperationForGateAssign>& _vectFlight);
	virtual void RemoveAssignedFlight(CFlightOperationForGateAssign& pFlight);

	virtual void GetToolTips(int _nIdx, CString& _csToolTips, StringDictionary* _pStrDict );

	const ProcessorID& GetProcessID(void){return (m_GateID);}

	virtual CString GetName(void);

	void SetProcessorID( const ProcessorID& _processorID ){m_GateID = _processorID; }
	bool BeAbletoTake(ARCFlight* pflight,const ElapsedTime& tStart, const ElapsedTime& tEnd, ParkingStandBufferList* pStandBufferList);
	virtual bool isLess(CAssignGate* pObj);

protected:
	ProcessorID m_GateID;
};