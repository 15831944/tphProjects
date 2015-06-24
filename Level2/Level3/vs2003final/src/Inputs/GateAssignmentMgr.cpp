#include "stdafx.h"
#include "GateAssignmentMgr.h"
#include "../Common/ALTObjectID.h"
#include "schedule.h"
#include "../InputAirside/CParkingStandBuffer.h"
#include "../InputAirside/TowOffStandAssignmentDataList.h"
#include "IntermediateStandAssign.h"
#include "flight.h"
#include "in_term.h"


static bool StandOrGateCompare(ref_ptr_bee<CAssignGate> obj1, ref_ptr_bee<CAssignGate> obj2 )
{
	return (ALTObjectID(obj1->GetName())< ALTObjectID(obj2->GetName()));
}

static bool FlightAssignOperationCompare(CFlightOperationForGateAssign* pFlight1, CFlightOperationForGateAssign* pFlight2)
{
	if(pFlight1->GetStartTime() < pFlight2->GetStartTime())
		return true;
	else 
		return false;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
CGateAssignmentMgr::CGateAssignmentMgr(int nProjID)
:m_nProjID(nProjID)
{

}

CGateAssignmentMgr::~CGateAssignmentMgr(void)
{
	int nSize = m_vectUnassignedFlight.size();
	for (int i = 0; i < nSize; i++)
	{
		FlightForAssignment* pFlight = m_vectUnassignedFlight.at(i);
		if (pFlight)
		{
			delete pFlight;
			pFlight = NULL;
		}
	}
	m_vectUnassignedFlight.clear();

	nSize = m_vAssignedFlight.size();
	for (int i = 0; i < nSize; i++)
	{
		FlightForAssignment* pFlight = m_vAssignedFlight.at(i);
		if (pFlight)
		{
			delete pFlight;
			pFlight = NULL;
		}
	}
	m_vAssignedFlight.clear();

}

int CGateAssignmentMgr::GetGateCount()
{
	return m_vectGate.size();
}

FlightOperationPostion CGateAssignmentMgr::GetFlightOperationPostion(CFlightOperationForGateAssign& flight)
{
	FlightOperationPostion Pos;
	for (size_t i = 0; i < m_vectGate.size();i++)
	{
		int nItem = m_vectGate[i]->GetFlightIdx(flight);
		if (nItem != -1)
		{
			Pos.mnLine = i;
			Pos.mnItem = nItem;
			break;
		}
	}
	return Pos;
}

CAssignGate* CGateAssignmentMgr::GetGate(int _nIdx)
{
	assert( _nIdx >= 0 && _nIdx < static_cast<int>(m_vectGate.size()) );
	return &*m_vectGate[_nIdx];
}

// set for all flight on the gate.
//_bOnly represents 'exclusive'
void CGateAssignmentMgr::SetAssignedFlightSelected( int _nGateIndex, bool _bOnly )
{
	if( _bOnly )
	{
		int nGateCount = m_vectGate.size();
		for( int i=0; i<nGateCount; i++ )
		{
			int nFlightCount = m_vectGate[i]->GetFlightCount();
			for( int j=0; j<nFlightCount; j++ )
			{
				m_vectGate[i]->SetFlightSelected( j, false );
			}
		}		
	}

	int nFlightCount = m_vectGate[_nGateIndex]->GetFlightCount();
	for( int i=0; i<nFlightCount; i++ )
	{
		m_vectGate[_nGateIndex]->SetFlightSelected( i, true );
	}
}


//_bOnly represents 'exclusive'
void CGateAssignmentMgr::SetAssignedFlightSelected(int _nGateIndex, int _nFlightIndex, bool _bOnly )
{
	if( _bOnly )
	{
		int nGateCount = m_vectGate.size();
		for( int i=0; i<nGateCount; i++ )
		{
			int nFlightCount = m_vectGate[i]->GetFlightCount();
			for( int j=0; j<nFlightCount; j++ )
			{
				m_vectGate[i]->SetFlightSelected( j, false );
			}
		}		
	}
	if( _nFlightIndex > -1 )
	{
		m_vectGate[_nGateIndex]->SetFlightSelected( _nFlightIndex, true );
	}
}

FlightForAssignment* CGateAssignmentMgr::getSelectedFlight(int _nGateIndex, int _nFlightIndex)
{
	if (_nFlightIndex <0 || _nGateIndex < 0)
		return NULL;

	if (_nGateIndex< GetGateCount() && _nFlightIndex < m_vectGate[_nGateIndex]->GetFlightCount() )
	{
		CFlightOperationForGateAssign flight = m_vectGate[_nGateIndex]->GetFlight(_nFlightIndex);
		return flight.GetAssignmentFlight();
	}
	return NULL;
}

// -1 for toggle
// 0 for unselected
// 1 for seleced.
void CGateAssignmentMgr::SetUnAssignedFlightSelected(int _nFlightIndex, int _nState)
{
	bool bSel = false;
	if( _nState == 1 || 
		( _nState == -1 && !m_vectUnassignedFlight[_nFlightIndex]->IsSelected() ) )
		bSel = true;
	m_vectUnassignedFlight[_nFlightIndex]->SetSelected( bSel );	
}

//void CGateAssignmentMgr::RemoveSelectedFlight()
//{
//	std::vector<CFlightOperationForGateAssign> vectFlight;
//	vectFlight.clear();
//	int nGateCount = m_vectGate.size();
//	for( int i=0; i<nGateCount; i++ )
//	{
//		m_vectGate[i]->RemoveSelectedFlight( vectFlight );
//	}
//	int nSize = vectFlight.size();
//	std::vector<FlightForAssignment*>::iterator iter;
//	for (int i =0; i < nSize; i++)
//	{
//		FlightForAssignment* pFlight = vectFlight[i].GetAssignmentFlight();
//		iter = std::find(m_vAssignedFlight.begin(),m_vAssignedFlight.end(),pFlight);
//		if (iter != m_vAssignedFlight.end())
//		{
//			m_vAssignedFlight.erase(iter);
//		}
//		iter = std::find(m_vectUnassignedFlight.begin(),m_vectUnassignedFlight.end(),pFlight);
//		if (iter != m_vectUnassignedFlight.end())
//			continue;
//
//		pFlight->ClearAllOperations();
//		m_vectUnassignedFlight.push_back(pFlight);
//	} 	
//}

void CGateAssignmentMgr::GetUnassignedFlight(std::vector<CFlightOperationForGateAssign*>& vFlightForGateAssignList)
{
	int nCount = m_vectUnassignedFlight.size();
	for (int i = 0; i < nCount; i++)
	{

		FlightForAssignment* pflight = m_vectUnassignedFlight.at(i);
		if (pflight->GetFlightArrPart())
			vFlightForGateAssignList.push_back(pflight->GetFlightArrPart());
		if (pflight->GetFlightDepPart())
			vFlightForGateAssignList.push_back(pflight->GetFlightDepPart());		
		
	}
}

std::vector<FlightForAssignment*>& CGateAssignmentMgr::GetUnassignedScheduleFlightList()
{
	return m_vectUnassignedFlight;
}

void CGateAssignmentMgr::Save( InputTerminal* _pInTerm, const CString& _csProjPath )
{
	int nCount = GetGateCount();
	for (int i = 0; i < nCount ; i++)
	{
		CAssignGate* pGate = GetGate(i);
		pGate->SaveFlightAssignment();

	}
	//int nUnAssignFlight = m_vectUnassignedFlight.size();
	//for (int j =0; j < nUnAssignFlight; j++)
	//{
	//	m_vectUnassignedFlight[j]->EmptyStand();
	//	m_vectUnassignedFlight[j]->EmptyGate();
	//}
	_pInTerm->flightSchedule->saveDataSet( _csProjPath, false );

}

void CGateAssignmentMgr::sortAllGateByName()
{
	std::sort( m_vectGate.begin(), m_vectGate.end(), StandOrGateCompare);
	return;
}


// un-assign all flight
int CGateAssignmentMgr::UnassignAllFlight( void )
{
	// make all flight selected
	for( unsigned i= 0; i< m_vectGate.size(); i++ )
	{
		for( int j=0; j<m_vectGate[i]->GetFlightCount(); j++ )
		{
			m_vectGate[i]->SetFlightSelected( j, true );
		}
	}
	// un-assign all gate;
	RemoveSelectedFlight();

	for( unsigned i= 0; i< m_vectGate.size(); i++ )
	{
		for( int j=0; j<m_vectGate[i]->GetFlightCount(); j++ )
		{
			m_vectGate[i]->SetFlightSelected( j, false );
		}
	}

	return 1;
}

//these code is  copied from the CGateAssignDlg::ReverseSort( ), 
// i think they should be here. TUTU in 2003-11-6
void CGateAssignmentMgr::ReverseSort(std::vector<CFlightOperationForGateAssign*>& vFlights,bool bReverseSort)
{
	if (bReverseSort)
		std::sort(vFlights.end(),vFlights.begin(),FlightAssignOperationCompare);
	else
		std::sort(vFlights.begin(),vFlights.end(),FlightAssignOperationCompare);
}

CAssignGate* CGateAssignmentMgr::GetGate(const CString& strName)
{
	CAssignGate* pRetGate = NULL;
	int nCount = GetGateCount();
	for (int i =0;i < nCount; i++)
	{
		CAssignGate*  pTempGate = GetGate(i);
		if(pTempGate &&pTempGate->GetName().CompareNoCase(strName) == 0)
		{
			pRetGate = pTempGate;
			break;
		}
	}
	return pRetGate;
}

void CGateAssignmentMgr::AdjustAssignedFlight(std::vector<CFlightOperationForGateAssign*>& vUnAssignedOperations)
{

	m_vAssignedFlight.insert(m_vAssignedFlight.end(), m_vectUnassignedFlight.begin(),m_vectUnassignedFlight.end());
	m_vectUnassignedFlight.clear();
	int nSize = vUnAssignedOperations.size();
	for (int i =0; i < nSize; i++)
	{
		CFlightOperationForGateAssign* pFlight = vUnAssignedOperations.at(i);
		RemoveAssignedFlight(pFlight->GetAssignmentFlight());
	} 
}

void CGateAssignmentMgr::GetUnAssignedFlightSelected( std::vector<FlightForAssignment*>& vFlightAssignmentList )
{
	for (unsigned i = 0; i < m_vectUnassignedFlight.size(); i++)
	{
		FlightForAssignment* pFlight = m_vectUnassignedFlight[i];
		if (pFlight && pFlight->IsSelected())
		{
			vFlightAssignmentList.push_back(pFlight);
		}
	}
}

