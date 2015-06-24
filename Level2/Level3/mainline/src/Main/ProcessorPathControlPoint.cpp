#include "StdAfx.h"
#include "./3DView.h"
#include "./../Common/path.h"
#include ".\processorpathcontrolpoint.h"
#include "./ShapeRenderer.h"

CProcessorPathControlPoint::CProcessorPathControlPoint(Path& path,int ptID,CProcessor2 * pOwner):
PathControlPoint(path,ptID),m_pOwner(pOwner)
{
}

CProcessorPathControlPoint::~CProcessorPathControlPoint(void)
{
}

void CProcessorPathControlPoint::DrawSelect( C3DView * pView )
{
	glNormal3i(0,0,1);

	ARCPoint3 pt = GetLocation();
	DrawFlatSquare(pt[VX],pt[VY],0,100);
}

CProcessorControlPointManager::CProcessorControlPointManager(void)
{
	m_bVisible = false;
}
CProcessorControlPointManager::~CProcessorControlPointManager(void)
{
	Clear();
}
void CProcessorControlPointManager::Draw(C3DView * pView)
{
	if(m_bVisible)
	{
		ProcessorPathControlPointList::iterator itr;
		for(itr = m_vServiceLocationControlPoints.begin();itr != m_vServiceLocationControlPoints.end();itr++)
			(*itr)->DrawSelect(pView);	
		for(itr = m_vQueueControlPoints.begin();itr != m_vQueueControlPoints.end();itr++)
			(*itr)->DrawSelect(pView);	
		for(itr = m_vInConstraintControlPoints.begin();itr != m_vInConstraintControlPoints.end();itr++)
			(*itr)->DrawSelect(pView);	
		for(itr = m_vOutConstraintControlPoints.begin();itr != m_vOutConstraintControlPoints.end();itr++)
			(*itr)->DrawSelect(pView);	
	}
}

void CProcessorControlPointManager::DrawServiceLocationControlPoints(C3DView * pView)
{
	if(m_bVisible)
	{
		ProcessorPathControlPointList::iterator itr;
		for(itr = m_vServiceLocationControlPoints.begin();itr != m_vServiceLocationControlPoints.end();itr++)
			(*itr)->DrawSelect(pView);	
	}
}
void CProcessorControlPointManager::DrawQueueControlPoints(C3DView * pView)
{
	if(m_bVisible)
	{
		ProcessorPathControlPointList::iterator itr;
		for(itr = m_vQueueControlPoints.begin();itr != m_vQueueControlPoints.end();itr++)
			(*itr)->DrawSelect(pView);	
	}
}
void CProcessorControlPointManager::DrawInConstraintControlPoints(C3DView * pView)
{
	if(m_bVisible)
	{
		ProcessorPathControlPointList::iterator itr;
		for(itr = m_vInConstraintControlPoints.begin();itr != m_vInConstraintControlPoints.end();itr++)
			(*itr)->DrawSelect(pView);	
	}
}
void CProcessorControlPointManager::DrawOutConstraintControlPoints(C3DView * pView)
{
	if(m_bVisible)
	{
		ProcessorPathControlPointList::iterator itr;
		for(itr = m_vOutConstraintControlPoints.begin();itr != m_vOutConstraintControlPoints.end();itr++)
			(*itr)->DrawSelect(pView);	
	}
}

void CProcessorControlPointManager::DrawSelect(C3DView * pView)
{
	if(m_bVisible)
	{
		ProcessorPathControlPointList::iterator itr;
		for(itr = m_vServiceLocationControlPoints.begin();itr != m_vServiceLocationControlPoints.end();itr++)
		{
			glLoadName( pView->GetSelectionMap().NewSelectable( (*itr).get() ) );
			(*itr)->DrawSelect(pView);	
		}
		for(itr = m_vQueueControlPoints.begin();itr != m_vQueueControlPoints.end();itr++)
		{
			glLoadName( pView->GetSelectionMap().NewSelectable( (*itr).get() ) );
			(*itr)->DrawSelect(pView);	
		}
		for(itr = m_vInConstraintControlPoints.begin();itr != m_vInConstraintControlPoints.end();itr++)
		{
			glLoadName( pView->GetSelectionMap().NewSelectable( (*itr).get() ) );
			(*itr)->DrawSelect(pView);	
		}
		for(itr = m_vOutConstraintControlPoints.begin();itr != m_vOutConstraintControlPoints.end();itr++)
		{
			glLoadName( pView->GetSelectionMap().NewSelectable( (*itr).get() ) );
			(*itr)->DrawSelect(pView);	
		}
	}
}

void CProcessorControlPointManager::DrawSelectServiceLocationControlPoints(C3DView * pView)
{
	if(m_bVisible)
	{
		ProcessorPathControlPointList::iterator itr;
		for(itr = m_vServiceLocationControlPoints.begin();itr != m_vServiceLocationControlPoints.end();itr++)
		{
			glLoadName( pView->GetSelectionMap().NewSelectable( (*itr).get() ) );
			(*itr)->DrawSelect(pView);	
		}
	}
}
void CProcessorControlPointManager::DrawSelectQueueControlPoints(C3DView * pView)
{
	if(m_bVisible)
	{
		ProcessorPathControlPointList::iterator itr;
		for(itr = m_vQueueControlPoints.begin();itr != m_vQueueControlPoints.end();itr++)
		{
			glLoadName( pView->GetSelectionMap().NewSelectable( (*itr).get() ) );
			(*itr)->DrawSelect(pView);	
		}
	}
}
void CProcessorControlPointManager::DrawSelectInConstraintControlPoints(C3DView * pView)
{
	if(m_bVisible)
	{
		ProcessorPathControlPointList::iterator itr;
		for(itr = m_vInConstraintControlPoints.begin();itr != m_vInConstraintControlPoints.end();itr++)
		{
			glLoadName( pView->GetSelectionMap().NewSelectable( (*itr).get() ) );
			(*itr)->DrawSelect(pView);	
		}
	}
}
void CProcessorControlPointManager::DrawSelectOutConstraintControlPoints(C3DView * pView)
{
	if(m_bVisible)
	{
		ProcessorPathControlPointList::iterator itr;
		for(itr = m_vOutConstraintControlPoints.begin();itr != m_vOutConstraintControlPoints.end();itr++)
		{
			glLoadName( pView->GetSelectionMap().NewSelectable( (*itr).get() ) );
			(*itr)->DrawSelect(pView);	
		}
	}
}

void CProcessorControlPointManager::Clear(void)
{	
	m_vServiceLocationControlPoints.clear();
	m_vQueueControlPoints.clear();
	m_vInConstraintControlPoints.clear();
	m_vOutConstraintControlPoints.clear();
}

void CProcessorControlPointManager::Initialize(Path * ServiceLocationPath,Path * QueuePath,Path * InConstraintPath,Path * OutConstraint,CProcessor2 * pOwner)
{
	Clear();
	int i = 0;
	if(ServiceLocationPath)
		for(i = 0;i < ServiceLocationPath->getCount();i++)
			m_vServiceLocationControlPoints.push_back( new CProcessorPathControlPoint(*ServiceLocationPath,i,pOwner) );

	if(QueuePath)
		for(i = 0;i < QueuePath->getCount();i++)
			m_vQueueControlPoints.push_back( new CProcessorPathControlPoint(*QueuePath,i,pOwner) );

	if(InConstraintPath)
		for(i = 0;i < InConstraintPath->getCount();i++)
			m_vInConstraintControlPoints.push_back( new CProcessorPathControlPoint(*InConstraintPath,i,pOwner) );

	if(OutConstraint)
		for(i = 0;i < OutConstraint->getCount();i++)
			m_vOutConstraintControlPoints.push_back( new CProcessorPathControlPoint(*OutConstraint,i,pOwner) );
}