#include "StdAfx.h"
#include ".\airside3d.h"
#include "Airspace3D.h"
#include "Airport3D.h"
#include "Topograph3D.h"

#include "..\InputAirside\InputAirside.h"
#include "..\InputAirside\ALTObject.h"
#include "AirsideGround.h"
#include "WayPoint3D.h"
#include "..\InputAirside\TrafficLight.h"
#include <CommonData/3DTextManager.h>
#include ".\Runway3D.h"
#include ".\Taxiway3D.h"
#include "..\InputAirside\Stand.h"
#include <cmath>
#include "Common/ARCpipe.h"
#include "OffsetMap.h"


CAirside3D::CAirside3D(void)
{
	m_pAirspace = NULL;
	m_pTopograph = NULL;

	m_pTempBaggageparkings = NULL;
	m_pTempPaxbusParkings = NULL;
	
}

CAirside3D::~CAirside3D(void)
{
	delete m_pAirspace;
	delete m_pTopograph;
	for(CAirport3DList::iterator aptIter = m_vAirports.begin(); aptIter!= m_vAirports.end();aptIter++)
	{
		delete *aptIter;
	}
}

BOOL CAirside3D::Init( int prjID )
{
	if(prjID < 0 ) return FALSE;
	//// read airspace database 
	//	
	m_nProjID = prjID;

	m_pAirspace  = new CAirspace3D( (prjID),this );
	
	// read airport database
	std::vector<int> airportIDList;
	InputAirside::GetAirportList(prjID,airportIDList);
	for(size_t i=0;i<airportIDList.size();++i){
		m_vAirports.push_back( new CAirport3D(airportIDList[i]) );		
	}	

	m_pTopograph = new CTopograph3D( prjID );

	UpdateBagCartParkingInfo();
	UpdatePaxBusParkingInfo();
	UpdateHoldShortLineInfo();
		
	return TRUE;
}

void CAirside3D::DrawOGL(C3DView * pView,bool bWithLevels)
{
	if(GetActiveLevel() && !GetActiveLevel()->IsVisible() )
	{
		return;
	}

	
	m_pTopograph->DrawOGL(pView);

	glEnable(GL_LIGHTING);
	for(size_t i=0;i<m_vAirports.size();i++){
		m_vAirports[i]->DrawOGL(pView, bWithLevels);
	}
	//	//
	RenderParkings(pView);

	m_pAirspace->DrawOGL(pView);
	//
	RenderObstructionSurface(pView);

   RenderHoldShortLine(pView);

}

void CAirside3D::DeleteObject( int objID )
{
	m_pAirspace->RemoveObject(objID);
	for(size_t i=0;i<m_vAirports.size();++i){
		m_vAirports[i]->RemoveObject(objID);
	}
}


void CAirside3D::DrawSelectOGL( C3DView * pView, SelectableSettings& selSetting)
{
	if(GetActiveLevel() && !GetActiveLevel()->IsVisible() )
	{
		return;
	}

	m_pAirspace->DrawSelectOGL(pView, selSetting);

	GetActiveAirport()->DrawSelectOGL(pView,selSetting);
	
	m_pTopograph->DrawSelectOGL(pView,selSetting);
}



ALTObject3D* CAirside3D::AddObject( ALTObject * pObj)
{
	if(pObj->IsAirportElement() ){
		int aptid = pObj->getAptID();
		CAirport3D * pAirport = GetAirport(aptid);
		if(pAirport)
		{
			return pAirport->AddObject(pObj);
		}
	
	}
	if(pObj->IsAirspaceElement())
	{
		return GetAirspace()->AddObject(pObj);	
	}
	if(pObj->IsTopographyElement())
	{
		return GetTopograph()->AddObject(pObj);
	}
	return NULL;
}


void CAirside3D::UpdateObject( int objID )
{
	ALTObject3D * pobj = GetObject3D(objID);
	if(pobj)
	{
		if(pobj->GetObjectType() == ALTOBJECT_TYPE(ALT_WAYPOINT))
		{
			CWayPoint3D* pWayPoint3D = (CWayPoint3D*)pobj;
			pWayPoint3D->Update(this);
		}		
		else
			pobj->Update();
	}
}

ALTObject3D * CAirside3D::GetObject3D( int id )
{
	ALTObject3D * pObj = NULL;
	if( pObj = m_pAirspace->GetObject3D(id) ) return pObj;
	for(CAirport3DList::iterator itr = m_vAirports.begin(); itr!= m_vAirports.end(); itr++) {
		if ( pObj = (*itr)->GetObject3D(id) ) return pObj;
	}
	if(pObj = m_pTopograph->GetObject3D(id) )return pObj;
	return NULL;
}

CAirspace3D * CAirside3D::GetAirspace()
{
	return m_pAirspace;
}

CAirport3D * CAirside3D::GetAirport( int nAirportID )
{
	for(CAirport3DList::iterator itr = m_vAirports.begin();itr!=m_vAirports.end();itr++){
		if( (*itr)->GetID() == nAirportID )
			return *itr;
	}
	return NULL;
}

void CAirside3D::UpdateAddorRemoveObjects()
{
	m_pAirspace->UpdateAddorRemoveObjects();
	for(CAirport3DList::iterator itr = m_vAirports.begin();itr!=m_vAirports.end();itr++){
		(*itr)->UpdateAddorRemoveObjects();
	}
	m_pTopograph->UpdateAddorRemoveObjects();
}

void CAirside3D::UpdateAddorRemoveAirports()
{
	ASSERT(m_nProjID >0 );
	std::vector<int> airportIDList;
	InputAirside::GetAirportList(m_nProjID,airportIDList);

	CAirport3DList newAirportList;

	for(int i =0;i<(int)airportIDList.size();i++){
		CAirport3D * pAirport = GetAirport(airportIDList[i]);
		if( !pAirport )
		{
			newAirportList.push_back(new CAirport3D(airportIDList[i]) );		
		}else {
			newAirportList.push_back(pAirport);
		}
	}
	m_vAirports = newAirportList;


}

ARCBoundingSphere CAirside3D::GetBoundingSphere() const
{
	ARCBoundingSphere bs;
	for(int i = 0 ;i< (int)m_vAirports.size(); ++i){
		CAirport3D * airport  = (CAirport3D*) m_vAirports.at(i);
		for(int j=0;j<airport->GetLevelList().GetCount();j++)
		{			
			double rad = airport->GetLevelList().GetLevel(j).GetBoundRadius();
			ARCVector3 vlocation(0,0,airport->GetLevelList().GetLevel(j).Altitude());
			bs += ARCBoundingSphere(vlocation,rad);
		}		
	}
	return bs;
}



void CAirside3D::GetObject3DList( ALTObject3DList& objList ) const
{
	m_pAirspace->GetObject3DList(objList);
	for(int i = 0 ;i<(int)m_vAirports.size();++i){
		m_vAirports.at(i)->GetObject3DList(objList);
	}
	m_pTopograph->GetObject3DList(objList);
}

void CAirside3D::UpdateAirportLevel( int nlevelid )
{
	for(int i=0;i<(int)m_vAirports.size();i++)
	{
		CAirport3D * pAirport = m_vAirports.at(i);
		for(int j=0;j< pAirport->GetLevelList().GetCount() ;j++)
		{
			if( pAirport->GetLevelList().GetLevel(j).getID() == nlevelid )
			{
				pAirport->GetLevelList().GetLevel(j).ReadData(nlevelid);
				pAirport->GetLevelList().GetLevel(j).ReadGridInfo(nlevelid);	
				pAirport->GetLevelList().GetLevel(j).ReadCADInfo(nlevelid);
				
				return;
			}
		}
	}
}

std::vector<CFloor2* > CAirside3D::GetAllAirportLevels()
{

	std::vector<CFloor2* > vReslt;
	for(int i=0;i<(int)m_vAirports.size();i++)
	{
		CAirport3D * pAirport = m_vAirports.at(i);
		for(int j=0;j< pAirport->GetLevelList().GetCount() ;j++)
		{
			vReslt.push_back(&pAirport->GetLevelList().GetLevel(j));
		}
	}
	return vReslt;
}

CAirsideGround* CAirside3D::GetAirportLevel( int nLevelid )
{
	for(int i=0;i<(int)m_vAirports.size();i++)
	{
		CAirport3D * pAirport = m_vAirports.at(i);
		for(int j=0;j< pAirport->GetLevelList().GetCount() ;j++)
		{
			if( pAirport->GetLevelList().GetLevel(j).getID() == nLevelid )
			{
				return &pAirport->GetLevelList().GetLevel(j);
			}
		}
	}
	return NULL;
}

CTopograph3D * CAirside3D::GetTopograph()
{
	return m_pTopograph;
}

CAirsideGround* CAirside3D::GetActiveLevel()
{
	for(int i =0 ;i<(int) m_vAirports.size() ;i ++)
	{
		CAirport3D * pAirport = m_vAirports.at(i);
		for(int j=0;j < pAirport->GetLevelList().GetCount();j++)
		{
			return &pAirport->GetLevelList().GetLevel(j);
		}
	}
	return NULL;
}

CAirport3D * CAirside3D::GetActiveAirport()
{
	if(m_vAirports.size())
		return m_vAirports.at(0);
	return NULL;
}

bool CAirside3D::UpdateChangeOfObject( ALTObject* pObj )
{
	int nObjID = pObj->getID();
	ALTObject3D  *pObj3D = NULL;
	if(pObj->IsAirportElement())
	{
		pObj3D = GetActiveAirport()->GetObject3D(nObjID);
	}
	else if( pObj->IsTopographyElement() )
	{
		return GetTopograph()->UpdateChangeOfObject(pObj);
	}	
	else {
		pObj3D = GetAirspace()->GetObject3D(nObjID);
	}
	if(pObj3D && pObj3D->GetObject() && pObj3D->GetObject()!=pObj)
	{		
		pObj3D->GetObject()->CopyData(pObj);		
		ALTObject3DList vObj3Ds;
		vObj3Ds.push_back(pObj3D);			
		ReflectChangeOf(vObj3Ds);
	}


	return true;
}

bool CAirside3D::ReflectChangeOf( ALTObject3DList vObj3Ds )
{
	
	ALTObject3DList vAirportObjs;
	ALTObject3DList vAirspaceObjs;

	for(int i=0;i< (int)vObj3Ds.size();i++)
	{
		ALTObject3D * pObj3D = vObj3Ds.at(i).get();
		if( pObj3D &&  pObj3D->GetObject()  )
		{
			if(pObj3D->GetObject()->IsAirportElement())
			{
				vAirportObjs.push_back( pObj3D );
			}
			if(pObj3D->GetObject()->IsAirspaceElement())
			{
				vAirspaceObjs.push_back( pObj3D );
			}
		}
	}

	if( vAirportObjs.size() )
	{	
		GetActiveAirport()->ReflectChangeOf(vAirportObjs);		
	}
	if( vAirspaceObjs.size() )
	{
		
	}
	return true;
}

void CAirside3D::RenderParkings( C3DView * pView )
{
	
	if( m_pTempBaggageparkings )
	{
		for(int i=0 ;i< (int)m_pTempBaggageparkings->GetElementCount();i++ )
		{
			BaggageCartParkingItem* pParkItem = m_pTempBaggageparkings->GetItem(i);
			Path path = pParkItem->GetParkArea();

			glDisable(GL_LIGHTING);
			glColor4ub(255, 100, 0 ,255);
			glBegin(GL_LINE_LOOP);
			for(int j=0;j<path.getCount();j++)
			{
				Point pt = path.getPoint(j);
				glVertex3d(pt.getX(),pt.getY(),pt.getZ());
			}
			glEnd();
		}
	}
	else
	{
		for(int i=0 ;i< (int)m_Baggageparkings.GetElementCount();i++ )
		{
			BaggageCartParkingItem* pParkItem = m_Baggageparkings.GetItem(i);
			Path path = pParkItem->GetParkArea();

			glDisable(GL_LIGHTING);
			glColor4ub(255, 100, 0 ,255);
			glBegin(GL_LINE_LOOP);
			for(int j=0;j<path.getCount();j++)
			{
				Point pt = path.getPoint(j);
				glVertex3d(pt.getX(),pt.getY(),pt.getZ());
			}
			glEnd();
		}

	}

	if( m_pTempPaxbusParkings )
	{
		for(int i=0 ;i< (int)m_pTempPaxbusParkings->GetElementCount();i++ )
		{
			CPaxBusParking * pParkItem = m_pTempPaxbusParkings->GetItem(i);
			Path path = pParkItem->GetPath();

			glDisable(GL_LIGHTING);
			glColor4ub(230, 210, 25,255);
			glBegin(GL_LINE_LOOP);
			for(int j=0;j<path.getCount();j++)
			{
				Point pt = path.getPoint(j);
				glVertex3d(pt.getX(),pt.getY(),pt.getZ());
			}
			glEnd();
		}
	}
	else
	{
		for(int i=0 ;i< (int)m_paxbusParkings.GetElementCount();i++ )
		{
			CPaxBusParking * pParkItem = m_paxbusParkings.GetItem(i);
			Path path = pParkItem->GetPath();

			glDisable(GL_LIGHTING);
			glColor4ub(230, 210, 25,255);
			glBegin(GL_LINE_LOOP);
			for(int j=0;j<path.getCount();j++)
			{
				Point pt = path.getPoint(j);
				glVertex3d(pt.getX(),pt.getY(),pt.getZ());
			}
			glEnd();
		}
	}
	
}

void CAirside3D::UpdateBagCartParkingInfo()
{
	m_Baggageparkings.CleanData();
	m_Baggageparkings.ReadData(m_nProjID);
	m_pTempBaggageparkings = NULL;
}

void CAirside3D::UpdatePaxBusParkingInfo()
{
	m_paxbusParkings.CleanData();
	m_paxbusParkings.ReadData(m_nProjID);
	m_pTempPaxbusParkings = NULL;
}

void CAirside3D::SetTempBagCartParkings( BaggageCartParkingSpecData* bagCartSpecData )
{
	m_pTempBaggageparkings = bagCartSpecData;
}

void CAirside3D::SetTempPaxBusParkings( CPaxBusParkingList* paxBusParks )
{
	m_pTempPaxbusParkings = paxBusParks;
}

void CAirside3D::UpdateHoldShortLineInfo()
{
	pHoldShortLine3DList.clear();
	TaxiInterruptLineList m_holdshortlines;
	m_holdshortlines.ReadData(m_nProjID);
	CTaxiInterruptLine* pItem = 0;
	for (size_t elemtCount = 0; elemtCount<m_holdshortlines.GetElementCount();elemtCount++)
	{

		pItem = m_holdshortlines.GetItem(elemtCount);
		int TaxiwayID = pItem->GetTaxiwayID();
		CPoint2008 pt =  pItem->GetPosition();
		if (pItem)
		{
			CTaxiInterruptLine3D* pHoldshortLine3D = new CTaxiInterruptLine3D(pItem->GetFrontIntersectionNodeID(),pItem->GetDistToFrontIntersectionNode(),pItem->GetBackIntersectionNodeID(),elemtCount,pItem->GetName(),TaxiwayID,pt);
			pHoldShortLine3DList.push_back(pHoldshortLine3D);		
		}
	}
}

void CAirside3D::RenderHoldShortLine(C3DView* pView)
{
	for (size_t i=0; i<pHoldShortLine3DList.size();i++)
	{
		pHoldShortLine3DList[i]->Draw(pView);
	}
}

void CAirside3D::ReflectRemoveOf( ALTObject3DList vObj3Ds )
{
	ALTObject3DList vAirportObjs;
	ALTObject3DList vAirspaceObjs;

	for(int i=0;i< (int)vObj3Ds.size();i++)
	{
		ALTObject3D * pObj3D = vObj3Ds.at(i).get();
		if( pObj3D &&  pObj3D->GetObject()  )
		{
			if(pObj3D->GetObject()->IsAirportElement())
			{
				vAirportObjs.push_back( pObj3D );
			}
			if(pObj3D->GetObject()->IsAirspaceElement())
			{
				vAirspaceObjs.push_back( pObj3D );
			}
		}
	}

	if( vAirportObjs.size() )
	{	
		GetActiveAirport()->ReflectRemoveOf(vAirportObjs);		
	}
	if( vAirspaceObjs.size() )
	{

	}	
}



void CAirside3D::RenderALTObjectText(C3DView * pView)
{
	if(GetActiveLevel() && !GetActiveLevel()->IsVisible() )
	{
		return;
	}
	ALTObject3DList objList;
	GetObject3DList(objList);
	
	for(size_t i = 0; i < objList.size();i++){
		const ALTObject3D* pObj3D = objList.at(i).get();

		int objID = pObj3D->GetID();
		const CString objStr = pObj3D->GetDisplayName();
		CString namestr = objStr;
#ifdef _DEBUG		
		namestr.Format("%s, %d",_T(objStr),objID);
#endif


		if(pObj3D->GetDisplayProp()->m_dpName.bOn){
			glColor4ubv(pObj3D->GetDisplayProp()->m_dpName.cColor);
			if (pObj3D->GetObject()->GetType()==ALT_TRAFFICLIGHT)
			{
				Path path = ((TrafficLight *)pObj3D->GetObject())->GetPath();
				for (int j=0; j<path.getCount(); j++)
				{
					char str = static_cast<char>(j+65);
					CString tempStr = namestr + ',' +str;
					TEXTMANAGER3D->DrawBitmapText(tempStr, pObj3D->GetSubLocation(j));
				}
			}
			else
			{
				TEXTMANAGER3D->DrawBitmapText(namestr, pObj3D->GetLocation() );
				if (pObj3D->GetObject()->GetType() == ALT_STAND)
				{
					Stand* pStand = (Stand*)pObj3D->GetObject();
					const StandLeadInLineList& leadInLineList = pStand->GetLeadInLineList();
					for (size_t i = 0; i < leadInLineList.GetItemCount(); i++)
					{
						const StandLeadInLine& leadInLine = leadInLineList.ItemAt(i);
						CString strLeadInLineName = _T("");
						strLeadInLineName.Format(_T("LeadIn%s"),leadInLine.GetName());
						const CPath2008& path = leadInLine.GetPath();
						if (path.getCount() > 0)
						{
							DistanceUnit dCenterAlt = path.GetTotalLength()/2;
							const CPoint2008& pt = path.GetDistPoint(dCenterAlt);
							TEXTMANAGER3D->DrawBitmapText(strLeadInLineName,pt);
						}
					}

					const StandLeadOutLineList& leadOutLineList = pStand->GetLeadOutLineList();
					for (size_t j = 0; j < leadOutLineList.GetItemCount(); j++)
					{
						const StandLeadOutLine& leadOutLine = leadOutLineList.ItemAt(j);
						CString strLeadOutLineName = _T("");
						strLeadOutLineName.Format(_T("LeadOut%s"),leadOutLine.GetName());
						const CPath2008& path = leadOutLine.GetPath();
						if (path.getCount() > 0)
						{
							DistanceUnit dCenterAlt = path.GetTotalLength()/2;
							const CPoint2008& pt = path.GetDistPoint(dCenterAlt);
							TEXTMANAGER3D->DrawBitmapText(strLeadOutLineName,pt);
						}
					}
				}
			}

		}

	}
}



void RenderLineStrip(const std::vector<ARCVector3>& vPts)
{
	glBegin(GL_LINE_STRIP);
		for(int i=0;i<(int)vPts.size(); ++i)
			glVertex3dv(vPts[i].n);
	glEnd();
}

void RenderRunwayObstructionSurface(Runway* pRunway, RUNWAY_MARK rwMark)
{
	if(rwMark == RUNWAYMARK_FIRST )
	{
		if( !pRunway->GetMark1Show() )
			return;
	}
	else
	{
		if(!pRunway->GetMark2Show())
			return;
	}


	ObjRuwayStruct& objStruct = pRunway->GetObstruction(rwMark);
	
	//build threshold height map
	OffsetHightMap thresholdHeightmap;	
	thresholdHeightmap.BuildHeightMap(objStruct.ThresHold);
	
	
	//build lateral
	OffsetHightMap laternalHeightMap;	
	laternalHeightMap.BuildHeightMap(objStruct.Lateral);

	//build end
	OffsetHightMap endHeightMap;	
	endHeightMap.BuildHeightMap(objStruct.EndOfRunway);	

	//draw 
	std::vector<ARCVector3> vLeftThresPts; vLeftThresPts.reserve(10);
	std::vector<ARCVector3> vRightThrestPts; vRightThrestPts.reserve(10);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glColor4f(0,1,0, 0.34f);
	glDepthMask(GL_FALSE);

	//threshold surface
	ARCVector3 vLenDir = pRunway->GetDir(rwMark); vLenDir.Normalize();
	ARCVector3 vWidthDir = ARCVector3(-vLenDir[VY], vLenDir[VX], 0); vWidthDir.Normalize();
	CPoint2008 vPos = (rwMark==RUNWAYMARK_FIRST)?pRunway->GetPath().getPoint(0):pRunway->GetPath().getPoint(1);

	glBegin(GL_QUAD_STRIP);
	for(int i=0;i<thresholdHeightmap.GetCount();++i)
	{
		ARCVector2& offsetHeight1 = thresholdHeightmap.Get(i);
		
		{//add the segment begin point
			DistanceUnit vLenOffset = offsetHeight1.x;
			DistanceUnit vWidthOffset;
			if( !laternalHeightMap.getHightOffset(offsetHeight1.y, vWidthOffset) ){ vWidthOffset = laternalHeightMap.getMaxOffset(); }

			vWidthOffset += pRunway->GetWidth()*0.5;

			ARCVector3 vPtLeft = -vLenDir * vLenOffset + vWidthDir * vWidthOffset + ARCVector3(vPos);
			vPtLeft[VZ] = offsetHeight1.y;

			ARCVector3 vPtRight = -vLenDir * vLenOffset - vWidthDir * vWidthOffset + ARCVector3(vPos);
			vPtRight[VZ] = offsetHeight1.y;

			glVertex3d(vPtLeft[VX],vPtLeft[VY],vPtLeft[VZ]); vLeftThresPts.push_back(vPtLeft);
			glVertex3d(vPtRight[VX],vPtRight[VY],vPtRight[VZ]); vRightThrestPts.push_back(vPtRight);

		}
	
		
		if(i == thresholdHeightmap.GetCount()-1)
			break;

		ARCVector2& offsetHeight2 = thresholdHeightmap.Get(i+1);
		std::vector<int> midHightList = laternalHeightMap.getHeightRangeOffsetIndexs(offsetHeight1.y, offsetHeight2.y);
		for(int j=0;j<(int)midHightList.size();++j)
		{		
			int midIndex = midHightList[j];
			DistanceUnit midHeight = laternalHeightMap.Get(midIndex).y;
			DistanceUnit vLenOffset;
			if( !thresholdHeightmap.getHightOffset(midHeight, vLenOffset) ){ vLenOffset = thresholdHeightmap.getMaxOffset(); }	
			DistanceUnit vWidthOffset = laternalHeightMap.Get(midIndex).x;
			vWidthOffset+= pRunway->GetWidth()*0.5;

			ARCVector3 vPtLeft = -vLenDir * vLenOffset + vWidthDir * vWidthOffset + ARCVector3(vPos);
			vPtLeft[VZ] = midHeight;

			ARCVector3 vPtRight = -vLenDir * vLenOffset - vWidthDir * vWidthOffset + ARCVector3(vPos);
			vPtRight[VZ] = midHeight;

			glVertex3d(vPtLeft[VX],vPtLeft[VY],vPtLeft[VZ]);  vLeftThresPts.push_back(vPtLeft);
			glVertex3d(vPtRight[VX],vPtRight[VY],vPtRight[VZ]); vRightThrestPts.push_back(vPtRight);
			
		}
		
		
	}
	glEnd();

	std::vector<ARCVector3> vLeftEndPts; vLeftEndPts.reserve(10);
	std::vector<ARCVector3> vRightEndPts; vRightEndPts.reserve(10);

	//end surface
	vLenDir = -vLenDir;
	vWidthDir = -vWidthDir;
	vPos = (rwMark==RUNWAYMARK_FIRST)?pRunway->GetPath().getPoint(1):pRunway->GetPath().getPoint(0);
	glBegin(GL_QUAD_STRIP);
	for(int i=0;i<endHeightMap.GetCount();++i)
	{
		ARCVector2& offsetHeight1 = endHeightMap.Get(i);

		{//add the segment begin point
			DistanceUnit vLenOffset = offsetHeight1.x;
			DistanceUnit vWidthOffset;
			if( !laternalHeightMap.getHightOffset(offsetHeight1.y, vWidthOffset) ){ vWidthOffset = laternalHeightMap.getMaxOffset(); }

			vWidthOffset += pRunway->GetWidth()*0.5;

			ARCVector3 vPtLeft = -vLenDir * vLenOffset + vWidthDir * vWidthOffset + ARCVector3(vPos);
			vPtLeft[VZ] = offsetHeight1.y;

			ARCVector3 vPtRight = -vLenDir * vLenOffset - vWidthDir * vWidthOffset + ARCVector3(vPos);
			vPtRight[VZ] = offsetHeight1.y;

			glVertex3d(vPtLeft[VX],vPtLeft[VY],vPtLeft[VZ]);     vLeftEndPts.push_back(vPtLeft);
			glVertex3d(vPtRight[VX],vPtRight[VY],vPtRight[VZ]);  vRightEndPts.push_back(vPtRight);

		}


		if(i == endHeightMap.GetCount()-1)
			break;

		ARCVector2& offsetHeight2 = endHeightMap.Get(i+1);
		std::vector<int> midHightList = laternalHeightMap.getHeightRangeOffsetIndexs(offsetHeight1.y, offsetHeight2.y);
		for(int j=0;j<(int)midHightList.size();++j)
		{		
			int midIndex = midHightList[j];
			DistanceUnit midHeight = laternalHeightMap.Get(midIndex).y;
			DistanceUnit vLenOffset;
			if( !endHeightMap.getHightOffset(midHeight, vLenOffset) ){ vLenOffset = endHeightMap.getMaxOffset(); }	
			DistanceUnit vWidthOffset = laternalHeightMap.Get(midIndex).x;
			vWidthOffset+= pRunway->GetWidth()*0.5;

			ARCVector3 vPtLeft = -vLenDir * vLenOffset + vWidthDir * vWidthOffset + ARCVector3(vPos);
			vPtLeft[VZ] = midHeight;

			ARCVector3 vPtRight = -vLenDir * vLenOffset - vWidthDir * vWidthOffset + ARCVector3(vPos);
			vPtRight[VZ] = midHeight;

			glVertex3d(vPtLeft[VX],vPtLeft[VY],vPtLeft[VZ]);  vLeftEndPts.push_back(vPtLeft);
			glVertex3d(vPtRight[VX],vPtRight[VY],vPtRight[VZ]); vRightEndPts.push_back(vPtRight);

		}


	}
	glEnd();


	/*int nLeftCount = min( vRightEndPts.size(), vLeftThresPts.size() );
	glBegin(GL_TRIANGLE_STRIP);
	for(int i=0;i< nLeftCount;i++)
	{
		glVertex3dv(vLeftThresPts[i].n);
		glVertex3dv(vRightEndPts[i].n);
	}
	glEnd();

	int nRightCount = min( vLeftEndPts.size(), vRightThrestPts.size() );
	glBegin(GL_TRIANGLE_STRIP);
	for(int i=0;i< nRightCount;i++)
	{
		glVertex3dv(vRightThrestPts[i].n);
		glVertex3dv(vLeftEndPts[i].n);
	}
	glEnd();*/



	//left surface
	DistanceUnit dHalfRunwayLen = pRunway->GetPath().GetTotalLength() *0.5;
	DistanceUnit dhalfRunwayWidth = pRunway->GetWidth() * 0.5;

	std::swap(vLenDir,vWidthDir);
	vPos = (pRunway->GetPath().getPoint(1) + pRunway->GetPath().getPoint(0) )*0.5;	
	for(int i=0;i<laternalHeightMap.GetCount()-1;++i)
	{

		std::vector<ARCVector3> vLeftPts;
		std::vector<ARCVector3> vRightPts;

		ARCVector2& offsetHeight1 = laternalHeightMap.Get(i);
		{//add the segment begin point
			DistanceUnit vLenOffset = offsetHeight1.x;
			
			DistanceUnit vWidthOffset1;
			if( !endHeightMap.getHightOffset(offsetHeight1.y, vWidthOffset1) ){ vWidthOffset1 = endHeightMap.getMaxOffset(); }
			DistanceUnit vWidthOffset2;
			if( !thresholdHeightmap.getHightOffset(offsetHeight1.y, vWidthOffset2) ){ vWidthOffset2 = thresholdHeightmap.getMaxOffset(); }

			vWidthOffset1 += dHalfRunwayLen;
			vWidthOffset2 += dHalfRunwayLen;

			ARCVector3 vPtLeft = -vLenDir * (vLenOffset + dhalfRunwayWidth) + vWidthDir * vWidthOffset2 + ARCVector3(vPos);
			vPtLeft[VZ] = offsetHeight1.y;

			ARCVector3 vPtRight = -vLenDir * (vLenOffset + dhalfRunwayWidth)- vWidthDir * vWidthOffset1 + ARCVector3(vPos);
			vPtRight[VZ] = offsetHeight1.y;

			//glVertex3d(vPtLeft[VX],vPtLeft[VY],vPtLeft[VZ]);     
			vLeftPts.push_back(vPtLeft);
			//glVertex3d(vPtRight[VX],vPtRight[VY],vPtRight[VZ]);
			vRightPts.push_back(vPtRight);

		}		

		ARCVector2& offsetHeight2 = laternalHeightMap.Get(i+1);
		
		{
			std::vector<int> midHightList = endHeightMap.getHeightRangeOffsetIndexs(offsetHeight1.y, offsetHeight2.y);
			for(int j=0;j<(int)midHightList.size();++j)
			{		
				int midIndex = midHightList[j];
				DistanceUnit midHeight = endHeightMap.Get(midIndex).y;
				DistanceUnit vWithOffset = endHeightMap.Get(midIndex).x;
				DistanceUnit vLenOffset;
				if( !laternalHeightMap.getHightOffset(midHeight,vLenOffset) ){ vLenOffset = laternalHeightMap.getMaxOffset(); }

				vWithOffset += dHalfRunwayLen;

				ARCVector3 vPtRight = -vLenDir * (vLenOffset + dhalfRunwayWidth) - vWidthDir * vWithOffset + ARCVector3(vPos);
				vPtRight[VZ] = midHeight;

				//glVertex3d(vPtLeft[VX],vPtLeft[VY],vPtLeft[VZ]);     
				//vLeftPts.push_back(vPtLeft);
				//glVertex3d(vPtRight[VX],vPtRight[VY],vPtRight[VZ]);
				vRightPts.push_back(vPtRight);


			}	
		}		
		{
			std::vector<int> midHightList = thresholdHeightmap.getHeightRangeOffsetIndexs(offsetHeight1.y, offsetHeight2.y);
			for(int j=0;j<(int)midHightList.size();++j)
			{		
				int midIndex = midHightList[j];
				DistanceUnit midHeight = thresholdHeightmap.Get(midIndex).y;
				
				DistanceUnit vWithOffset = thresholdHeightmap.Get(midIndex).x;
				DistanceUnit vLenOffset;
				if( !laternalHeightMap.getHightOffset(midHeight,vLenOffset) ){ vLenOffset = laternalHeightMap.getMaxOffset(); }

				vWithOffset += dHalfRunwayLen;

				ARCVector3 vPtRight = -vLenDir * (vLenOffset + dhalfRunwayWidth) + vWidthDir * vWithOffset + ARCVector3(vPos);
				vPtRight[VZ] = midHeight;

				//glVertex3d(vPtLeft[VX],vPtLeft[VY],vPtLeft[VZ]);     
				vLeftPts.push_back(vPtRight);
				//glVertex3d(vPtRight[VX],vPtRight[VY],vPtRight[VZ]);
				//vRightPts.push_back(vPtRight);
			}	
		}

		{//add the segment begin point
			DistanceUnit vLenOffset = offsetHeight2.x;

			DistanceUnit vWidthOffset1;
			if( !endHeightMap.getHightOffset(offsetHeight2.y, vWidthOffset1) ){ vWidthOffset1 = endHeightMap.getMaxOffset(); }
			DistanceUnit vWidthOffset2;
			if( !thresholdHeightmap.getHightOffset(offsetHeight2.y, vWidthOffset2) ){ vWidthOffset2 = thresholdHeightmap.getMaxOffset(); }

			vWidthOffset1 += dHalfRunwayLen;
			vWidthOffset2 += dHalfRunwayLen;

			ARCVector3 vPtLeft = -vLenDir * (vLenOffset + dhalfRunwayWidth) + vWidthDir * vWidthOffset2 + ARCVector3(vPos);
			vPtLeft[VZ] = offsetHeight2.y;

			ARCVector3 vPtRight = -vLenDir * (vLenOffset + dhalfRunwayWidth)- vWidthDir * vWidthOffset1 + ARCVector3(vPos);
			vPtRight[VZ] = offsetHeight2.y;

			//glVertex3d(vPtLeft[VX],vPtLeft[VY],vPtLeft[VZ]);     
			vLeftPts.push_back(vPtLeft);
			//glVertex3d(vPtRight[VX],vPtRight[VY],vPtRight[VZ]);
			vRightPts.push_back(vPtRight);
		}		
		glBegin(GL_POLYGON);
			for(int i=0;i<(int)vLeftPts.size();++i)
			{
				glVertex3dv(vLeftPts[i].n);
			}
			for(int i=(int)vRightPts.size()-1;i>=0; --i)
			{
				glVertex3dv(vRightPts[i].n);
			}
		glEnd();


	}
	
	//right side
	vLenDir = -vLenDir;
	for(int i=0;i<laternalHeightMap.GetCount()-1;++i)
	{

		std::vector<ARCVector3> vLeftPts;
		std::vector<ARCVector3> vRightPts;

		ARCVector2& offsetHeight1 = laternalHeightMap.Get(i);
		{//add the segment begin point
			DistanceUnit vLenOffset = offsetHeight1.x;

			DistanceUnit vWidthOffset1;
			if( !endHeightMap.getHightOffset(offsetHeight1.y, vWidthOffset1) ){ vWidthOffset1 = endHeightMap.getMaxOffset(); }
			DistanceUnit vWidthOffset2;
			if( !thresholdHeightmap.getHightOffset(offsetHeight1.y, vWidthOffset2) ){ vWidthOffset2 = thresholdHeightmap.getMaxOffset(); }

			vWidthOffset1 += dHalfRunwayLen;
			vWidthOffset2 += dHalfRunwayLen;

			ARCVector3 vPtLeft = -vLenDir * (vLenOffset + dhalfRunwayWidth) + vWidthDir * vWidthOffset2 + ARCVector3(vPos);
			vPtLeft[VZ] = offsetHeight1.y;

			ARCVector3 vPtRight = -vLenDir * (vLenOffset + dhalfRunwayWidth)- vWidthDir * vWidthOffset1 + ARCVector3(vPos);
			vPtRight[VZ] = offsetHeight1.y;

			//glVertex3d(vPtLeft[VX],vPtLeft[VY],vPtLeft[VZ]);     
			vLeftPts.push_back(vPtLeft);
			//glVertex3d(vPtRight[VX],vPtRight[VY],vPtRight[VZ]);
			vRightPts.push_back(vPtRight);

		}		

		ARCVector2& offsetHeight2 = laternalHeightMap.Get(i+1);

		{
			std::vector<int> midHightList = endHeightMap.getHeightRangeOffsetIndexs(offsetHeight1.y, offsetHeight2.y);
			for(int j=0;j<(int)midHightList.size();++j)
			{		
				int midIndex = midHightList[j];
				DistanceUnit midHeight = endHeightMap.Get(midIndex).y;
				DistanceUnit vWithOffset = endHeightMap.Get(midIndex).x;
				DistanceUnit vLenOffset;
				if( !laternalHeightMap.getHightOffset(midHeight,vLenOffset) ){ vLenOffset = laternalHeightMap.getMaxOffset(); }

				vWithOffset += dHalfRunwayLen;

				ARCVector3 vPtRight = -vLenDir * (vLenOffset + dhalfRunwayWidth) - vWidthDir * vWithOffset + ARCVector3(vPos);
				vPtRight[VZ] = midHeight;

				//glVertex3d(vPtLeft[VX],vPtLeft[VY],vPtLeft[VZ]);     
				//vLeftPts.push_back(vPtLeft);
				//glVertex3d(vPtRight[VX],vPtRight[VY],vPtRight[VZ]);
				vRightPts.push_back(vPtRight);


			}	
		}		
		{
			std::vector<int> midHightList = thresholdHeightmap.getHeightRangeOffsetIndexs(offsetHeight1.y, offsetHeight2.y);
			for(int j=0;j<(int)midHightList.size();++j)
			{		
				int midIndex = midHightList[j];
				DistanceUnit midHeight = thresholdHeightmap.Get(midIndex).y;

				DistanceUnit vWithOffset = thresholdHeightmap.Get(midIndex).x;
				DistanceUnit vLenOffset;
				if( !laternalHeightMap.getHightOffset(midHeight,vLenOffset) ){ vLenOffset = laternalHeightMap.getMaxOffset(); }

				vWithOffset += dHalfRunwayLen;

				ARCVector3 vPtRight = -vLenDir * (vLenOffset + dhalfRunwayWidth) + vWidthDir * vWithOffset + ARCVector3(vPos);
				vPtRight[VZ] = midHeight;

				//glVertex3d(vPtLeft[VX],vPtLeft[VY],vPtLeft[VZ]);     
				vLeftPts.push_back(vPtRight);
				//glVertex3d(vPtRight[VX],vPtRight[VY],vPtRight[VZ]);
				//vRightPts.push_back(vPtRight);
			}	
		}

		{//add the segment begin point
			DistanceUnit vLenOffset = offsetHeight2.x;

			DistanceUnit vWidthOffset1;
			if( !endHeightMap.getHightOffset(offsetHeight2.y, vWidthOffset1) ){ vWidthOffset1 = endHeightMap.getMaxOffset(); }
			DistanceUnit vWidthOffset2;
			if( !thresholdHeightmap.getHightOffset(offsetHeight2.y, vWidthOffset2) ){ vWidthOffset2 = thresholdHeightmap.getMaxOffset(); }

			vWidthOffset1 += dHalfRunwayLen;
			vWidthOffset2 += dHalfRunwayLen;

			ARCVector3 vPtLeft = -vLenDir * (vLenOffset + dhalfRunwayWidth) + vWidthDir * vWidthOffset2 + ARCVector3(vPos);
			vPtLeft[VZ] = offsetHeight2.y;

			ARCVector3 vPtRight = -vLenDir * (vLenOffset + dhalfRunwayWidth)- vWidthDir * vWidthOffset1 + ARCVector3(vPos);
			vPtRight[VZ] = offsetHeight2.y;

			//glVertex3d(vPtLeft[VX],vPtLeft[VY],vPtLeft[VZ]);     
			vLeftPts.push_back(vPtLeft);
			//glVertex3d(vPtRight[VX],vPtRight[VY],vPtRight[VZ]);
			vRightPts.push_back(vPtRight);
		}		
		glBegin(GL_POLYGON);
		for(int i=0;i<(int)vLeftPts.size();++i)
		{
			glVertex3dv(vLeftPts[i].n);
		}
		for(int i=(int)vRightPts.size()-1;i>=0; --i)
		{
			glVertex3dv(vRightPts[i].n);
		}
		glEnd();


	}



	glDepthMask(GL_TRUE);
	glColor3f(0.3f,1,0.3f);
	glDisable(GL_BLEND);

	//render lines
	RenderLineStrip(vLeftEndPts);
	RenderLineStrip(vRightEndPts);
	RenderLineStrip(vRightThrestPts);
	RenderLineStrip(vLeftThresPts);


}

void RenderTaxiwayObstructionSurface(Taxiway* pTaxiway)
{
	if(!pTaxiway->GetShow())
	{
		return;
	}
	
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glColor4f(0,1,0, 0.34f);
	glDepthMask(GL_FALSE);

	//build end
	OffsetHightMap laternalHeightMap;
	laternalHeightMap.BuildHeightMap(pTaxiway->GetObjSurface());	

	
	DistanceUnit dWidth = pTaxiway->GetWidth();
	int nPointCount = pTaxiway->GetPath().getCount();

	std::vector<ARCPath3> vLinesPointLeft;
	std::vector<ARCPath3> vLinesPointRight;
	vLinesPointLeft.resize( nPointCount );
	vLinesPointRight.resize( nPointCount );

	for(int i=0;i<laternalHeightMap.GetCount();++i)
	{
		ARCVector2& offsetHeight1 = laternalHeightMap.Get(i);		
		ARCPipe thisPipe(pTaxiway->GetPath(), dWidth + offsetHeight1.x * 2.0 );
		DistanceUnit thisHeight = offsetHeight1.y;
		
		for(int idxPt=0;idxPt < (int)thisPipe.m_sidePath1.size(); ++idxPt)
		{
			ARCVector3 vLeft(thisPipe.m_sidePath1[idxPt][VX], thisPipe.m_sidePath1[idxPt][VY], thisHeight);

			ARCVector3 vRight(thisPipe.m_sidePath2[idxPt][VX], thisPipe.m_sidePath2[idxPt][VY], thisHeight);

			vLinesPointLeft[idxPt].push_back(vLeft);
			vLinesPointRight[idxPt].push_back(vRight);
		}		
	}

	//Draw Quads
	for(int i=0;i<nPointCount-1;i++)
	{
		glBegin(GL_QUAD_STRIP);
		for(int j=0;j<laternalHeightMap.GetCount();++j)
		{
			glVertex3dv(vLinesPointLeft[i][j].n);
			glVertex3dv(vLinesPointLeft[i+1][j].n);

		}
		glEnd();
	}
	for(int i=0;i<nPointCount-1;i++)
	{
		glBegin(GL_QUAD_STRIP);
		for(int j=0;j<laternalHeightMap.GetCount();++j)
		{
			glVertex3dv(vLinesPointRight[i][j].n);
			glVertex3dv(vLinesPointRight[i+1][j].n);

		}
		glEnd();
	}

	glDepthMask(GL_TRUE);
	glColor3f(0.3f,1,0.3f);
	glDisable(GL_BLEND);

	for(int i =0;i<nPointCount;i++)
	{
		RenderLineStrip(vLinesPointLeft[i]);
		RenderLineStrip(vLinesPointRight[i]);
	}

}

void CAirside3D::RenderObstructionSurface(C3DView* pView)
{
//

	
	for(int i=0;i<(int)m_vAirports.size();i++ )
	{
		CAirport3D  *pAirport = m_vAirports.at(i);
		
		//render runways
		ALTObject3DList& runwaylist = pAirport->m_vRunways;
		for(int j =0;j < (int)runwaylist.size(); ++j)
		{
			CRunway3D *pRuwnay3D = (CRunway3D*)runwaylist.at(j).get();
			RenderRunwayObstructionSurface( pRuwnay3D->GetRunway(),RUNWAYMARK_FIRST );
			RenderRunwayObstructionSurface( pRuwnay3D->GetRunway(), RUNWAYMARK_SECOND );
		}
		
		//render taxiways
		ALTObject3DList& taxiwaylist = pAirport->m_vTaxways;
		for(int j=0;j< (int)taxiwaylist.size(); ++j)
		{
			CTaxiway3D* pTaxiway3D = (CTaxiway3D*)taxiwaylist.at(j).get();
			RenderTaxiwayObstructionSurface( pTaxiway3D->GetTaxiway() );
		}		
	}

}