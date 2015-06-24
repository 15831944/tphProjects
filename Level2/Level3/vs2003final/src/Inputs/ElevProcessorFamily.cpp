#include "StdAfx.h"
#include ".\elevprocessorfamily.h"

ElevProcessorFamily::ElevProcessorFamily(void)
:ProcessorFamily()
{
}

ElevProcessorFamily::~ElevProcessorFamily(void)
{
}

bool ElevProcessorFamily::GetMaxMinFloorOfFamily( ProcessorList* pProcList, int& _nMaxFloor, int& _nMinFloor )
{
		int MaxFloorOfFamily=-1;
		int MinFloorOfFamily=-1;
		for (int procIndex=m_nStartIndexInProcList;procIndex<=m_nEndIndexInProcList;procIndex++)
		{
			Processor* pProcessor = pProcList->getProcessor( procIndex );
			ElevatorProc* pEleVatorProcessor=(ElevatorProc*)pProcessor;
			int tempMaxFloor=pEleVatorProcessor->GetMaxFloor();
			int tempMinFloor=pEleVatorProcessor->GetMinFloor();

			if (  MaxFloorOfFamily<tempMaxFloor )
				MaxFloorOfFamily=tempMaxFloor;

			if ( MinFloorOfFamily==-1 || MinFloorOfFamily>tempMinFloor)
				MinFloorOfFamily=tempMinFloor;
			
		}

		_nMaxFloor=MaxFloorOfFamily;
		_nMinFloor=MinFloorOfFamily;

		if (MaxFloorOfFamily==-1||MinFloorOfFamily==-1)
		{
			return FALSE;
		} 
		else
		{
			return TRUE;
		}
	

}


