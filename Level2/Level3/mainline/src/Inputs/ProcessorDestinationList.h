// ProcessorDestinationList.h: interface for the CProcessorDestinationList class.
#pragma once
#include "FlowDestination.h"
#include "Direction.h"

typedef std::vector<CFlowDestination> FLOWDESTVECT;

class CProcessorDestinationList  
{
private:
	ProcessorID m_procID;
	CDirection m_Dircetion;
	CDirection m_ReclaimDircetion;//for lead to reclaim;
	FLOWDESTVECT m_vDestProcs;
	InputTerminal* m_pTerm;
	bool m_bVisitFlag;
public:
	CProcessorDestinationList( InputTerminal* _pTerm );
	virtual ~CProcessorDestinationList();
	//CProcessorDestinationList( const CProcessorDestinationList& _anotherFlowPair );
	//CProcessorDestinationList& operator = ( const CProcessorDestinationList& _anotherFlowPair );
public:
	void SetProcID( const ProcessorID& _procID ){ m_procID = _procID;	};
	const ProcessorID&  GetProcID() const {  return m_procID;	};

	CDirection* GetDirection() { return &m_Dircetion; }
	void AddCanLeadGate(const CMobileElemConstraint &p_type, const ProcessorID& procID);
	void AddCanLeadGateList(const CMobileElemConstraint &p_type, std::vector<ProcessorID>& vProcID);
	BOOL FindCanLeadGate(const CMobileElemConstraint &p_type, const ProcessorID& procID);

	//reclaim
	CDirection* GetReclaimDirection(){return &m_ReclaimDircetion;}
	void AddCanLeadReclaim(const CMobileElemConstraint &p_type, const ProcessorID& procID);
	void AddCanLeadReclaimList(const CMobileElemConstraint &p_type, std::vector<ProcessorID>& vProcID);
	BOOL FindCanLeadReclaim(const CMobileElemConstraint &p_type, const ProcessorID& procID);

	void AddDestProc( const CFlowDestination& _destProc, bool _bMustEvenPercent = true);
	void EvenPercent();
	void EvenRemainPercent( const ProcessorID& _destProc );
	void Total100Percent();
	void SetFlag(bool bFlag){m_bVisitFlag = bFlag;}
	bool GetFlag()const{return m_bVisitFlag;}
/*[7/5/2004]
	void EvenRemainPercent( const std::vector<ProcessorID>& _vDestProc );
	int GetAppointedDestCount() const;
	void NormalizeDest();
	bool IsNormalized();
*/
		
	bool IsStation( const ProcessorID& _procID ) const ;
	
	void DeleteDestProc( const ProcessorID & _destProc );
	void DeleteDestProc( int _iIndex );
	int GetDestCount() const{ return m_vDestProcs.size();};
	const CFlowDestination& GetDestProcAt( int _iIndex ) const ;
//	const CFlowDestination& GetDestProcAt( const ProcessorID & _destProc ) const ;
	
	CFlowDestination* GetDestProcArcInfo( const ProcessorID & _destProc );
	CFlowDestination* GetDestProcArcInfo( int _iIndex );
	int GetDestProcCount() const{ return m_vDestProcs.size();	};
	void ClearAllDestProc(){ m_vDestProcs.clear();	};

	bool operator == ( const CProcessorDestinationList& _another ) const;
	void SetAllDestVisitFlag( bool _bVisit );
	//void SetAllDestInherigeFlag( bool _bIsInherige );
	void SetAllDestInherigeFlag( int _iType );
	bool IfTotal100Percent() const;
	bool IfUseThisPipe(int nCurPipeIndex)const;
	bool FlowDistinationUnVisit()const;

};
