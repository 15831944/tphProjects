#include "stdafx.h"
#include ".\renderelevator3d.h"
#include "Render3DScene.h"
#include <CommonData/ProcessorProp.h>
#include <CommonData/ExtraProcProp.h>
#include "COMMON/IARCportLayoutEditContext.h"
#include "common/IEnvModeEditContext.h"
#include "shapebuilder.h"
#include "MaterialDef.h"
using namespace Ogre;


void CRenderElevator3D::Update3D( CProcessor2Base* pProc )
{
	ProcessorProp* pProcProp = pProc->GetProcessorProp();
	ASSERT(pProcProp && pProcProp->getProcessorType() == Elevator);
	ExtraProcProp* pExtraProp = pProcProp->GetExtraProp();
	ASSERT(pExtraProp && pExtraProp->GetPropType() == ExtraProcProp::Elevator_Prop_Type);
	ElevatorProcProp* pElevator = (ElevatorProcProp*)pExtraProp;
	const CProcessor2Base::CProcDispProperties& pdp = pProc->m_dispProperties;

	std::vector<DistanceUnit> dAlt;
	std::vector<BOOL> bOn;
	ITerminalEditContext* pCtxTerminal =   GetRoot().GetModel()->GetTerminalContext();
	ASSERT(pCtxTerminal);
	pCtxTerminal->GetFloorAltOn(dAlt,bOn);

	ARCVector3 parentPos = GetWorldPosition();
	for(size_t i=0;i<dAlt.size();i++)
	{
		dAlt[i]-=parentPos.z;
	}

	ManualObject* pShapeObj = _GetOrCreateManualObject(GetName());//OgreUtil::createOrRetrieveManualObject( GetIDName()+_T("/board"),GetScene() );
	ASSERT(pShapeObj);
	if(!pShapeObj)
		return;
	pShapeObj->clear();

	
	int nMinFloor = pElevator->m_iMinFloor;
	int nMaxFloor = pElevator->m_iMaxFloor;
	ARCColor3 stateOffColor(128,128,128);
	for(int nFloor=nMinFloor; nFloor<=nMaxFloor; nFloor++) {
		std::vector<Pollygon> vAreas;
		std::vector<Point> vDoors;
		pElevator->GetDataAtFloorLayoutData(nFloor,vAreas);
		pElevator->GetDataAtFloorLiftDoors(nFloor,vDoors);

		if(pdp.bDisplay[PDP_DISP_LIFTAREA]) {
			ARCColor3 cDisplay;
			cDisplay = pProc->GetProcessorStateOffTag() ? pdp.color[PDP_DISP_LIFTAREA] : stateOffColor;
			int nAreaCount = static_cast<int>(vAreas.size());
			ColourValue c = OGRECOLOR(/*pdp.color[PDP_DISP_LIFTAREA]*/cDisplay);
			for(int i=0; i<nAreaCount; i++) {
				vAreas[i].DoOffset(-parentPos.x,-parentPos.y,0);
				ProcessorBuilder::DrawPath(pShapeObj,&(vAreas[i]),c,dAlt[nFloor],true);
			}
		}
		if(pdp.bDisplay[PDP_DISP_DOORS]) {
			int nDoorCount = (int)vDoors.size();
			const static Real delta = 70.0f;
			const static Real height = 200.0f;
			ARCColor3 cDisplay;
			cDisplay = pProc->GetProcessorStateOffTag() ? pdp.color[PDP_DISP_DOORS] : stateOffColor;
			ColourValue c = OGRECOLOR(/*pdp.color[PDP_DISP_DOORS]*/cDisplay);
			for(int iDoorIdx=0; iDoorIdx<nDoorCount; iDoorIdx++) {
				//float r1 = (pElevator->GetOrientation());
				//float r2 = (pProc->GetRotation());
				
				Quaternion quat;
				quat.FromAngleAxis(Degree(pProc->GetRotation()),Vector3::UNIT_Z);				
				Vector3 offset(vDoors[iDoorIdx].getX() - parentPos.x, vDoors[iDoorIdx].getY()-parentPos.y,dAlt[nFloor]);
			/*	ShapeTransformer tranf;
				tranf.Rotate(ARCVector3(0,0,1),pProc->GetRotation());
				tranf.Translate( ARCVector3(v) );*/
				
				pShapeObj->begin(VERTEXCOLOR_LIGHTOFF_NOCULL,RenderOperation::OT_TRIANGLE_FAN);
				pShapeObj->position(quat*Vector3(-delta,0,0)+ offset);			
				pShapeObj->colour(c);
				pShapeObj->position(quat*Vector3(delta,0,0)+offset);
				pShapeObj->position(quat*Vector3(delta,0,height)+offset);
				pShapeObj->position(quat*Vector3(-delta,0,height)+offset);
				pShapeObj->end();	
				
			}
		}
	}
	if(pdp.bDisplay[PDP_DISP_SHAFTLINES]) {
		//draw vertical lines between 1st floor and last floor
		ARCColor3 cDisplay;
		cDisplay = pProc->GetProcessorStateOffTag() ? ARCColor3(122,122,122) : stateOffColor;
		ColourValue c = OGRECOLOR(cDisplay);
	//	ColourValue c(122/255.0f,122/255.0f,122/255.0f);		
		std::vector<Pollygon>& liftAreas = pElevator->GetDataAtFloorLiftAreas(nMinFloor);
		int nLiftAreaCount = (int)liftAreas.size();
		for(int iLiftArea=0; iLiftArea<nLiftAreaCount; iLiftArea++) {
			
			Pollygon lifearea = liftAreas[iLiftArea];
			lifearea.DoOffset(-parentPos.x,-parentPos.y,0);
			int ptCount = lifearea.getCount();
			Point* ptList = lifearea.getPointList();

			pShapeObj->begin(VERTEXCOLOR_LIGTHOFF,RenderOperation::OT_LINE_LIST);
			for(int j=0; j<ptCount; j++) {				
				pShapeObj->position(ptList[j].getX(),ptList[j].getY(),dAlt[nMinFloor]);pShapeObj->colour(c);
				pShapeObj->position(ptList[j].getX(),ptList[j].getY(),dAlt[nMaxFloor]);				
			}
			pShapeObj->end();
		}		
	}
	AddObject(pShapeObj,true);


}

void CRenderElevator3D::UpdateSelected( bool b )
{


}

void CRenderElevator3D::Setup3D( CProcessor2Base* pProc, int idx )
{
	ARCVector3 center = pProc->GetLocation();
	SetPosition(center);
	Update3D(pProc);
}
