#include "StdAfx.h"
#include ".\renderdeicepad3d.h"

#include "RenderAirport3D.h"
#include "OgreConvert.h"
#include "ShapeBuilder.h"
#include "Shape3DMeshManager.h"
#include <InputAirside/ALTObjectDisplayProp.h>
#include <Common/ARCPath.h>

using namespace Ogre;

void CRenderDeicePad3D::Build(DeicePad* pDeicepad)
{

	DeicePadDisplayProp dispProp;
	dispProp.ReadData(pDeicepad->getID());
	DeicePadDisplayProp* pDispProp = &dispProp;

	//CPoint2008 serverpt = pDeicepad->GetServicePoint();
	const Path& inConstrain = pDeicepad->GetInContrain();
	ARCVector3 centerPt = pDeicepad->GetServicePoint();
	SetPosition(centerPt);

	if(pDispProp->m_dpShape.bOn)
	{
		//// 
		//if (!m_shapeStandBar)
		//{
		//	m_shapeStandBar = CreateNewChild();
		//}
		Entity * pEnt = _GetOrCreateEntity(GetName()+_T("Shape"),_T("StandBar.mesh"));//OgreUtil::createOrRetrieveEntity(GetIDName() + _T("-Entity"), , GetScene());
		if (pEnt)
		{
			AddObject(pEnt);
		}

		//m_shapeStandBar.SetPosition(pt);
	}
	

	/*if(pDispProp->m_dpTruck.bOn)
	{
		ARCVector2 vdir = pDeicepad->GetDir();
		double dAngleFromX =- ARCMath::RadiansToDegrees(acos(vdir[VX]));
		ARCVector2 vLength = 0.4 * pDeicepad->GetLength()* vdir;
		vdir.Rotate(90);
		ARCVector2 vSpan = 0.4 * pDeicepad->GetWinSpan() * vdir;

		ARCVector3 vpos1 = ARCVector3(serverpt) + ARCVector3( vSpan[VX],vSpan[VY], 0) + ARCVector3(vLength[VX],vLength[VY], 0.0);
		ARCVector3 vpos2 = ARCVector3(serverpt) - ARCVector3( vSpan[VX],vSpan[VY], 0) + ARCVector3(vLength[VX],vLength[VY], 0.0);		

		CString strMesh;
		VERIFY(CShape3DMeshManager::GetInstance().GetVehicleShapeName(strMesh, _T("Deicing truck")));
		if (!m_shapeTruck1)
		{
			m_shapeTruck1 = CreateNewChild();
		}
		Entity* pEnt1 = OgreUtil::createOrRetrieveEntity(GetIDName() + _T("-Truck1"), strMesh, GetScene());
		if (pEnt1)
		{
			m_shapeTruck1.AddObject(pEnt1,true);
		}
		m_shapeTruck1.SetPosition(vpos1);
		m_shapeTruck1.SetRotation(ARCVector3(0.0, 0.0, dAngleFromX));

		if (!m_shapeTruck2)
		{
			m_shapeTruck2 = CreateNewChild();
		}
		Entity* pEnt2 = OgreUtil::createOrRetrieveEntity(GetIDName() + _T("-Truck2"), strMesh, GetScene());
		if (pEnt2)
		{
			m_shapeTruck2.AddObject(pEnt2,true);
		}
		m_shapeTruck2.SetPosition(vpos2);
		m_shapeTruck2.SetRotation(ARCVector3(0.0, 0.0, dAngleFromX));
	}
*/
	ManualObject* pObj = _GetOrCreateManualObject(GetName()+"Other");
	pObj->clear();

	//draw in constrain
	if( pDispProp->m_dpInConstr.bOn && !pDeicepad->IsUsePushBack())
	{
		Ogre::ColourValue clr = OgreConvert::GetColor(pDispProp->m_dpInConstr.cColor);

		const CPath2008& path = pDeicepad->GetInContrain();
		int nCount = path.getCount();
		ARCPath3 drawPath(path);
		drawPath.DoOffset(-centerPt);

		ProcessorBuilder::DrawColorPath(pObj, clr, drawPath, 200, false, 0.1);
		ProcessorBuilder::DrawArrow(pObj, Point(drawPath[nCount-2].x, drawPath[nCount-2].y, 0.1),
			Point(drawPath[nCount-1].x, drawPath[nCount-1].y, 0.1), clr, 800.0, 800.0);

// 		if(IsInEdit()){	
// 			glNormal3f(0,0,1);
// 			for(ALTObjectControlPoint2008List::iterator itr = m_vInConstPoint.begin();itr!=m_vInConstPoint.end();itr++){
// 				(*itr)->DrawSelect(pView);		 
// 			}			
// 		}
	}

	//draw out constrain
	if( pDispProp->m_dpOutConstr.bOn && !pDeicepad->IsUsePushBack())
	{
		Ogre::ColourValue clr = OgreConvert::GetColor(pDispProp->m_dpOutConstr.cColor);

		const CPath2008& path = pDeicepad->GetOutConstrain();
		int nCount = path.getCount();
		ARCPath3 drawPath(path);
		drawPath.DoOffset(-centerPt);

		ProcessorBuilder::DrawColorPath(pObj, clr, drawPath, 200, false, 0.1);
		ProcessorBuilder::DrawArrow(pObj, Point(drawPath[nCount-2].x, drawPath[nCount-2].y, 0.1),
			Point(drawPath[nCount-1].x, drawPath[nCount-1].y, 0.1), clr, 800.0, 800.0);

// 		if(IsInEdit()){		
// 			glNormal3f(0,0,1);			
// 			for(ALTObjectControlPoint2008List::iterator itr = m_vOutConstPoint.begin();itr!=m_vOutConstPoint.end();itr++){
// 				(*itr)->DrawSelect(pView);		 
// 			}			
// 		}
	}
	//draw Push Back ways
	if(pDeicepad->IsUsePushBack())
	{
		Ogre::ColourValue clr = OgreConvert::GetColor(pDispProp->m_dpOutConstr.cColor);

		const CPath2008& path = pDeicepad->GetInContrain();
		int nCount = path.getCount();
		ARCPath3 drawPath(path);
		drawPath.DoOffset(-centerPt);

		ProcessorBuilder::DrawColorPath(pObj, clr, drawPath, 200, false, 0.1);
		ProcessorBuilder::DrawArrow(pObj, Point(drawPath[nCount-2].x, drawPath[nCount-2].y, 0.1),
			Point(drawPath[nCount-1].x, drawPath[nCount-2].y, 0.1), clr, 800.0, 800.0);
		ProcessorBuilder::DrawArrow(pObj, Point(drawPath[1].x, drawPath[1].y, 0.1),
			Point(drawPath[0].x, drawPath[0].y, 0.1), clr, 800.0, 800.0);

// 		if(IsInEdit()){		
// 			glNormal3f(0,0,1);
// 			for(ALTObjectControlPoint2008List::iterator itr = m_vInConstPoint.begin();itr!=m_vInConstPoint.end();itr++){
// 				(*itr)->DrawSelect(pView);		 
// 			}			
// 		}
	}

	if ( pDispProp->m_dpPad.bOn)
	{
		Ogre::ColourValue clr = OgreConvert::GetColor(pDispProp->m_dpPad.cColor);

		ARCVector2 vdir = pDeicepad->GetDir();
		ARCPath3 drawPath;
		drawPath.push_back(centerPt);
		drawPath.push_back(ARCVector3(centerPt) - ARCVector3(vdir, 0.0)*(pDeicepad->GetLength()));
		drawPath.DoOffset(-centerPt);

		ProcessorBuilder::DrawColorPath(pObj, clr, drawPath, pDeicepad->GetWinSpan(), false, 0.1);
	}
	AddObject(pObj);
}

void CRenderDeicePad3D::Update3D( ALTObject* pBaseObj )
{
	//ManualObject* pObj = _GetOrCreateManualObject(GetName());
	Build((DeicePad*)pBaseObj);
	//AddObject(pObj);
}

//IMPLEMENT_AIRSIDE3DOBJECT_SIMPLE_UPDATE(CRenderDeicePad3D, DeicePad);


