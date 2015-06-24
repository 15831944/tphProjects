#include "StdAfx.h"
#include ".\pipedisplay.h"

#include <Inputs/PipeDataSet.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//CPipeDisplay
CPipeDisplay* CPipeDisplayList::GetItem( int idx )
{
	CPipeDisplay *pret = dynamic_cast<CPipeDisplay*>(m_vObjectDisplay[idx]);
	ASSERT(pret);
	return pret;
}
//void CPipeDisplay::OffsetProperties( double dx, double dy )
//{
//	CPipe * pPipe = GetPipe();
//	if(pPipe)
//	{
//		pPipe->MoveObject(dx,dy);
//	}
//}
//
//void CPipeDisplay::RotateProperties( double dr, const ARCVector3& point )
//{
//	CPipe * pPipe = GetPipe();
//	if(pPipe)
//	{
//		pPipe->RotateObject(dr,point);
//	}
//
//}

CPipeDisplay::CPipeDisplay( CPipeDataSet* pPipes,int nIndex )
{
	m_pPipes = pPipes;
	m_nIndex = nIndex;
}

//ARCVector3 CPipeDisplay::GetLocation() const
//{
//	CPipe* pPipe = GetPipe();
//	if(pPipe && pPipe->GetPipePointCount()){
//		PIPEPOINT* ptFirst = pPipe->GetPipePoint(0);
//		return ptFirst->m_point;
//	}
//	else
//	{
//		ASSERT(FALSE);
//		return ARCVector3();
//	}
//}

CPipe* CPipeDisplay::GetPipe() const
{
	ASSERT(m_pPipes);
	ASSERT(m_pPipes->GetPipeCount() > m_nIndex);
	if(m_pPipes && m_nIndex < m_pPipes->GetPipeCount())
	{
		return m_pPipes->GetPipeAt(m_nIndex);
	}
	return NULL;
}

//int CPipeDisplay::GetFloorIndex() const
//{
//	CPipe* pPipe = GetPipe();
//	if(pPipe)
//	{
//		return static_cast<int>(pPipe->GetZ()/SCALE_FACTOR);
//	}
//	else
//	{
//		return -1;
//	}
//}

CBaseObject* CPipeDisplay::GetBaseObject() const
{
	return GetPipe();
}
