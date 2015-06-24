// PaxDirecton.h: interface for the CPaxDirecton class.
#pragma once
#include "MobileElemConstraint.h"
#include <CommonData/procid.h>
#include "Common\ALTObjectID.h"
#include "Engine\LandsideTerminalLinkageInSim.h"


class InputTerminal;

class CPaxDirection  
{
private:
	CMobileElemConstraint m_paxType;
	std::vector<ProcessorID> m_vCanGoToProcessor;
	
	//landside object
	std::vector<LandsideObjectLinkageItemInSim> m_vCanGoToObject;


public:
	CPaxDirection();
	CPaxDirection( const CMobileElemConstraint _paxType);
	virtual ~CPaxDirection();

public:
	std::vector<ProcessorID>& GetCanGoToProcessor() { return m_vCanGoToProcessor; }
	bool IsThisPaxType( const CMobileElemConstraint& _paxType ){ return m_paxType == _paxType ? true : false;	}
	void AddDestProc( const ProcessorID& _procID );
	bool operator < ( const CPaxDirection& _another )const { return m_paxType < _another.m_paxType ? true : false;	}
	bool operator != (const CPaxDirection& _another) const;
	const CMobileElemConstraint& GetPaxType()const { return  m_paxType;}
	//const std::vector<ProcessorID>& GetDest()const { return m_vCanGoToProcessor;}

	bool IfExistInDest( const ProcessorID& _testID ,InputTerminal* _pTerm )const;


	//for landside 
	std::vector<LandsideObjectLinkageItemInSim>& GetCanGoToObject() { return m_vCanGoToObject; }
	void AddDestProc( const LandsideObjectLinkageItemInSim& _procID );
	bool IfExistInDest( const LandsideObjectLinkageItemInSim& _testID ,InputTerminal* _pTerm )const;
	bool IfExistInDest( const std::vector<LandsideObjectLinkageItemInSim>& _vDestProcID ,InputTerminal* _pTerm )const;

};
