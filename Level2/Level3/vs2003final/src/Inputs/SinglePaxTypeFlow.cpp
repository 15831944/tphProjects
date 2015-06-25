// SinglePaxTypeFlow.cpp: implementation of the CSinglePaxTypeFlow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SinglePaxTypeFlow.h"
#include <algorithm>
#include "engine\proclist.h"
#include "AllPaxTypeFlow.h"
#include "common\ARCException.h"
#include "DirectionUtil.h"
#include "in_term.h"
#include "engine\flowchannellist.h"
#include "HandleSingleFlowLogic.h"
#include "Engine/LandsideTerminalLinkageInSim.h"

//#define  _TRACE_ 

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSinglePaxTypeFlow::CSinglePaxTypeFlow( InputTerminal* _pTerm )
{
	m_pTerm = _pTerm;
	m_pPaxConstrain = new CMobileElemConstraint(_pTerm);
	m_pParentPaxFlow = NULL;
	m_bHasChanged = false;
	m_vFlowDestion.clear();
	
}

CSinglePaxTypeFlow::CSinglePaxTypeFlow()
{
	m_pPaxConstrain = new CMobileElemConstraint(NULL);
	m_pParentPaxFlow = NULL;
	m_bHasChanged = false;
}

CSinglePaxTypeFlow::~CSinglePaxTypeFlow()
{
	if( m_pPaxConstrain )
	{
		delete m_pPaxConstrain;
		m_pPaxConstrain = NULL;
	}
}
CSinglePaxTypeFlow::CSinglePaxTypeFlow( const CSinglePaxTypeFlow& _anotherPaxFlow )
{
	m_pPaxConstrain = new CMobileElemConstraint(_anotherPaxFlow.m_pTerm);
	*m_pPaxConstrain = * (_anotherPaxFlow.m_pPaxConstrain);
	m_pParentPaxFlow = _anotherPaxFlow.m_pParentPaxFlow;
	m_bHasChanged = _anotherPaxFlow.m_bHasChanged;
	m_vPaxFlowDigraph = _anotherPaxFlow.m_vPaxFlowDigraph;
	m_pTerm = _anotherPaxFlow.m_pTerm;
	m_vFlowDestion = _anotherPaxFlow.m_vFlowDestion;
	
}
CSinglePaxTypeFlow& CSinglePaxTypeFlow::operator = ( const CSinglePaxTypeFlow& _anotherPaxFlow )
{
	if( this != & _anotherPaxFlow )
	{
		*m_pPaxConstrain = * (_anotherPaxFlow.m_pPaxConstrain);
		m_vPaxFlowDigraph = _anotherPaxFlow.m_vPaxFlowDigraph;
		m_bHasChanged = _anotherPaxFlow.m_bHasChanged;
		m_pParentPaxFlow = _anotherPaxFlow.m_pParentPaxFlow;
		m_pTerm = _anotherPaxFlow.m_pTerm;
		m_vFlowDestion = _anotherPaxFlow.m_vFlowDestion;
			
	}
	return *this;
}
void CSinglePaxTypeFlow::InitPaxConstrain( InputTerminal* _pTerm, CString& _sPaxConstrainName, int _nConstraintVersion )
{
	m_pPaxConstrain->SetInputTerminal( _pTerm );
	m_pPaxConstrain->setConstraintWithVersion( _sPaxConstrainName );
}
void CSinglePaxTypeFlow::SetPaxConstrain( const CMobileElemConstraint* _pPaxConstrain )
{
	*m_pPaxConstrain = *_pPaxConstrain;
}

void CSinglePaxTypeFlow::AddDestProc(const ProcessorID & _sourceProcID,const CFlowDestination& _destProc , bool _bMustEvenPercent )
{
	if( _sourceProcID == _destProc.GetProcID() )
		return;
	PAXFLOWIDGRAPH::iterator iterSource = NULL;
	PAXFLOWIDGRAPH::iterator iterDest = NULL;
	PAXFLOWIDGRAPH::iterator iter = m_vPaxFlowDigraph.begin();
	PAXFLOWIDGRAPH::iterator iterEnd = m_vPaxFlowDigraph.end();
	for( ; iter != iterEnd; ++iter )
	{
		if( iter->GetProcID() == _sourceProcID )
		{
			iterSource = iter;
		}
		else if( iter->GetProcID() == _destProc.GetProcID() )
		{
			iterDest = iter;
		}
		if( &(*iterSource) && &(*iterDest) )
			break;
	}

	//ASSERT( iterSource );
	if( !&(*iterSource) )
	{
		AddIsolatedProc( _sourceProcID );
		iterSource = m_vPaxFlowDigraph.begin() + m_vPaxFlowDigraph.size() -1;			
	}
	

	iterSource->AddDestProc( _destProc ,_bMustEvenPercent );
	//iterSource->SetAllDestInherigeFlag( false  );
	if( !&(*iterDest) )
	{
		AddIsolatedProc( _destProc.GetProcID() );
	}
	m_bHasChanged = true;
		//PrintAllStructure();	
}

// add _secondProc to _newProc's destination list  , add _newProc  to _firstProc's destination list and remove _secondProc from _firstProc's destination list
void CSinglePaxTypeFlow::InsertBetwwen( const ProcessorID & _firstProcID, const ProcessorID & _secondProcID, const CFlowDestination& _newProc )
{
	if( _firstProcID == _newProc.GetProcID() ||
		_secondProcID == _newProc.GetProcID() )
	{
		return;
	}
	PAXFLOWIDGRAPH::iterator iterFirstProc = NULL;
	PAXFLOWIDGRAPH::iterator iterSecondProc = NULL;
	PAXFLOWIDGRAPH::iterator iterNewProc = NULL;
	PAXFLOWIDGRAPH::iterator iter = m_vPaxFlowDigraph.begin();
	PAXFLOWIDGRAPH::iterator iterEnd = m_vPaxFlowDigraph.end();
	for( ; iter != iterEnd; ++iter )
	{
		if( iter->GetProcID() == _firstProcID )
		{
			iterFirstProc = iter;
		}
		else if( iter->GetProcID() == _secondProcID )
		{
			iterSecondProc = iter;
		}
		else if ( iter->GetProcID() == _newProc.GetProcID()  )
		{
			iterNewProc = iter;
		}

		if( &(*iterFirstProc) && &(*iterSecondProc) && &(*iterNewProc) )
			break;
	}

	if( &(*iterFirstProc) && &(*iterSecondProc) )
	{
		CFlowDestination* pDestInfo = iterFirstProc->GetDestProcArcInfo( _secondProcID );
		ASSERT( pDestInfo );
		
		CFlowDestination tempInfo ;
		tempInfo= *pDestInfo;
		iterFirstProc->AddDestProc( _newProc );
		iterFirstProc->DeleteDestProc( _secondProcID );		
		
		if( !&(*iterNewProc) )
		{	
			AddIsolatedProc( _newProc.GetProcID() );
			iterNewProc = m_vPaxFlowDigraph.begin() + m_vPaxFlowDigraph.size() -1;			
		}

//		tempInfo.SetInheritedFlag( false );
		tempInfo.SetTypeOfOwnership( 0 );
		iterNewProc->AddDestProc( tempInfo );
		
		m_bHasChanged = true;
		return;
	}

	ASSERT( 0 );
}

//add  _sourceProcID's dest proc into _insertProcID's dest list , and delete _sourceProcID's all dest list ,then add _insertProcID into _sourceProcID's dest list
void CSinglePaxTypeFlow::InsertProceoosorAfter( const ProcessorID& _sourceProcID , const ProcessorID& _insertProcID  )
{
	if( _sourceProcID == _insertProcID )
		return;
	if( !IfExist( _insertProcID ) )
	{
		AddIsolatedProc( _insertProcID );
	}
	
	PAXFLOWIDGRAPH::iterator iterSource = NULL;
	PAXFLOWIDGRAPH::iterator iterNewProc = NULL;
	
	PAXFLOWIDGRAPH::iterator iter = m_vPaxFlowDigraph.begin();
	PAXFLOWIDGRAPH::iterator iterEnd = m_vPaxFlowDigraph.end();
	for( ; iter != iterEnd; ++iter )
	{
		if( iter->GetProcID() == _sourceProcID )
		{
			iterSource = iter;
		}
		else if( iter->GetProcID() == _insertProcID )
		{
			iterNewProc = iter;
		}

		if( &(*iterSource) && &(*iterNewProc) )
			break;
	}
	ASSERT(	&(*iterSource) && &(*iterNewProc) );
	int iDestCount = iterSource->GetDestProcCount();
	for( int i=0; i<iDestCount; ++i )
	{
		//iterSource->GetDestProcArcInfo( i )->SetInheritedFlag( false );
		iterSource->GetDestProcArcInfo( i )->SetTypeOfOwnership( 0 );
		iterNewProc->AddDestProc( iterSource->GetDestProcAt( i ) , true );
	}

	iterSource->ClearAllDestProc();

	CFlowDestination temp;
	temp.SetProcID( _insertProcID );
	iterSource->AddDestProc( temp, true );
	m_bHasChanged = true;

}
//remove _destProc from _sourceProc's destination list . if this pair is derived from father and _sourceProc has only one destination ,that is _destProc, then this operation is not allowed.
void CSinglePaxTypeFlow::CutLinkWithSourceProc( const  ProcessorID& _sourceProcID, const ProcessorID& _destProcID )
{
	if( _sourceProcID == _destProcID )
	{
		ClearSideEffectTree( _sourceProcID );
		ClearIsolateNode();
		return;
	}
	PAXFLOWIDGRAPH::iterator iter = m_vPaxFlowDigraph.begin();
	PAXFLOWIDGRAPH::iterator iterEnd = m_vPaxFlowDigraph.end();
	for( ; iter != iterEnd; ++iter )
	{
		if( iter->GetProcID() == _sourceProcID )
		{
			CFlowDestination* pDestInfo = iter->GetDestProcArcInfo( _destProcID );
			ASSERT( pDestInfo );
			iter->DeleteDestProc( _destProcID );
			m_bHasChanged = true;
			/*
				if( iter->GetDestProcCount() <= 0 )
					m_vPaxFlowDigraph.erase( iter );
			*/
			ClearSideEffectTree( _destProcID );
			//must call after ClearSideEffectTree
			ClearIsolateNode();
			return;			
		}
	}

	//ASSERT( 0 );
	
}

// add all destnition proccessors of _oldDestProcId to _soruceProcID and remove all destproc from _oldDestProcId , then remove _oldDestProcID
void CSinglePaxTypeFlow::TakeOverDestProc( const  ProcessorID& _sourceProcID, const ProcessorID& _oldDestProcID , bool _bEraseOldProc )
{

	PAXFLOWIDGRAPH::iterator iterFirstProc = NULL;
	PAXFLOWIDGRAPH::iterator iterSecondProc = NULL;
	
	PAXFLOWIDGRAPH::iterator iter = m_vPaxFlowDigraph.begin();
	PAXFLOWIDGRAPH::iterator iterEnd = m_vPaxFlowDigraph.end();
	for( ; iter != iterEnd; ++iter )
	{
		if( iter->GetProcID() == _sourceProcID )
		{
			iterFirstProc = iter;
			if( _sourceProcID == _oldDestProcID )// remove root.
			{
				m_vPaxFlowDigraph.erase( iter );
				return;
			}
		}
		else if( iter->GetProcID() == _oldDestProcID )
		{
			iterSecondProc = iter;
		}

		if( &(*iterFirstProc) && &(*iterSecondProc) )
			break;
	}

//	ASSERT( &(*iterFirstProc) && &(*iterSecondProc) );
	if (iterFirstProc != NULL)
		iterFirstProc->DeleteDestProc( _oldDestProcID );
	
	if (iterFirstProc != NULL && iterSecondProc != NULL)
	{
		int iDestCount = iterSecondProc->GetDestCount();
		for( int i=0; i < iDestCount ; ++i )
		{
//			iterSecondProc->GetDestProcArcInfo( i )->SetInheritedFlag( false );
			iterSecondProc->GetDestProcArcInfo( i )->SetTypeOfOwnership( 0 );
			iterFirstProc->AddDestProc( iterSecondProc->GetDestProcAt( i ) );
		}
	}


	int iSourceCount = GetSourceProcCount( _oldDestProcID );
	if( iSourceCount >= 1 )
	{
		ClearIsolateNode();
	}
 	else if (iterSecondProc != NULL)
 	{
 		DeleteProcNode( iterSecondProc->GetProcID() );
 	}
	/*
	if( _bEraseOldProc )
	{
		//iterSecondProc->ClearAllDestProc();
		//m_vPaxFlowDigraph.erase( iterSecondProc );
		DeleteProcNode( iterSecondProc->GetProcID() );
		//should call DeleteProcNode(...)
	}
	
	ClearIsolateNode();
	*/
	m_bHasChanged = true;
	
}

// add all destnition proccessors of _procID to all its source proc and remove all destproc from _procID and then remove _procID
void CSinglePaxTypeFlow::TakeOverDestProc(const ProcessorID& _procID )
{
	std::vector<ProcessorID> vAllSourceProc;
	GetSourceProc( _procID , vAllSourceProc );
	int iCount = vAllSourceProc.size();
	//only take over
	for( int i=0; i< iCount-1; ++i )
	{
		TakeOverDestProc( vAllSourceProc[i] , _procID );
	}

	//take over and clear all _procId's destproc then erase _procID
	if( iCount >=0 )
	{
		TakeOverDestProc( vAllSourceProc[iCount-1] , _procID ,true );
	}
	//m_bHasChanged = true;
}

//Remove _procID from _procID's all source proc's destination list and remove _procID's all destination list and then remove _procID
void CSinglePaxTypeFlow::DeleteProcNode( const ProcessorID& _procID )
{
	CString strTmpName = _procID.GetIDString();

	PAXFLOWIDGRAPH::iterator iterProc = NULL;
	
	PAXFLOWIDGRAPH::iterator iter = m_vPaxFlowDigraph.begin();
	PAXFLOWIDGRAPH::iterator iterEnd = m_vPaxFlowDigraph.end();
	for( ; iter != iterEnd; ++iter )
	{
		if( iter->GetProcID() == _procID )
		{
			iterProc = iter;
		}
		else
		{
			iter->DeleteDestProc( _procID );
		}		
	}
	//ASSERT( iterProc );
	if( &(*iterProc) )
	{
		iterProc->ClearAllDestProc();
		m_vPaxFlowDigraph.erase( iterProc );
		m_bHasChanged = true;
		ClearIsolateNode();
	}
	
}
// make the flow _flowPairProcID's dest ( exclude _operateProcID ) proc's percent balance 
void CSinglePaxTypeFlow::EvenRemainDestPercent( const ProcessorID& _flowPairProcID , const ProcessorID& _operateProcID  )
{
	PAXFLOWIDGRAPH::iterator iter = m_vPaxFlowDigraph.begin();
	PAXFLOWIDGRAPH::iterator iterEnd = m_vPaxFlowDigraph.end();
	for( ; iter != iterEnd; ++iter )
	{
		if( iter->GetProcID() == _flowPairProcID )
		{
			iter->EvenRemainPercent( _operateProcID );
			m_bHasChanged = true;
			return;
		}
	}
}
/*[7/5/2004]
void CSinglePaxTypeFlow::EvenRemainDestPercent( const ProcessorID& _flowPairProcID , const std::vector<ProcessorID>& _vOperateProcID  )
{
	PAXFLOWIDGRAPH::iterator iter = m_vPaxFlowDigraph.begin();
	PAXFLOWIDGRAPH::iterator iterEnd = m_vPaxFlowDigraph.end();
	for( ; iter != iterEnd; ++iter )
	{
		if( iter->GetProcID() == _flowPairProcID )
		{
			iter->EvenRemainPercent( _vOperateProcID );
			m_bHasChanged = true;
			return;
		}
	}

}
*/

// add a new node which have no source proc and no destination proc
void CSinglePaxTypeFlow::AddIsolatedProc( const ProcessorID& _procID )
{
  CProcessorDestinationList temp( m_pTerm );
  temp.SetProcID( _procID );
  m_vPaxFlowDigraph.push_back( temp );
  CString strtmp = temp.GetProcID().GetIDString();  
  m_bHasChanged = true;
  
}

const CProcessorDestinationList& CSinglePaxTypeFlow::GetFlowPairAt( int _iIndex ) const
{
	ASSERT( _iIndex >= 0 && (UINT)_iIndex < m_vPaxFlowDigraph.size() );
	return m_vPaxFlowDigraph.at( _iIndex );
}

CProcessorDestinationList* CSinglePaxTypeFlow::GetFlowPairAt( int _iIndex )
{
	ASSERT( _iIndex >= 0 && (UINT)_iIndex < m_vPaxFlowDigraph.size() );
	return &m_vPaxFlowDigraph.at( _iIndex );
}

bool CSinglePaxTypeFlow::GetFlowPairAt( const ProcessorID& _procID, CProcessorDestinationList& _flowPair )const
{
	PAXFLOWIDGRAPH::const_iterator iter = m_vPaxFlowDigraph.begin();
	PAXFLOWIDGRAPH::const_iterator iterEnd = m_vPaxFlowDigraph.end();
	for( ; iter != iterEnd; ++iter )
	{
		if( iter->GetProcID().idFits(_procID ) )
		{
			_flowPair = *iter;
			return true;
		}

	}
	return false;
}
bool CSinglePaxTypeFlow::GetEqualFlowPairAt( const ProcessorID& _procID, CProcessorDestinationList& _flowPair )const
{
	PAXFLOWIDGRAPH::const_iterator iter = m_vPaxFlowDigraph.begin();
	PAXFLOWIDGRAPH::const_iterator iterEnd = m_vPaxFlowDigraph.end();
	for( ; iter != iterEnd; ++iter )
	{
		if( iter->GetProcID() == _procID )
		{
			_flowPair = *iter;
			return true;
		}

	}
	return false;
}
CProcessorDestinationList* CSinglePaxTypeFlow::GetFlowPairAt( const ProcessorID& _procID)
{
	PAXFLOWIDGRAPH::iterator iter = m_vPaxFlowDigraph.begin();
	PAXFLOWIDGRAPH::iterator iterEnd = m_vPaxFlowDigraph.end();
	for( ; iter != iterEnd; ++iter )
	{
		if( iter->GetProcID().idFits(_procID ) )
			return &(*iter);
	}
	return NULL;
}
CProcessorDestinationList* CSinglePaxTypeFlow::GetEqualFlowPairAt( const ProcessorID& _procID)
{
	PAXFLOWIDGRAPH::iterator iter = m_vPaxFlowDigraph.begin();
	PAXFLOWIDGRAPH::iterator iterEnd = m_vPaxFlowDigraph.end();
	for( ; iter != iterEnd; ++iter )
	{
		if( iter->GetProcID() == _procID  )
			return &(*iter);
	}
	return NULL;
}

void CSinglePaxTypeFlow::GetSourceProc( const ProcessorID& _procID,std::vector<ProcessorID>& _sourceIDVector )const
{
	_sourceIDVector.clear();

	PAXFLOWIDGRAPH::const_iterator iter = m_vPaxFlowDigraph.begin();
	PAXFLOWIDGRAPH::const_iterator iterEnd = m_vPaxFlowDigraph.end();
	for( ; iter != iterEnd; ++iter )
	{
		int iDestProcCount = iter->GetDestProcCount();
		for( int i=0; i< iDestProcCount ; ++i )
		{
			if( iter->GetDestProcAt( i ).GetProcID().idFits( _procID ) )
			{
				_sourceIDVector.push_back( iter->GetProcID() );
				break;
			}
		}
	}
	
}
int CSinglePaxTypeFlow::GetSourceProcCount( const ProcessorID& _procID ) const 
{
	int iSourceCount = 0;
	int iFlowPairCount = m_vPaxFlowDigraph.size();
	for( int i=0; i<iFlowPairCount; ++i )
	{
		const CProcessorDestinationList& pFlowPair = m_vPaxFlowDigraph[i];
		int iDestProcCount = pFlowPair.GetDestProcCount();
		for( int j=0; j< iDestProcCount ; ++j )
		{
//			// TRACE("test Proc :%s,  Proc:%s, DestPRoc: %s \n", _procID.GetIDString(),pFlowPair.GetProcID().GetIDString(), pFlowPair.GetDestProcAt( j ).GetProcID().GetIDString() );
			if( pFlowPair.GetDestProcAt( j ).GetProcID() == _procID )
			{
				++iSourceCount;
				break;
			}			
		}
	}
	return iSourceCount;
}


bool CSinglePaxTypeFlow::IfExist( const ProcessorID& _procID ) const 
{
	PAXFLOWIDGRAPH::const_iterator iter = m_vPaxFlowDigraph.begin();
	PAXFLOWIDGRAPH::const_iterator iterEnd = m_vPaxFlowDigraph.end();
	for( ; iter != iterEnd; ++iter )
	{	
		if( iter->GetProcID() == _procID )
			return true;
	}
	return false;
}


bool CSinglePaxTypeFlow::IfFits( const ProcessorID& _procID ) const 
{
	PAXFLOWIDGRAPH::const_iterator iter = m_vPaxFlowDigraph.begin();
	PAXFLOWIDGRAPH::const_iterator iterEnd = m_vPaxFlowDigraph.end();
	for( ; iter != iterEnd; ++iter )
	{	
		if( iter->GetProcID().idFits( _procID ) )
			return true;
	}
	return false;
}

bool CSinglePaxTypeFlow::IfBaggageProcExistInFlow()const
{
//	Processor* pBaggageProc = m_pTerm->procList->getProcessor ( BAGGAGE_DEVICE_PROCEOR_INDEX);
	Processor *proc = NULL;

	PAXFLOWIDGRAPH::const_iterator iter = m_vPaxFlowDigraph.begin();
	PAXFLOWIDGRAPH::const_iterator iterEnd = m_vPaxFlowDigraph.end();
	for( ; iter != iterEnd; ++iter )
	{
		GroupIndex group = m_pTerm->procList->getGroupIndex ( iter->GetProcID() );
		for (int i = group.start; i <= group.end; i++)
		{
		    proc = m_pTerm->procList->getProcessor ( i );
	//			// TRACE("\n%s\n", proc->getID()->GetIDString() );
		    if( proc->getProcessorType() == BaggageProc )
			{
				return true;
			}
		}
	}
	return false;
}

bool CSinglePaxTypeFlow::IfGateProcExistInFlow()const
{
	Processor *proc = NULL;

	PAXFLOWIDGRAPH::const_iterator iter = m_vPaxFlowDigraph.begin();
	PAXFLOWIDGRAPH::const_iterator iterEnd = m_vPaxFlowDigraph.end();
	for( ; iter != iterEnd; ++iter )
	{
		GroupIndex group = m_pTerm->procList->getGroupIndex ( iter->GetProcID() );
		for (int i = group.start; i <= group.end; i++)
		{
		    proc = m_pTerm->procList->getProcessor ( i );
//			// TRACE("\n%s\n", proc->getID()->GetIDString() );
		    if( proc->getProcessorType() == GateProc  )
			{
				return true;
			}
		}
	}
	return false;
}
int CSinglePaxTypeFlow::IfProcessGroupExist( const ProcessorID& _procID ) const 
{
//	// TRACE("\n%s\n", _procID.GetIDString() );
	int iPos = -1;
	PAXFLOWIDGRAPH::const_iterator iter = m_vPaxFlowDigraph.begin();
	PAXFLOWIDGRAPH::const_iterator iterEnd = m_vPaxFlowDigraph.end();
	for( int i=0 ; iter != iterEnd; ++iter ,++i )
	{	
//		// TRACE("\n%s\n", iter->GetProcID().GetIDString() );
		if( iter->GetProcID() == _procID )
		{
			iPos = i;
			return iPos;
		}
//		else if( _procID.isChildOf(  iter->GetProcID() ) )
		else if( iter->GetProcID().idFits( _procID ) )		
		{
			iPos = i;
			return iPos;
		}
	}
	return iPos;
}

//get first processor equal with _procID in hierarchy flow 
//INPUT : _procID
//OUTPUT: _bOwened , CProcessorDestinationList
const CProcessorDestinationList* CSinglePaxTypeFlow::GetHierarchyFlowPair( const ProcessorID& _procID ,bool& _bOwened ) const
{
	PAXFLOWIDGRAPH::const_iterator iter = m_vPaxFlowDigraph.begin();
	PAXFLOWIDGRAPH::const_iterator iterEnd = m_vPaxFlowDigraph.end();
	//PrintAllStructure();
	for( ; iter != iterEnd; ++iter )
	{
//		// TRACE("%s, %s\n",iter->GetProcID().GetIDString() ,_procID.GetIDString() );
		if( _procID == iter->GetProcID() )
		{
			_bOwened = true;
			return &(*iter);
		}
	}

	_bOwened = false;
	if( m_pParentPaxFlow )
	{
		bool bNotUsed;
		return m_pParentPaxFlow->GetHierarchyFlowPair( _procID ,bNotUsed );
	}
	else
	{
		return NULL;
	}
}
//copy _destPair's all destproc into _sourcepair
// _bBackInsert = true : add into tail . _bBackInsert = false : clear all _sourcePair's destproc and add
// _iInheritedFlagType = 0 : inheritedflag don not be changed
// _iInheritedFlagType = 1 : set inheritedflag = true
// _iInheritedFlagType = 2 : set inheritedflag = false
void CSinglePaxTypeFlow::CopyFlowPair( CProcessorDestinationList* _sourcePair, const CProcessorDestinationList* _destPair , bool _bBackInsert, int _iInheritedFlagType )
{
	ASSERT( _sourcePair && _destPair );
	if( ! _bBackInsert )
	{
		_sourcePair->ClearAllDestProc();
	}

	int iDestProcCount = _destPair->GetDestCount();
	for( int i=0; i<iDestProcCount ;++i )
	{
		CFlowDestination temp = _destPair->GetDestProcAt( i );
		
		if( _iInheritedFlagType == 1 )
		{
//			temp.SetInheritedFlag( true );
			temp.SetTypeOfOwnership( 1 );
		}
		else if( _iInheritedFlagType == 2 )
		{
			//temp.SetInheritedFlag( false );
			temp.SetTypeOfOwnership( 0 );
		}
		_sourcePair->AddDestProc( temp );
	}
}
//get all processors in passenger flow hierarchy 
void CSinglePaxTypeFlow::GetAllInvolvedProc( std::vector<ProcessorID>& _sourceIDVector )const
{
	std::vector<ProcessorID>::const_iterator iterFindID = NULL;
	PAXFLOWIDGRAPH::const_iterator iter = m_vPaxFlowDigraph.begin();
	PAXFLOWIDGRAPH::const_iterator iterEnd = m_vPaxFlowDigraph.end();
//	PrintAllStructure();
	for( ; iter != iterEnd; ++iter )
	{
//		// TRACE("%s\n",iter->GetProcID().GetIDString() );		
		iterFindID = std::find( _sourceIDVector.begin(), _sourceIDVector.end(), iter->GetProcID() );
		if( iterFindID == _sourceIDVector.end() )
		{
			_sourceIDVector.push_back( iter->GetProcID() );
//	    	// TRACE("_sourceIDVector.push_back %s\n",iter->GetProcID().GetIDString() );
		}
	}

	if( m_pParentPaxFlow )
		m_pParentPaxFlow->GetAllInvolvedProc( _sourceIDVector );
}
bool operator < ( const CSinglePaxTypeFlow& _firstPaxFlow ,const CSinglePaxTypeFlow& _anotherPaxFlow )
//bool CSinglePaxTypeFlow::operator < ( const CSinglePaxTypeFlow& _anotherPaxFlow )
{
	if( *(_firstPaxFlow.m_pPaxConstrain) < *(_anotherPaxFlow.m_pPaxConstrain) )
		return true;
	return false;
}
void CSinglePaxTypeFlow::GetRootProc( std::vector<ProcessorID>& _rootIDVector ) const 
{
	_rootIDVector.clear();
	PAXFLOWIDGRAPH::const_iterator iter = m_vPaxFlowDigraph.begin();
	PAXFLOWIDGRAPH::const_iterator iterEnd = m_vPaxFlowDigraph.end();
//	PrintAllStructure();
	for( ; iter != iterEnd; ++iter )
	{
		//// TRACE("GetRootProc : %s\n",iter->GetProcID().GetIDString());
		int iSourceCount = GetSourceProcCount ( iter->GetProcID() );
		if( 0 == iSourceCount )
		{
			_rootIDVector.push_back( iter->GetProcID() );
		}
	}
}

void CSinglePaxTypeFlow::GetLeafNode( std::vector<ProcessorID>& _rootIDVector ) const 
{
	_rootIDVector.clear();
	PAXFLOWIDGRAPH::const_iterator iter = m_vPaxFlowDigraph.begin();
	PAXFLOWIDGRAPH::const_iterator iterEnd = m_vPaxFlowDigraph.end();
//	PrintAllStructure();
	for( ; iter != iterEnd; ++iter )
	{
		if( iter->GetDestCount() <=0 )
		{
			_rootIDVector.push_back( iter->GetProcID() );
		}
	}
}
void CSinglePaxTypeFlow::GetLeafNodeEx( std::vector<ProcessorID>& _leafIDVector )  
{
	//_leafIDVector.clear();
	std::vector<ProcessorID> vRoot;
	GetRootProc( vRoot );
	int iRootCount = vRoot.size();
	for( int i=0; i<iRootCount; ++i )
	{
		CProcessorDestinationList* pPair = GetFlowPairAt( vRoot[ i ] );	
		int iDestCount = pPair->GetDestCount();
		if(iDestCount == 0)//if START - B
						   //	      - C    B&C should be leaf node 	
		{
			_leafIDVector.push_back(vRoot[i]);
			continue;
		}

		CSinglePaxTypeFlow _resultFlow;
		for( int j=0; j<iDestCount; ++j )
		{
			FindLeafFromNode( pPair->GetDestProcAt( j).GetProcID() ,_resultFlow,_leafIDVector );
		}
	}	
}

void CSinglePaxTypeFlow::FindLeafFromNode( const ProcessorID& _fromProc , CSinglePaxTypeFlow& _resultFlow,std::vector<ProcessorID>& _leafIDVector)
{
	CProcessorDestinationList* pPair = GetFlowPairAt( _fromProc );	
	if( pPair == NULL )
	{
		if( std::find( _leafIDVector.begin(), _leafIDVector.end(), _fromProc ) == _leafIDVector.end() )
		{
			_leafIDVector.push_back( _fromProc );
		}

		return;
	}
	int iDestCount = pPair->GetDestCount();

	if( iDestCount == 0 )
	{
		if( std::find( _leafIDVector.begin(), _leafIDVector.end(), _fromProc ) == _leafIDVector.end() )
		{
			_leafIDVector.push_back( _fromProc );
		}
		return;
	}
	

	TRACE("%s\n",_fromProc.GetIDString());
	_resultFlow.AddFowPair(*pPair);
	for( int j=0; j<iDestCount; ++j )
	{
		if (!HandleSingleFlowLogic::IfHaveCircleFromProc(&_resultFlow,pPair->GetDestProcAt( j ).GetProcID()))
		{
			FindLeafFromNode( pPair->GetDestProcAt( j ).GetProcID(),_resultFlow, _leafIDVector );
		}
	}
}
void CSinglePaxTypeFlow::PrintAllStructure()const
{
	CString sOutStr;
	PAXFLOWIDGRAPH::const_iterator iter = m_vPaxFlowDigraph.begin();
	PAXFLOWIDGRAPH::const_iterator iterEnd = m_vPaxFlowDigraph.end();
	for( ; iter != iterEnd; ++iter )
	{
		sOutStr=" SourceProcID :";
		sOutStr += iter->GetProcID().GetIDString();
		 TRACE("%s   ", sOutStr );

		int iDestProcCount = iter->GetDestProcCount();
		sOutStr = " DestProceID: ";
		for( int i=0; i< iDestProcCount ; ++i )
		{
			sOutStr += iter->GetDestProcAt( i ).GetProcID().GetIDString();
			sOutStr +="  ";
		}
		 TRACE("%s\n", sOutStr );

	}
}

void CSinglePaxTypeFlow::SetAllPairVisitFlag( bool _bVisit )
{
	PAXFLOWIDGRAPH::iterator iter = m_vPaxFlowDigraph.begin();
	PAXFLOWIDGRAPH::iterator iterEnd = m_vPaxFlowDigraph.end();
	for( ; iter != iterEnd; ++iter )
	{
		iter->SetAllDestVisitFlag( _bVisit );
	}
}
/************************************************************************
FUNCTION: make sure the processor is a root or not
INPUT	: the specified root
OUTPUT	: TRUE,it is a root
		  FALSE,not a root                                                                      
/************************************************************************/
bool CSinglePaxTypeFlow::IsRootInHierarchy( const ProcessorID& _testProcID ) const
{

	int iFlowPairCount = m_vPaxFlowDigraph.size();
	for( int i=0; i<iFlowPairCount; ++i )
	{
		const CProcessorDestinationList& pFlowPair = m_vPaxFlowDigraph[i];
		int iDestProcCount = pFlowPair.GetDestProcCount();
		for( int j=0; j< iDestProcCount ; ++j )
		{
#ifdef _TRACE_
			// TRACE("IsRootInHierarchy\n");
			// TRACE("test Proc :%s,  Proc:%s, DestPRoc: %s \n", _testProcID.GetIDString(),pFlowPair.GetProcID().GetIDString(), pFlowPair.GetDestProcAt( j ).GetProcID().GetIDString() );
#endif
			if( pFlowPair.GetDestProcAt( j ).GetProcID() == _testProcID )
			{
				return false;
			}			
		}
	}

	if( !m_pParentPaxFlow )
	{
		return true;
	}

	return m_pParentPaxFlow->IsRootInHierarchy( _testProcID ) ;
	
}
/************************************************************************
FUNCTION: Get all roots in the hierarchy FLow
INPUT	:
RETURN	:_rootIDVector,stored the root processor
/************************************************************************/

void CSinglePaxTypeFlow::GetRootProcInHierarchy( std::vector<ProcessorID>& _rootIDVector ) const 
{
	_rootIDVector.clear();
	std::vector<ProcessorID> allProc;
	GetAllInvolvedProc( allProc );
	int iCount = allProc.size();
	for( int i=0; i<iCount; ++i )
	{
		CString strTemp= allProc[ i ].GetIDString() ;		
		if( IsRootInHierarchy( allProc[ i ] ) )
		{
			_rootIDVector.push_back( allProc[ i ] );
			strTemp= allProc[ i ].GetIDString() ;
		}
	}
}
bool CSinglePaxTypeFlow::IfFlowValid() const
{
	PAXFLOWIDGRAPH::const_iterator iter = m_vPaxFlowDigraph.begin();
	PAXFLOWIDGRAPH::const_iterator iterEnd = m_vPaxFlowDigraph.end();
	for( ; iter != iterEnd; ++iter )
	{
		if( !iter->IfTotal100Percent() )
		{
			CString sMsg;
			m_pPaxConstrain->screenPrint( sMsg ,0,128);
			CString sErrorMsg = " Passenger Flow : ";
			sErrorMsg += sMsg;
			sErrorMsg += ".\n Processor: ";
			sErrorMsg += iter->GetProcID().GetIDString();
			sErrorMsg += ".\n Total Destination Distribution Probabilty is not 100% !";
			AfxMessageBox( sErrorMsg);
			return false;
		}			
	}
	return true;
}
void CSinglePaxTypeFlow::AddFowPair( const CProcessorDestinationList& _flowPair )
{
	if( IfExist( _flowPair.GetProcID() ) )
	{
		int iSize = _flowPair.GetDestCount();
		for( int i=0; i<iSize ; ++i )
		{
			AddDestProc( _flowPair.GetProcID(),_flowPair.GetDestProcAt( i ),false );
		}
	}
	else
	{
		m_vPaxFlowDigraph.push_back( _flowPair );
		CString strtmp = _flowPair.GetProcID().GetIDString();
	}
/*
	CProcessorDestinationList* pFlowPair=GetFlowPairAt(_flowPair.GetProcID());
	ASSERT(pFlowPair!=NULL);
	if(pFlowPair->IsNormalized()==false)
	{
		pFlowPair->NormalizeDest();
	}
*/
}
void CSinglePaxTypeFlow::ClearIsolateNode()
{
	std::vector<int> vErasedPos;
	PAXFLOWIDGRAPH::iterator iter = m_vPaxFlowDigraph.begin();
	PAXFLOWIDGRAPH::iterator iterEnd = m_vPaxFlowDigraph.end();
	if (m_pTerm == NULL)
		return;
	
	for(int i=0 ; iter != iterEnd; ++iter ,++i )
	{	
		if (m_pTerm->procList->getGroupIndexOriginal(iter->GetProcID()).start == START_PROCESSOR_INDEX)
			continue;

		int iSourceProcCount = GetSourceProcCount( iter->GetProcID() );
		if( iSourceProcCount <= 0 )
		{
			if( iter->GetDestProcCount() <= 0)
			{
				vErasedPos.push_back( i );
			}
		}
	}

	int iErasedPosCount = vErasedPos.size();
	for( int j=iErasedPosCount-1; j >= 0; --j )
	{
		m_vPaxFlowDigraph.erase( m_vPaxFlowDigraph.begin() + vErasedPos[ j ]  );
	}
}

void CSinglePaxTypeFlow::ClearUselessTree( const ProcessorID& _usefulRootProcID )
{
	std::vector<ProcessorID> vRoot;
	GetRootProc( vRoot );
	if( vRoot.size() <= 1 )
		return;
	
	std::vector<ProcessorID>::iterator iter = vRoot.begin();
	std::vector<ProcessorID>::iterator iterEnd = vRoot.end();
	for( ; iter != iterEnd ; ++iter )
	{
		if( *iter == _usefulRootProcID )
			continue;
		
		DeleteProcNode( *iter );
		
	}

	ClearUselessTree( _usefulRootProcID );
	return;
}

bool CSinglePaxTypeFlow::IfHaveCircleFromProc( const ProcessorID& _fromProc )
{
	CProcessorDestinationList* pFlowPair = GetFlowPairAt( _fromProc );
//	ASSERT( pFlowPair );
	if (pFlowPair == NULL || pFlowPair->GetFlag())
	{
		return false;
	}

	pFlowPair->SetFlag(true);

	int iDestCount = pFlowPair->GetDestCount();
	for( int i=0; i<iDestCount; ++i )
	{
		CFlowDestination* pDestInfo = pFlowPair->GetDestProcArcInfo( i );
		if( pDestInfo->GetVisitFlag() == true )
		{
			return true;
		}
		else
		{
			pDestInfo->SetVisitFlag( true );
			if( IfHaveCircleFromProc( pDestInfo->GetProcID() ) )
			{
				return true;
			}
		}

		pDestInfo->SetVisitFlag(false);
	}
	return false;
}

void CSinglePaxTypeFlow::ClearSideEffectTree( const ProcessorID& _rootProcID)
{
	if( GetSourceProcCount( _rootProcID ) == 0 )// is root
	{
		CProcessorDestinationList* pFlowPair = GetEqualFlowPairAt( _rootProcID );
		if (pFlowPair == NULL)
			return;

		int iDestCount = pFlowPair->GetDestProcCount();
		for( int i=iDestCount-1; i>=0; --i )
		{
			ProcessorID tempID ( pFlowPair->GetDestProcAt( i ).GetProcID() );
			pFlowPair->DeleteDestProc( i );
			ClearSideEffectTree( tempID );
		}	
	}
}

int CSinglePaxTypeFlow::GetFlowPairIdx( const ProcessorID& _flowPairID )const
{
	PAXFLOWIDGRAPH::const_iterator iter = m_vPaxFlowDigraph.begin();
	PAXFLOWIDGRAPH::const_iterator iterEnd = m_vPaxFlowDigraph.end();
	for( int i=0 ; iter != iterEnd; ++iter ,++i )
	{	
		if( iter->GetProcID().idFits(_flowPairID ) )
		{
			return i;
		}
	}
	return -1;
}

bool CSinglePaxTypeFlow::ResetDesitinationFlag(bool bFlag)
{
	PAXFLOWIDGRAPH::iterator iter = m_vPaxFlowDigraph.begin();
	PAXFLOWIDGRAPH::iterator iterEnd = m_vPaxFlowDigraph.end();
	for( int i=0 ; iter != iterEnd; ++iter ,++i )
	{	
		iter->SetFlag(bFlag); 
	}
	return true;
}
/************************************************************************
FUNCTION: merge two Flow(subFlow)
INPUT	: _flow,the flow(subFlow) to be merged
OUTPUT	:                                                                      
/************************************************************************/
void CSinglePaxTypeFlow::MergeTwoFlow( const CSinglePaxTypeFlow& _flow )
{
	int iPairCount = _flow.GetFlowPairCount();
	for( int i=0; i<iPairCount; ++i )
	{
		const CProcessorDestinationList& pair = _flow.GetFlowPairAt( i );
		int iDestCount = pair.GetDestCount();
		for( int j=0; j<iDestCount-1; ++j )
		{
			AddDestProc( pair.GetProcID(), pair.GetDestProcAt( j ) , false );
		}			
		if( iDestCount > 0 )
		{
			AddDestProc( pair.GetProcID(), pair.GetDestProcAt( iDestCount -1  ) , true );
		}
	}
}
/************************************************************************
FUNCTION	:analyse the flow and disassemble the Flow into possible path
INPUT		:_vAllPossiblePath, stored all the possible path
original Flow:
A
	B
		C
	D
		E
		F
result:
A--->B--->C
A--->D--->E
A--->D--->F
/************************************************************************/
void CSinglePaxTypeFlow::BuildAllPossiblePath( std::vector<CSinglePaxTypeFlow*>& _vAllPossiblePath, CDirectionUtil* pDir )
{
	m_vFlowDestion.clear();
	std::vector<ProcessorID>vRootIDVector;
	GetRootProc( vRootIDVector );
	int iRootCount = vRootIDVector.size();
	for( int i=0; i<iRootCount; ++i )
	{
		std::vector<CFlowDestination> vProc;
		CFlowDestination temp;
		temp.SetProcID( vRootIDVector[ i ] );
		if (std::find(m_vFlowDestion.begin(),m_vFlowDestion.end(),temp) == m_vFlowDestion.end())
		{
			m_vFlowDestion.push_back(temp);
			vProc.push_back( temp );
			BuildAllPathOfSingleTree( vRootIDVector[ i ] , vProc,  _vAllPossiblePath, pDir );
		}
		vProc.pop_back();
	}

	m_pTerm->procList->ClearGateAndReclaimData();
	SetAllPairVisitFlag(false);

	m_vFlowDestion.clear();
}


// start from any node,
// 1. if leave node. add the path to the result vector. delete the leaf node in the tempflow.
// 2. else, for each dest node:
//	a. add node to the tempflow.
//  b. check if pattern repeat. if yes delete the node from tempflow and return
//  c. continue call recusive function.
//
void CSinglePaxTypeFlow::BuildAllPathOfSingleTree( const ProcessorID& _rootID , std::vector<CFlowDestination>& _vProc, std::vector<CSinglePaxTypeFlow*>& _vAllPossiblePath, CDirectionUtil* pDir )
{
	CProcessorDestinationList* pFlowPair = GetFlowPairAt( _rootID );
	GroupIndex rootGroup = m_pTerm->procList->getGroupIndex(_rootID);
	
	//TRACE("\r\n --------- Proc: %s",_rootID.GetIDString());
	int iDestCount = pFlowPair->GetDestCount();
	if( iDestCount <=0 )// leaf node 
	{		
		ASSERT( m_pTerm );
		CSinglePaxTypeFlow* pFlow = new CSinglePaxTypeFlow( m_pTerm );
		pFlow->BuildLineFlowFromVector( _vProc );
	
		_vAllPossiblePath.push_back( pFlow );
		//add gate info into before the current processor

		std::vector<ProcessorID> vReclaimProc;
		CMobileElemConstraint type(m_pTerm);

		Processor* pToGate = m_pTerm->procList->getProcessor( TO_GATE_PROCESS_INDEX );
		if (pToGate)
		{
			pToGate->GetDirection().Insert(*m_pPaxConstrain,*(pToGate->getID()));
		}

		Processor* pFromGate = m_pTerm->procList->getProcessor( FROM_GATE_PROCESS_INDEX );
		if (pFromGate)
		{
			pFromGate->GetDirection().Insert(*m_pPaxConstrain,*(pFromGate->getID()));
		}

		Processor* pBaggage = m_pTerm->procList->getProcessor( BAGGAGE_DEVICE_PROCEOR_INDEX );
		if (pBaggage)
		{
			pBaggage->GetReclaimDrection().Insert(*m_pPaxConstrain,*(pBaggage->getID()));
		}
		
		//landside object
		if(pDir && pDir->m_pLTLinkageManager && m_pPaxConstrain->isArrival() )//terminal processor linkage to landside object
		{
			//get last processor non-end processor
			ProcessorID lastProcID;
			lastProcID.SetStrDict(m_pTerm->inStrDict);
			int nProcCount = _vProc.size();
			if(nProcCount > 1)
				lastProcID = _vProc[nProcCount -2].GetProcID();
			

			//nProcCount -1 = END, ingore
			//nProcCount -2 = Last Processor
			if(!lastProcID.isBlank())
			{	
				CString strProc = lastProcID.GetIDString();
				std::vector<LandsideObjectLinkageItemInSim> _vLandsideDestObject;

				//the landside destination object linked to the last processor group
				std::vector<LandsideObjectLinkageItemInSim> _vGroupLandsideDestObject;
				//find the landside linkage
				TerminalProcLinkageInSim *pLinkageInSim = pDir->m_pLTLinkageManager->FindItem(lastProcID);
				if(pLinkageInSim)
				{
					_vGroupLandsideDestObject = pLinkageInSim->GetLandsideObject();
					_vLandsideDestObject.insert(_vLandsideDestObject.begin(), _vGroupLandsideDestObject.begin(), _vGroupLandsideDestObject.end());
				}

				GroupIndex procGroupIndex = m_pTerm->procList->getGroupIndex(lastProcID);
				for (int nGroupIndex = procGroupIndex.start; nGroupIndex <= procGroupIndex.end; ++ nGroupIndex)
				{
					Processor *pProc = m_pTerm->procList->getProcessor(nGroupIndex);
					if(pProc)
					{
						CString strSubProc = pProc->getIDName();

						TerminalProcLinkageInSim *pLinkageInSim = pDir->m_pLTLinkageManager->FindItem(*pProc->getID());
						if(pLinkageInSim)
						{
							const std::vector<LandsideObjectLinkageItemInSim>& vThisLandsideDest = pLinkageInSim->GetLandsideObject();
							pProc->GetLandsideDrection().Insert(*m_pPaxConstrain, vThisLandsideDest);
							pProc->GetLandsideDrection().Insert(*m_pPaxConstrain, _vGroupLandsideDestObject);
							_vLandsideDestObject.insert(_vLandsideDestObject.begin(), vThisLandsideDest.begin(), vThisLandsideDest.end());
						}
					}
				}
				//nProcCount -3 = 
			//	for (int nProc = nProcCount -3; nProc >= 0; nProc --)
			//	{
				//	const ProcessorID& procID = _vProc[nProc].GetProcID();
				//	CString strID = procID.GetIDString();
					GroupIndex group = m_pTerm->procList->getGroupIndex(lastProcID);
					if(group.start >= 0)
					{
						for (int nProcIndex = group.start; nProcIndex <= group.end; nProcIndex++)
						{	
							Processor* pProc = m_pTerm->procList->getProcessor( nProcIndex );
							if (pProc)
							{
								pProc->GetLandsideDrection().Insert(*m_pPaxConstrain, _vLandsideDestObject);
							}
						}					
					}

			//	}
			}
		}
		//terminal processor
		int nCount = (int)_vProc.size();
		for(int m = nCount-1; m >= 0; m--)
		{
			const ProcessorID& procID = _vProc[m].GetProcID();
			
			GroupIndex group = m_pTerm->procList->getGroupIndex(procID);
			Processor* pProc = m_pTerm->procList->getProcessor( group.start );

			if (pProc && pProc->getProcessorType() == BaggageProc)
			{
				for (int n = group.start; n <= group.end; n++)
				{
					Processor* pSourcePro = m_pTerm->procList->getProcessor(n);
					if (pSourcePro)
					{
						pSourcePro->GetReclaimDrection().Insert(*m_pPaxConstrain,*(pSourcePro->getID()));
					}
				}
			}

			if (pProc && pProc->getProcessorType() == GateProc)
			{
				//TRACE("\r\n Gate Proc: %s",pProc->getID()->GetIDString());
				for (int n = group.start; n <= group.end; n++)
				{
					Processor* pSourcePro = m_pTerm->procList->getProcessor(n);
					if (pSourcePro)
					{
						pSourcePro->GetDirection().Insert(*m_pPaxConstrain,*(pSourcePro->getID()));
					}
				}
			}
		}
	 }
	else
	{
		for( int i=0; i<iDestCount; ++i )
		{
			CFlowDestination* destArc = pFlowPair->GetDestProcArcInfo( i );

			_vProc.push_back( *destArc );

			GroupIndex groupSource = m_pTerm->procList->getGroupIndex ( _rootID );
			GroupIndex groupDest = m_pTerm->procList->getGroupIndex(destArc->GetProcID());

			if (destArc->GetOneToOneFlag())
			{
				ProcessorArray proArray;
				
				for (int n = groupDest.start; n <= groupDest.end; n++)
				{
					Processor* pDestProc = m_pTerm->procList->getProcessor(n);
					
					proArray.addItem(pDestProc);
				}

				for (int m = groupSource.start; m <= groupSource.end; m++)
				{
					Processor* pSourceProc = m_pTerm->procList->getProcessor( m );
					const ProcessorID* sourceID = pSourceProc->getID();
					GroupIndex sourceIndex = m_pTerm->procList->getGroupIndex(*sourceID);

					Processor* pReslutProc = CFlowChannel::GetOutChannelProc(*pSourceProc,proArray,destArc->GetProcID().idLength());
					if (pReslutProc)
					{
						pReslutProc->AddGateFlowPairProcessor(m);
						pReslutProc->AddReclaimFlowPairProcessor(m);
						pReslutProc->AddLandsideFacilityFlowPair(m);
					}
				}
			}
			else
			{
				for (int n = groupDest.start; n <= groupDest.end; n++)
				{
					for (int m = groupSource.start; m <= groupSource.end; m++)
					{
						m_pTerm->procList->getProcessor( n )->AddGateFlowPairProcessor(m);
						m_pTerm->procList->getProcessor( n )->AddReclaimFlowPairProcessor(m);
						m_pTerm->procList->getProcessor( n )->AddLandsideFacilityFlowPair(m);
					}
				}
			}

			if( std::find(m_vFlowDestion.begin(),m_vFlowDestion.end(),*destArc) == m_vFlowDestion.end() || pFlowPair->FlowDistinationUnVisit())
			{
				if (!RepeatPattern( _vProc ))
				{
					destArc->SetVisitFlag(true);
					m_vFlowDestion.push_back(*destArc);
					BuildAllPathOfSingleTree( destArc->GetProcID(), _vProc , _vAllPossiblePath, pDir);
				}
			}
			
 			for (int n = groupDest.start; n <= groupDest.end; n++)
			{
				Processor* pDestProc = m_pTerm->procList->getProcessor(n);
				if (pDestProc->getProcessorType() == BaggageProc)
				{
					pDestProc->GetReclaimDrection().Insert(*m_pPaxConstrain,*(pDestProc->getID()));
				}

				if (pDestProc->getProcessorType() == GateProc)
				{
					pDestProc->GetDirection().Insert(*m_pPaxConstrain,*(pDestProc->getID()));
				}

 				m_pTerm->procList->getProcessor( n )->notifyGateFlowPair(*m_pPaxConstrain);
 				m_pTerm->procList->getProcessor( n )->notifyReclaimFlowPair(*m_pPaxConstrain);
				m_pTerm->procList->getProcessor( n )->notifyLandsideFacilityFlowPair(*m_pPaxConstrain);
 			}

			_vProc.pop_back();
		}
	}

}



// assumme the SinglePaxTypeFlow is one line flow. not a tree. check if there any repeat processor sublist 
// and also check if three stations in continues position.
bool CSinglePaxTypeFlow::RepeatPattern( std::vector<CFlowDestination>& _vProc )
{
	int nCount = _vProc.size();
	if( nCount >= 3 )
	{
		GroupIndex gIndex1 = m_pTerm->procList->getGroupIndex( _vProc[nCount-1].GetProcID() );
		GroupIndex gIndex2 = m_pTerm->procList->getGroupIndex( _vProc[nCount-2].GetProcID() );
		GroupIndex gIndex3 = m_pTerm->procList->getGroupIndex( _vProc[nCount-3].GetProcID() );
		//if( gIndex1.start <0 )
		//{
		//	//throw new ARCCriticalException( Error_Processor_Not_Found, _vProc[nCount-1].GetProcID().GetIDString() );
		//	throw new ARCProcessNameInvalideError( _vProc[nCount-1].GetProcID().GetIDString(), "in flow" );
		//}
		//if( gIndex2.start<0 )
		//{
		//	//throw new ARCCriticalException( Error_Processor_Not_Found, _vProc[nCount-2].GetProcID().GetIDString() );
		//	throw new ARCProcessNameInvalideError( _vProc[nCount-2].GetProcID().GetIDString(), "in flow" );
		//}
		//if( gIndex3.start < 0 )
		//{
		//	//throw new ARCCriticalException( Error_Processor_Not_Found, _vProc[nCount-3].GetProcID().GetIDString() );
		//	throw new ARCProcessNameInvalideError( _vProc[nCount-3].GetProcID().GetIDString(), "in flow" );
		//}

		//1, 2, 3,
		if(_vProc[nCount-1].GetProcID().GetIDString() == "SKYLINE-A3-NONSHENGEN")
		{
			int i = 0;
			i = 3;
		}
		if(gIndex2.start >= 0
			&& m_pTerm->procList->getProcessor( gIndex2.start )->getProcessorType() == IntegratedStationProc)
		{
			if(gIndex1.start < 0 
				|| m_pTerm->procList->getProcessor( gIndex1.start )->getProcessorType() != IntegratedStationProc)
			{
				if(gIndex3.start < 0
					|| m_pTerm->procList->getProcessor( gIndex3.start )->getProcessorType() != IntegratedStationProc)
					return true;
			}
		}
		
		if(gIndex1.start >= 0 && gIndex2.start >= 0 && gIndex3.start >= 0)
		{
			if( m_pTerm->procList->getProcessor( gIndex1.start )->getProcessorType() == IntegratedStationProc &&
					m_pTerm->procList->getProcessor( gIndex2.start )->getProcessorType() == IntegratedStationProc &&
					m_pTerm->procList->getProcessor( gIndex3.start )->getProcessorType() == IntegratedStationProc )
			{
				return true;
			}

			// one station only is not allowed.
			if( m_pTerm->procList->getProcessor( gIndex1.start )->getProcessorType() != IntegratedStationProc &&
					m_pTerm->procList->getProcessor( gIndex2.start )->getProcessorType() == IntegratedStationProc &&
					m_pTerm->procList->getProcessor( gIndex3.start )->getProcessorType() != IntegratedStationProc )
			{
				return true;
			}
		}
	}

	if( nCount == 2 )
	{
		// one station only is not allowed.
		//// TRACE("\n id =%s\n",_vProc[nCount-1].GetProcID().GetIDString() );
	//	// TRACE("\n id =%s\n",_vProc[nCount-2].GetProcID().GetIDString() );
		GroupIndex gIndex1 = m_pTerm->procList->getGroupIndex( _vProc[nCount-1].GetProcID() );
		GroupIndex gIndex2 = m_pTerm->procList->getGroupIndex( _vProc[nCount-2].GetProcID() );
		//if( gIndex1.start <0 )
		//{
		//	CFlowDestination& tempDest = _vProc[nCount-1];
		//	//throw new ARCCriticalException( Error_Processor_Not_Found, _vProc[nCount-1].GetProcID().GetIDString() );
		//	throw new ARCProcessNameInvalideError(  _vProc[nCount-1].GetProcID().GetIDString(), "in flow" );
		//}
		//if( gIndex2.start<0 )
		//{
		//	//throw new ARCCriticalException( Error_Processor_Not_Found, _vProc[nCount-2].GetProcID().GetIDString() );
		//	throw new ARCProcessNameInvalideError( _vProc[nCount-2].GetProcID().GetIDString(), "in flow" );
		//}
		if(gIndex1.start >= 0 && gIndex2.start >= 0)
		{
			if( m_pTerm->procList->getProcessor( gIndex1.start )->getProcessorType() != IntegratedStationProc &&
					m_pTerm->procList->getProcessor( gIndex2.start )->getProcessorType() == IntegratedStationProc )
			{
				return true;
			}
		}
	}



	//int nStep = 0;	// number item to compare.
	//while( ++nStep < nCount / 2 )
	//{
	//	// check if [nCount-1, nCount-2....nCount-nStep] is the same as [nCount-nStep-1,...nCount-2nStep]
	//	bool bRepeat = true;
	//	for( int i=0; i<nStep; i++ )
	//	{
	//		// [nCount-1-i] [nCount-nStep-1-i]
	//		if( !( _vProc[nCount-1-i].GetProcID() == _vProc[nCount-nStep-1-i].GetProcID() ) )
	//		{
	//			bRepeat = false;			
	//			break;
	//		}
	//	}
	//	if( bRepeat )
	//		return true;
	//}

	typedef std::vector<CFlowDestination>::iterator FlowDestIter;
	for (FlowDestIter iter1 = _vProc.begin();
		 iter1 != _vProc.end() - 1; ++iter1)
	{
		if( iter1->GetProcID() == _vProc.back().GetProcID() )
		{
			//need to check the in IntegratedStationProc processor
			//if the two processor are same procs,but if it is a IntegratedStationProc, it is legal
			GroupIndex gIndexProc = m_pTerm->procList->getGroupIndex(_vProc.back().GetProcID() );
			if(!(m_pTerm->procList->getProcessor( gIndexProc.start )->getProcessorType() == IntegratedStationProc))
				return true;
		}
	}

	return false;
}

void CSinglePaxTypeFlow::BuildLineFlowFromVector( std::vector<CFlowDestination>& _vProc )
{
	int nCount = _vProc.size();
	if( nCount < 1 )
		return;
	ProcessorID procID = _vProc[0].GetProcID();
	AddIsolatedProc( procID );
	for( int i=1; i<nCount; i++ )
	{
		//CFlowDestination info;
		//info.SetProcID( _vProc[i] );
		AddDestProc( procID, _vProc[i] );
		procID = _vProc[i].GetProcID();
	}
}

bool CSinglePaxTypeFlow::getUsedProInfo(const ProcessorID& id,InputTerminal *_pTerm,std::vector<CString>& _strVec,const CString& strSubFlowName)
{
	CProcessorDestinationList *entry;

	for (int i = GetFlowPairCount() - 1; i >= 0; i--)
	{
		entry = (CProcessorDestinationList *)GetFlowPairAt (i);
		if (id.idFits (entry->GetProcID()))
		{
			CString _str = _T("");
			_str.Format(_T("%s is referenced in %s"),entry->GetProcID().GetIDString(),strSubFlowName);
			_strVec.push_back(_str);
			return true;
		}
		else if(entry->GetProcID().idFits(id) && !entry->GetProcID().isBlank())
		{
			GroupIndex gFirstIndex = _pTerm->GetProcessorList()->getGroupIndex(entry->GetProcID());
			GroupIndex gSecondIndex = _pTerm->GetProcessorList()->getGroupIndex(id);
			if (gFirstIndex.start == gSecondIndex.start
				&& gFirstIndex.end == gSecondIndex.end)
			{
				CString _str = _T("");
				_str.Format(_T("%s is referenced in %s"),entry->GetProcID().GetIDString(),strSubFlowName);
				_strVec.push_back(_str);
				return true;
			}
			else if (gFirstIndex.start == gFirstIndex.end)
			{
				CString _str = _T("");
				_str.Format(_T("%s is referenced in %s"),entry->GetProcID().GetIDString(),strSubFlowName);
				_strVec.push_back(_str);
				return true;
			}
		}
	}
	return false;
}

bool CSinglePaxTypeFlow::IfUseThisPipe(int nCurPipeIndex)const
{
	for (int i = 0; i < GetFlowPairCount(); i++)
	{
		if (GetFlowPairAt(i).IfUseThisPipe(nCurPipeIndex))
		{
			return true;
		}
	}
	return false;
}