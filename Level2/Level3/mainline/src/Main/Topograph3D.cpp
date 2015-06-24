#include "StdAfx.h"
#include ".\..\InputAirside\Topography.h"
#include ".\topograph3d.h"
#include ".\..\InputAirside\Surface.h"
#include ".\Surface3D.h"
#include "./3Dview.h"
#include "SelectableSettings.h"
#include "Contour3D.h"
#include ".\..\InputAirside\ALTAirport.h"
#include "Airside3D.h"
#include "Airport3D.h"
#include "TermPlanDoc.h"
#include "Structure3D.h"
#include "ReportingArea3D.h"

CTopograph3D::CTopograph3D( int id ) : m_nID(id)
{
	Init();
}
CTopograph3D::~CTopograph3D(void)
{
}

ALTObject3D * CTopograph3D::GetObject3D( int id )
{
	//get surface object
	//According the id, we can return the object of Surface's and Contour's 
	for(size_t i=0;i<m_vSurface.size();++i){
		if( m_vSurface[i]->GetID() == id ) return m_vSurface[i].get();
	}

	//get contour object
	for(size_t i=0;i<m_vContour.size();++i){
		if( m_vContour[i]->GetID() == id ) return m_vContour[i].get();
	}

	//get structure object
	for(size_t i=0;i<m_vStructure.size();++i){
		if( m_vStructure[i]->GetID() == id ) return m_vStructure[i].get();
	}

	//get reporting area object
	for(size_t i=0;i<m_vReportingArea.size();++i){
		if( m_vReportingArea[i]->GetID() == id ) return m_vReportingArea[i].get();
	}
	return NULL;
}

void CTopograph3D::UpdateAddorRemoveObjects()
{
	std::vector<int> objectIDs;

	//update surface
	//According the m_nID, we can get the objectIDs, it's a vector
	Topography::GetSurfaceList(m_nID,objectIDs);

	ALTObject3DList newSurfaceList;
	for(size_t i=0;i<objectIDs.size();++i)
	{
		ALTObject3DList::iterator theItr;
		if(  m_vSurface.end() == (theItr=find_if( m_vSurface.begin(),m_vSurface.end(), ALTObject3DIDIs(objectIDs[i]) ) ) )
		{
			CSurface3D * pSurface = new CSurface3D(objectIDs[i]);
			newSurfaceList.push_back(pSurface);
			pSurface->Update();
		}
		else 
		{
			newSurfaceList.push_back(*theItr);
			m_vSurface.erase(theItr);
		}
	}
	m_vSurface = newSurfaceList;

	//Update contour
	//objectIDs.clear();
	//Topography::GetContourList(m_nID, objectIDs);

	//ALTObject3DList newContourList;
	//for(size_t i=0;i<objectIDs.size();++i)
	//{
	//	ALTObject3DList::iterator theItr;
	//	if(  m_vContour.end() == (theItr=find_if( m_vContour.begin(),m_vContour.end(), ALTObject3DIDIs(objectIDs[i]) ) ) )
	//	{
	//		CContour3D *pContour = new CContour3D(objectIDs[i]);
	//		newContourList.push_back(pContour);
	//		pContour->Update();
	//	}
	//	else 
	//	{
	//		newContourList.push_back(*theItr);
	//		m_vContour.erase(theItr);
	//	}
	//}
	//m_vContour = newContourList;

	objectIDs.clear();
	Topography::GetRootContourList(m_nID, objectIDs);

	ALTObject3DList newContourList;
	for(size_t i=0;i<objectIDs.size();++i)
	{
		ALTObject3DList::iterator theItr;
		//if(  m_vContour.end() == (theItr=find_if( m_vContour.begin(),m_vContour.end(), ALTObject3DIDIs(objectIDs[i]) ) ) )
		{
			CContour3D *pContour = new CContour3D(objectIDs[i]);
			newContourList.push_back(pContour);
			pContour->Update();
		}
		//else 
		//{
		//	(*theItr)->Update();
		//	newContourList.push_back(*theItr);
		//	m_vContour.erase(theItr);
		//}
	}
	m_vContour = newContourList;

	
	//update Structure
	std::vector<ALTObject> vObjects;
	ALTObject::GetObjectList(ALT_STRUCTURE,m_nID,vObjects);
	ALTObject3DList newStructureList;
	objectIDs.clear();
	for(size_t i=0;i<vObjects.size();++i){ objectIDs.push_back( vObjects[i].getID() ); }

	for(size_t i=0;i<objectIDs.size();++i){	
		ALTObject3DList::iterator theItr;
		if(  m_vStructure.end() == (theItr=find_if( m_vStructure.begin(),m_vStructure.end(), ALTObject3DIDIs(objectIDs[i]) ) ) )
		{
			CStructure3D *pStrucutre = new CStructure3D(objectIDs[i]);
			newStructureList.push_back(pStrucutre);
			pStrucutre->Update();
		}
		else 
		{
			(*theItr)->Update();
			newStructureList.push_back(*theItr);
			m_vStructure.erase(theItr);
		}
	}
	m_vStructure = newStructureList;

	//update ReportingArea
	std::vector<ALTObject> vAreaObjects;
	ALTObject::GetObjectList(ALT_REPORTINGAREA,m_nID,vAreaObjects);
	ALTObject3DList newAreaList;
	objectIDs.clear();
	for(size_t i=0;i<vAreaObjects.size();++i)
	{ 
		objectIDs.push_back( vAreaObjects[i].getID() );
	}

	for(size_t i=0;i<objectIDs.size();++i)
	{	
		ALTObject3DList::iterator theItr;
		if(  m_vReportingArea.end() == (theItr=find_if( m_vReportingArea.begin(),m_vReportingArea.end(), ALTObject3DIDIs(objectIDs[i]) ) ) )
		{
			CReportingArea3D *pArea = new CReportingArea3D(objectIDs[i]);
			newAreaList.push_back(pArea);
			pArea->Update();
		}
		else 
		{
			(*theItr)->Update();
			newAreaList.push_back(*theItr);
			m_vReportingArea.erase(theItr);
		}
	}
	m_vReportingArea = newAreaList;

}

void CTopograph3D::DrawOGL( C3DView * pView )
{
	RenderToDepth(pView);
	//draw surface 	
	glPolygonOffset(0,0);
	//if(m_vSurface.size())
	//{
	//	pView->GetDocument()->m_bIsOpenStencil = TRUE;
	//	pView->GetAirside3D()->GetActiveAirport()->RenderStenciles();
	//	glEnable(GL_STENCIL_TEST);
	//	glStencilFunc(GL_NOTEQUAL,0x1,0x1);
	//	glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
	//	glDepthMask(GL_FALSE);	
	//	//surface

	//	for(size_t i=0;i<m_vSurface.size();++i){
	//		CSurface3D * pSurface = (CSurface3D*)m_vSurface[i].get();
	//		pSurface->RenderSurface(pView);
	//	}	

	//	glDepthMask(GL_TRUE);
	//	glDisable(GL_STENCIL_TEST);
	//}
	//else
	{
		pView->GetDocument()->m_bIsOpenStencil = FALSE;
		glDepthMask(GL_FALSE);	
		//surface

		for(size_t i=0;i<m_vSurface.size();++i){
			CSurface3D * pSurface = (CSurface3D*)m_vSurface[i].get();
			pSurface->RenderSurface(pView);
		}	

		glDepthMask(GL_TRUE);
	}


	//contour
	for (size_t i=0; i<m_vContour.size(); i++)
	{
		CContour3D *pContour = (CContour3D*)m_vContour[i].get();
		pContour->DrawOGL(pView);
	}
	
	//render structure
	for(size_t i=0;i< m_vStructure.size();i++)
	{
		CStructure3D* pStruture = (CStructure3D*)m_vStructure[i].get();
		pStruture->DrawOGL(pView);
	}

	//reporting area
	for(size_t i=0;i< m_vReportingArea.size();i++)
	{
		CReportingArea3D* pArea = (CReportingArea3D*)m_vReportingArea[i].get();
		pArea->DrawOGL(pView);
	}

}

void CTopograph3D::DrawSelectOGL( C3DView *pView, SelectableSettings& selSetting)
{
	//surface
	if( selSetting.m_ALTobjectSelectableMap[ALT_SURFACE] == TRUE)
	{
		for(ALTObject3DList::reverse_iterator itr = m_vSurface.rbegin();itr!=m_vSurface.rend();itr++)
		{
			CSurface3D * pSurface = (CSurface3D*)(*itr).get();
			glLoadName( pView->GetSelectionMap().NewSelectable(pSurface) );
			pSurface->DrawSelect(pView);
		}	
	}
	// draw contours
	if( selSetting.m_ALTobjectSelectableMap[ALT_CONTOUR] == TRUE)
	{
		for(ALTObject3DList::reverse_iterator itr = m_vContour.rbegin();itr!=m_vContour.rend();itr++)
		{
			CContour3D * pContour = (CContour3D*)(*itr).get();
			glLoadName( pView->GetSelectionMap().NewSelectable(pContour) );
			pContour->DrawSelect(pView);
		}	
	}
	//draw structure
	if( selSetting.m_ALTobjectSelectableMap[ALT_STRUCTURE] == TRUE)
	{
		for(ALTObject3DList::reverse_iterator itr = m_vStructure.rbegin();itr!=m_vStructure.rend();itr++)
		{
			CStructure3D * pStructure = (CStructure3D*)(*itr).get();
			glLoadName( pView->GetSelectionMap().NewSelectable(pStructure) );
			pStructure->DrawSelect(pView);
		}	
	}

	//draw reporting area
	if( selSetting.m_ALTobjectSelectableMap[ALT_REPORTINGAREA] == TRUE)
	{
		for(ALTObject3DList::reverse_iterator itr = m_vReportingArea.rbegin();itr!=m_vReportingArea.rend();itr++)
		{
			CReportingArea3D * pArea = (CReportingArea3D*)(*itr).get();
			glLoadName( pView->GetSelectionMap().NewSelectable(pArea) );
			pArea->DrawSelect(pView);
		}	
	}
}

void CTopograph3D::Init()
{
	std::vector<int> objectIDs;
	// get way point Object list
	objectIDs.clear();
	Topography::GetSurfaceList(m_nID,objectIDs);

	for(size_t i=0;i<objectIDs.size();++i){
		// constructor give object's id to the CSurface3D, then we can envoke the Surface with id;
		CSurface3D * pSurface3D = new CSurface3D(objectIDs[i]);
		m_vSurface.push_back(pSurface3D);
		pSurface3D->Update();
	}

	//get contours
	objectIDs.clear();
	ALTAirport::GetRootContoursIDs(m_nID, objectIDs);

	for(size_t i=0;i<objectIDs.size();++i){
		CContour3D * pContour3D = new CContour3D(objectIDs[i]);
		m_vContour.push_back(pContour3D);
		pContour3D->Update();
	}

	//gets structures	
	std::vector<ALTObject> vObjects;
	ALTObject::GetObjectList(ALT_STRUCTURE,m_nID,vObjects);
	for(size_t i=0;i<vObjects.size();++i){
		ALTObject& obj = vObjects[i];
		CStructure3D* pStructure3D = new CStructure3D(obj.getID());
		m_vStructure.push_back(pStructure3D);
		pStructure3D->Update();
	}
	
	//gets reporting area	
	std::vector<ALTObject> vAreaObjects;
	ALTObject::GetObjectList(ALT_REPORTINGAREA,m_nID,vAreaObjects);
	for(size_t i=0;i<vAreaObjects.size();++i){
		ALTObject& obj = vAreaObjects[i];
		CReportingArea3D* pArea3d = new CReportingArea3D(obj.getID());
		m_vReportingArea.push_back(pArea3d);
		pArea3d->Update();
	}

} 

void CTopograph3D::RenderToDepth( C3DView * pView )
{
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(2.0f,2.0f);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL,0x1,0x1);
	glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
	for(size_t i=0;i<m_vSurface.size();++i){
		//Get the object of the Surface with the id, then Render the object.
		CSurface3D * pSurface = (CSurface3D*)m_vSurface[i].get();
		pSurface->RenderSurface(pView);
	}	
	glDisable(GL_POLYGON_OFFSET_FILL);
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
	glDisable(GL_STENCIL_TEST);

	//contour
	/*for (size_t i=0; i<m_vContour.size(); i++)
	{
		CContour3D *pContour = (CContour3D*)m_vContour[i].get();
		pContour->RenderSurface(pView);
	}*/
}

void CTopograph3D::RenderContours( C3DView * pView )
{
	
}

void CTopograph3D::GetObject3DList( ALTObject3DList& objList ) const
{
	objList.insert(objList.end(),m_vSurface.begin(),m_vSurface.end());
	objList.insert(objList.end(),m_vWallShape.begin(),m_vWallShape.end());
	objList.insert(objList.end(),m_vContour.begin(),m_vContour.end());
	objList.insert(objList.end(),m_vStructure.begin(),m_vStructure.end());
	objList.insert(objList.end(), m_vReportingArea.begin(),m_vReportingArea.end());
	
}


ALTObject3D* CTopograph3D::AddObject( ALTObject* pObj )
{
	ALTObject3D * pNewObj3D = NULL;
	switch(pObj->GetType())
	{
	case ALT_SURFACE:
		pNewObj3D = ALTObject3D::NewALTObject3D(pObj);
		m_vSurface.push_back(pNewObj3D);
		break;
	case ALT_STRUCTURE:
		pNewObj3D = ALTObject3D::NewALTObject3D(pObj);
		m_vStructure.push_back(pNewObj3D);
		break;
	case ALT_WALLSHAPE:
		pNewObj3D = ALTObject3D::NewALTObject3D(pObj);
		m_vWallShape.push_back(pNewObj3D);
		break;
	case ALT_CONTOUR:
		pNewObj3D = ALTObject3D::NewALTObject3D(pObj);
		m_vContour.push_back(pNewObj3D);
		break;
	case ALT_REPORTINGAREA:
		pNewObj3D = ALTObject3D::NewALTObject3D(pObj);
		m_vReportingArea.push_back(pNewObj3D);
		break;
	default:
		ASSERT(false);
	}
	return pNewObj3D;
}

Contour* GetContourByID(Contour* pParentContour , int nCountID)
{

	if( pParentContour->getID() == nCountID )
		return pParentContour;

	for(int i=0;i<pParentContour->GetChildCount(); i++)
	{
		Contour* pContour = pParentContour->GetChildItem(i);
		
		Contour* pIDContour = GetContourByID(pContour, nCountID);
		if( pIDContour )
		{
			return pIDContour;
		}

	}
	return NULL;
}

bool CTopograph3D::UpdateChangeOfObject( ALTObject* pObj )
{
	//////////////////////////////////////////////////////////////////////////
	for(size_t i=0;i<m_vSurface.size();++i){
		ALTObject3D* pObj3D = m_vSurface[i].get();
		if(pObj3D->GetID() == pObj->getID() ) 
		{			
			pObj3D->GetObject()->CopyData(pObj);
			pObj3D->InValidate();
			return true;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	for( size_t i=0;i <m_vContour.size(); ++i)
	{
		ALTObject3D* pObj3D = m_vContour[i].get();
		ASSERT(pObj3D&& pObj3D->GetObjectType() == ALT_CONTOUR );
		CContour3D* pContour3D = (CContour3D*)pObj3D;
		Contour* pContour = pContour3D->GetContour();
        Contour* ptheContour = GetContourByID(pContour,pObj->getID());
		if(ptheContour)
		{
			ptheContour->CopyData(pObj);
			pContour3D->InValidate();
			return true;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	for(size_t i=0;i<m_vStructure.size(); ++i)
	{
		ALTObject3D* pObj3D = m_vStructure[i].get();
		if(pObj3D->GetID() == pObj->getID() ) 
		{			
			pObj3D->GetObject()->CopyData(pObj);
			pObj3D->InValidate();
			return true;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	for(size_t i=0;i<m_vReportingArea.size(); ++i)
	{
		ALTObject3D* pObj3D = m_vReportingArea[i].get();
		if(pObj3D->GetID() == pObj->getID() ) 
		{			
			pObj3D->GetObject()->CopyData(pObj);
			pObj3D->InValidate();
			return true;
		}
	}

	return false;
}