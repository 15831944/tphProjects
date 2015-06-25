#include "StdAfx.h"
#include ".\taxiway3d.h"
#include "..\InputAirside\Taxiway.h"
#include "..\InputAirside\ALTObjectDisplayProp.h"
#include <Common/ShapeGenerator.h>
#include ".\3DView.h"
#include <CommonData/3DTextManager.h>
#include ".\..\InputAirside\Runway.h"
#include "ChildFrm.h"
#include "../Common/ARCPipe.h"
#include "FilletTaxiway3D.h"
#include "..\InputAirside\IntersectionNodesInAirport.h"
#include "../Common/PathSnapFunctor.h"
#include "../InputAirside/HoldShortLines.h"

#include ".\glrender\glTextureResource.h"

struct FilletDistInTaxiwayLess
{
	FilletDistInTaxiwayLess(int TaxiwayID){ m_nTaxiID = TaxiwayID; }

	int m_nTaxiID;

	bool operator()(CFilletPoint3D* pPt1, CFilletPoint3D* pPt2)const
	{
		ASSERT(pPt1 && pPt2);
		if(pPt1->GetIntersectionNode().GetDistance(m_nTaxiID) < pPt2->GetIntersectionNode().GetDistance(m_nTaxiID) )
		{
			return true;
		}
		if( pPt1->GetIntersectionNode().GetDistance(m_nTaxiID) > pPt2->GetIntersectionNode().GetDistance(m_nTaxiID) )
		{
			return false;
		}
		return pPt1->GetDistInObj() < pPt2->GetDistInObj();
	}
};



CTaxiway3D::CTaxiway3D(int id):ALTObject3D(id)
{
	m_pObj = new Taxiway;
	//m_pDisplayProp = new TaxiwayDisplayProp;
	m_bInEdit = false;
	m_TaxiwayID = id;

	//m_pTaxiwayMark = new TaxiwayMarking3D(GetTaxiway());
}


CTaxiway3D::~CTaxiway3D(void)
{
}

void CTaxiway3D::DrawOGL( C3DView * pView )
{

	TaxiwayDisplayProp * pObjDisplay = (TaxiwayDisplayProp*) GetDisplayProp();
	glEnable(GL_BLEND);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glDisable(GL_LIGHTING);
	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
	if(pObjDisplay->m_dpShape.bOn)
	{
		glColor3ubv(pObjDisplay->m_dpShape.cColor);
		RenderSurface(pView);
	}
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(-4.0,-4.0);
	glDepthMask(GL_FALSE);
	if(pObjDisplay->m_dpShape.bOn)
	{
		glColor3ubv(pObjDisplay->m_dpShape.cColor);
		RenderSurface(pView);
	}

	if(pObjDisplay->m_dpCenterLine.bOn){
		glColor3ubv(pObjDisplay->m_dpCenterLine.cColor);
		RenderMarkLine(pView);
	}
	if(pObjDisplay->m_dpMarking.bOn){
		glColor3ubv(pObjDisplay->m_dpMarking.cColor);
		RenderMarkText(pView);
	}	

	if(pObjDisplay->m_dpDir.bOn){
		glColor3ubv(pObjDisplay->m_dpDir.cColor);
	}
	
	RenderEditPoint(pView);	
	glDepthMask(GL_TRUE);
	glDisable(GL_POLYGON_OFFSET_FILL);
	
}

void CTaxiway3D::DrawSelect( C3DView * pView )
{
	
	if(IsNeedSync()){
		DoSync();
	}
	TaxiwayDisplayProp * pObjDisplay = (TaxiwayDisplayProp*) GetDisplayProp();//.get();

	
	RenderSurface(pView);

	RenderEditPoint(pView);
	
	for(int i=0;i<(int)m_vAllHoldPositions3D.size();i++){
		m_vAllHoldPositions3D.at(i)->DrawSelect(pView);
	}

	if(pObjDisplay->m_dpMarking.bOn){
		glColor3ubv(pObjDisplay->m_dpMarking.cColor);
		if(m_pTaxiwayMark.get())
		{
			m_pTaxiwayMark->DrawSelect(pView);
		}
	}	
	
}

void CTaxiway3D::RenderSurface( C3DView *pView )
{
	//if(!IsInEdit())
	glEnable(GL_BLEND);
	
	if(IsNeedSync()){
		DoSync();
	}

	if(GetTaxiway()->GetPath().getCount() < 2)
		return ;

	CPath2008 cullPath;

	if( IsOnMove() || IsInEdit())
	{
		cullPath = GetTaxiway()->GetPath();	
	}
	else
	{
		DistanceUnit fromDist = 0 ;
		DistanceUnit toDist = GetTaxiway()->GetPath().GetTotalLength(); 
		std::sort(m_vFilletPoints.begin(),m_vFilletPoints.end(),FilletDistInTaxiwayLess(GetTaxiway()->getID()) );

		if(m_vFilletPoints.size())
		{
			//begin
			CFilletPoint3D * pFilletBegin = *m_vFilletPoints.begin();
			if( pFilletBegin->GetIntersectionNode().GetDistance(GetTaxiway()->getID()) <  pFilletBegin->GetDistInObj() )
			{
				fromDist = pFilletBegin->GetDistInObj();
			}
			//end
			CFilletPoint3D * pFilletEnd = *m_vFilletPoints.rbegin();
			if(pFilletEnd->GetIntersectionNode().GetDistance(GetTaxiway()->getID()) > pFilletEnd->GetDistInObj() )
			{
				toDist = pFilletEnd->GetDistInObj();
			}
		}
		cullPath = GetSubPath(GetTaxiway()->GetPath(), fromDist, toDist);
	}

	Taxiway * pObj = (Taxiway*)m_pObj.get();	
	TaxiwayDisplayProp * pObjDisplay = (TaxiwayDisplayProp*) GetDisplayProp();//.get();	
	
	CTexture * pTexture = pView->getTextureResource()->getTexture("Taxiway");
	if(pTexture) pTexture->Apply();

	glDisable(GL_LIGHTING);

	if(IsInEdit())
		glColor4d(1.0,1.0,1.0,0.4);
	else
		glColor4d(1,1,1,1);

	glNormal3i(0,0,1);
	ARCPipe pipepath (cullPath,GetTaxiway()->GetWidth()*1.05);
	int npippathcnt  = (int)pipepath.m_centerPath.size();
	if(npippathcnt < 2) return;


	//draw to color buffer	
	double dLenside1 =0; double dLenside2 = 0;
	ARCVector3 prePoint1 = pipepath.m_sidePath1[0];
	ARCVector3 prePoint2 = pipepath.m_sidePath2[0];
	
	double texcord1 = 0;
	double texcord2 = 0;
	glBegin(GL_QUADS);	
	
	for(int i =1; i < npippathcnt;++i){	
		
		glTexCoord2d(0,texcord1);
		glVertex3dv(prePoint1);
		glTexCoord2d(1,texcord2);
		glVertex3dv(prePoint2);


		double dLen1 = prePoint1.DistanceTo(pipepath.m_sidePath1[i]);
		double dLen2 = prePoint2.DistanceTo(pipepath.m_sidePath2[i]);

		texcord1 += dLen1 * 0.0002;
		texcord2 += dLen2 * 0.0002;		
		
		glTexCoord2d(1,texcord2);
		glVertex3dv(pipepath.m_sidePath2[i]);	
		glTexCoord2d(0,texcord1);
		glVertex3dv(pipepath.m_sidePath1[i]);	

		prePoint1 = pipepath.m_sidePath1[i];
		prePoint2 = pipepath.m_sidePath2[i];

		std::swap(texcord1,texcord2);
	}
	glEnd();
	/*if(!IsInEdit())
		glEnable(GL_BLEND);*/

}

void CTaxiway3D::RenderMarkLine( C3DView *pView )
{	
	if (!glIsEnabled(GL_BLEND))
	{
		glEnable(GL_BLEND);
	}
	if(GetTaxiway()->GetPath().getCount() < 2)
		return; 	

	Taxiway * pObj = (Taxiway*)m_pObj.get();
	TaxiwayDisplayProp * pObjDisplay = (TaxiwayDisplayProp*) GetDisplayProp();//.get();
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	DistanceUnit slwidth = 50;	
	CPath2008 cullPath;
	if( IsOnMove() || IsInEdit())
	{
		cullPath = GetTaxiway()->GetPath();	
	}
	else
	{
		DistanceUnit fromDist = 0 ;
		DistanceUnit toDist = GetTaxiway()->GetPath().GetTotalLength(); 
		std::sort(m_vFilletPoints.begin(),m_vFilletPoints.end(),FilletDistInTaxiwayLess(GetTaxiway()->getID()) );

		if(m_vFilletPoints.size())
		{
			//begin
			CFilletPoint3D * pFilletBegin = *m_vFilletPoints.begin();
			if( pFilletBegin->GetIntersectionNode().GetDistance(GetTaxiway()->getID()) <  pFilletBegin->GetDistInObj() )
			{
				fromDist = pFilletBegin->GetDistInObj();
			}
			//end
			CFilletPoint3D * pFilletEnd = *m_vFilletPoints.rbegin();
			if(pFilletEnd->GetIntersectionNode().GetDistance(GetTaxiway()->getID()) > pFilletEnd->GetDistInObj() )
			{
				toDist = pFilletEnd->GetDistInObj();
			}
		}
		cullPath = GetSubPath(GetTaxiway()->GetPath(), fromDist, toDist);
	}

	ShapeGenerator::widthPath widthpath;
	ShapeGenerator::GenWidthPath(cullPath,slwidth,widthpath);	

	glNormal3f(0,0,1);
	glBegin(GL_QUAD_STRIP);
	for(int i=0;i<(int)widthpath.leftpts.size();i++){
		glVertex3dv(widthpath.leftpts[i]);
		glVertex3dv(widthpath.rightpts[i]);
	}	
	glEnd();

	RenderHoldPosition(pView);
}

void CTaxiway3D::RenderMarkText( C3DView *pView )
{	
	if(!m_pTaxiwayMark.get())
	{
		Taxiway * pObj = (Taxiway*)m_pObj.get();
		m_pTaxiwayMark = new TaxiwayMarking3D(pObj);
	}
	m_pTaxiwayMark->Draw(pView);	
}



ARCPoint3 CTaxiway3D::GetLocation( void ) const
{
	const CPath2008& path = GetTaxiway()->GetPath();
	
	int nMid = int( (path.getCount()-1) * 0.5);
	
	if(nMid<path.getCount()-1)
		return (path.getPoint(nMid) + path.getPoint(nMid+1) )*0.5;
	return ARCPoint3(0,0,0);
}

//TaxiwayDisplayProp * CTaxiway3D::GetDisplayProp()
//{
//	return (TaxiwayDisplayProp* )m_pDisplayProp.get();
//
//}

Taxiway * CTaxiway3D::GetTaxiway()const
{
	return (Taxiway*)m_pObj.get();
}

void CTaxiway3D::OnMove( DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /*= 0*/ )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	ALTObject3D::OnMove(dx,dy,dz);

	GetTaxiway()->m_path.DoOffset(dx,dy,0);
	
}

void CTaxiway3D::DoSync()
{
	m_vControlPoints.clear();
	int nPtCnt = GetTaxiway()->GetPath().getCount();

	for(int i=0;i<nPtCnt;++i)
	{
		m_vControlPoints.push_back( new CALTObjectPathControlPoint2008(this,GetTaxiway()->getPath(),i) );
	}
		
	ALTObject3D::DoSync();	
}



void CTaxiway3D::FlushChange()
{
	try
	{
		GetTaxiway()->UpdatePath();
		UpdateSync();
	}
	catch (CADOException& e)
	{
		CString strErr = e.ErrorMessage(); 
	}
}

void CTaxiway3D::SnapTo( const ALTObject3DList& RwOrTaxiList )
{
	CPath2008& taxiPath = GetTaxiway()->getPath();
	int nPtCnt = GetTaxiway()->getPath().getCount();
	if(nPtCnt < 2)
		return ;

	CPoint2008 headPoint = taxiPath.getPoint(0);
	CPoint2008 vHead = CPoint2008(headPoint - taxiPath.getPoint(1));
	CPoint2008 snapHead  = headPoint;
	DistanceUnit minDistHead = ARCMath::DISTANCE_INFINITE;
	DistanceUnit minDistTail = ARCMath::DISTANCE_INFINITE;
	bool bToSnapHead = false;
	bool bToSnapTail = false;

	CPoint2008 tailPoint = taxiPath.getPoint( nPtCnt -1);
	CPoint2008 vTail = CPoint2008(taxiPath.getPoint(nPtCnt-2) - tailPoint); 
	CPoint2008 snapTail = tailPoint;

	for(ALTObject3DList::const_iterator itr = RwOrTaxiList.begin();itr!=RwOrTaxiList.end();itr++){
		const ALTObject3D * pObj3D  = (*itr).get();
		if(pObj3D == this) continue;
		CPath2008 otherPath;
		DistanceUnit otherWidth;

		if( pObj3D->GetObjectType() == ALT_RUNWAY )
		{
			Runway* pRunway = (Runway*)(pObj3D->GetObject());	
			otherPath = pRunway->getPath();
			otherWidth = pRunway->GetWidth();
		}	
		else if( pObj3D->GetObjectType() == ALT_TAXIWAY )
		{
			Taxiway* pTaxiway = (Taxiway*)(pObj3D->GetObject());
			otherPath = pTaxiway->getPath();
			otherWidth = pTaxiway->GetWidth();
		}
		else {
			continue;
		}

		PathSnapFunctor pathSnapFuc(otherPath, otherWidth*0.5);
		{ //head point
			CPoint2008 snapPt;
			if( pathSnapFuc.GetSnapPoint( headPoint, vHead, snapPt) )
			{
				DistanceUnit snapDist = pathSnapFuc.mindist;
				if( snapDist < minDistHead )
				{
					minDistHead = snapDist;
					snapHead = snapPt;
					bToSnapHead = true;
				}
			}
		}
		{ // tail point 
			CPoint2008 snapPt;
			if( pathSnapFuc.GetSnapPoint( tailPoint, vTail, snapPt))
			{
				DistanceUnit snapDist = pathSnapFuc.mindist;
				if(snapDist < minDistTail )
				{
					minDistTail  = snapDist;
					snapTail = snapPt;
					bToSnapTail = true;
				}
			}
		}
	}	
	
	bool bSnaped = false;
	if( bToSnapHead )
	{
		if(taxiPath[0].distance3D(snapHead) > ARCMath::EPSILON )
		{
			taxiPath[0] = snapHead;
			bSnaped = true;
		}
	}
	if( bToSnapTail )
	{
		if( taxiPath[nPtCnt-1].distance3D(snapTail) > ARCMath::EPSILON) 
		{
			taxiPath[nPtCnt-1] = snapTail;
			bSnaped = true;
		}
	}

	if(bSnaped)
		FlushChange();
}




void CTaxiway3D::RenderEditPoint( C3DView * pView )
{
	
	if(!IsInEdit()) return;
	
	if(IsNeedSync()){
		DoSync();
	}	
	TaxiwayDisplayProp * pObjDisplay = (TaxiwayDisplayProp*) GetDisplayProp();//.get();
	glDisable(GL_TEXTURE_2D);
	glColor3ubv(pObjDisplay->m_dpCenterLine.cColor);
	for(ALTObjectControlPoint2008List::iterator itr = m_vControlPoints.begin();itr!=m_vControlPoints.end();itr++){
		glLoadName( pView->GetSelectionMap().NewSelectable( (*itr).get() ) );
		(*itr)->DrawSelect(pView);		 
	}
}



void CTaxiway3D::RenderHoldPosition( C3DView *pView )
{	
	for(int i=0;i<(int)m_vAllHoldPositions3D.size();i++){
		m_vAllHoldPositions3D.at(i)->Draw(pView);
	}

}
void CTaxiway3D::RenderHoldName( C3DView* pView )
{
	for(int i=0;i<(int)m_vAllHoldPositions3D.size();i++){
		ARCVector3 vLoc = m_vAllHoldPositions3D.at(i)->GetLocation();
		CString strText = m_vAllHoldPositions3D.at(i)->GetHoldPosition().m_strSign;
		TEXTMANAGER3D->DrawBitmapText(strText, vLoc );
	}
}
void CTaxiway3D::UpdateHoldPositions( const ALTAirport& airport, const ALTObject3DList&vRunwayList,const FilletTaxiwaysInAirport& vFillets, const IntersectionNodesInAirport& nodelist)
{
	std::vector<Runway*> vRunways;
	for(int i=0;i<(int)vRunwayList.size();i++)
	{
		const ALTObject3D * pObj3D = vRunwayList[i].get();
		if(pObj3D && pObj3D->GetObjectType() == ALT_RUNWAY )
		{
			vRunways.push_back( (Runway*)pObj3D->GetObject() );
		}
	}
	
	GetTaxiway()->UpdateHoldPositions(airport , vRunways, vFillets, nodelist);

	RefreshHoldPosition3D();
}

void CTaxiway3D::OnRotate( DistanceUnit dx )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	ARCVector3 center = GetLocation();
	GetTaxiway()->getPath().DoOffset(-center[VX],-center[VY],-center[VZ]);
	GetTaxiway()->getPath().Rotate(dx);
	GetTaxiway()->getPath().DoOffset(center[VX],center[VY],center[VZ]);
	DoSync();
}

void CTaxiway3D::RenderMarkings( C3DView * pView )
{
	if(IsNeedSync()){
		DoSync();
	}

	TaxiwayDisplayProp * pObjDisplay = (TaxiwayDisplayProp*)GetDisplayProp();// m_pDisplayProp.get();

	if(pObjDisplay->m_dpCenterLine.bOn){
		glColor3ubv(pObjDisplay->m_dpCenterLine.cColor);
		RenderMarkLine(pView);
	}
	if(pObjDisplay->m_dpMarking.bOn){
		glColor3ubv(pObjDisplay->m_dpMarking.cColor);
		RenderMarkText(pView);
	}	

	if(pObjDisplay->m_dpDir.bOn){
		glColor3ubv(pObjDisplay->m_dpDir.cColor);
	}

	RenderEditPoint(pView);	
}

void CTaxiway3D::RenderNodeNames( C3DView *pView )
{	
	glColor3ub(20,20,220);	

	//render path node
	CPath2008 taxiPath = GetTaxiway()->GetPath();
	for(int i=1;i < taxiPath.getCount()-1;i++)
	{
		CPoint2008 pos = taxiPath.getPoint(i);
		CString nodeName;
		nodeName.Format("%s %d", GetTaxiway()->GetMarking().c_str(), i);
		TEXTMANAGER3D->DrawBitmapText(nodeName, pos );
	}
}



void CTaxiway3D::SetRelatFillets( const CFilletTaxiway3DInAirport& vFillets )
{
	m_vFilletPoints.clear();
	for(int i =0 ;i< vFillets.GetCount(); i++ )
	{
		CFilletTaxiway3D* pFillet3D = vFillets.GetFillet3D(i);
		if( pFillet3D->m_filetTaxway.GetObject1ID() == GetTaxiway()->getID() )
		{
			m_vFilletPoints.push_back(pFillet3D->m_filetPt1.get());		
		}

		if( pFillet3D->m_filetTaxway.GetObject2ID() == GetTaxiway()->getID() )
		{
			m_vFilletPoints.push_back(pFillet3D->m_filetPt2.get());
		}
	}

}

void CTaxiway3D::AddEditPoint(double x, double y, double z)
{
	int recordPos = -1;
	CPath2008 &taxiPath = GetTaxiway()->getPath();
	int nCount = taxiPath.getCount();
	CPoint2008 newPoint;
	double minDis = 10000.0;
	for (int i=0; i<nCount-1; i++)
	{
		CPoint2008 pPrev = taxiPath[i];
		CPoint2008 pNext = taxiPath[i+1];
		DistanceUnit dist = abs((pNext.getY()-pPrev.getY())*(x-pPrev.getX())-(y-pPrev.getY())*(pNext.getX()-pPrev.getX()))/pPrev.getDistanceTo(pNext);
		if(dist<minDis)
		{
			CLine2008 proLine(pPrev, pNext);
			CPoint2008 proPoint = proLine.getProjectPoint(CPoint2008(x,y,z));
			if(proPoint.within(pPrev, pNext))
			{
				newPoint = proPoint;
				minDis = dist;
				recordPos = i;
			}
      	}
	}

	if(recordPos<0)
	{
		taxiPath.insertPointAfterAt(newPoint, 0);
	}
	else
	{
		taxiPath.insertPointAfterAt(newPoint, recordPos);
	}

	UpdateSync();

}

void CTaxiway3D::UpdateIntersectionNodes( const IntersectionNodesInAirport& nodelist )
{
	for(int i=0;i< nodelist.GetCount();i++)
	{
		IntersectionNode theNode = nodelist.NodeAt(i);
		if(theNode.HasObject( m_nID ) )
		{
			m_vIntersectionNodes.push_back( nodelist.NodeAt(i) );
		}
	}

	GetTaxiway()->InitHoldPositions(nodelist);
	RefreshHoldPosition3D();
}

void CTaxiway3D::RefreshHoldPosition3D()
{
	m_vAllHoldPositions3D.clear();
	for(int i =0 ;i<(int)GetTaxiway()->m_vAllHoldPositions.size();i++)
	{
		HoldPosition& hold = GetTaxiway()->m_vAllHoldPositions[i];

		if( hold.GetIntersectNode().GetRunwayIntersectItemList().size() )
		{
			m_vAllHoldPositions3D.push_back(new TaxiwayHoldPosition3D(GetTaxiway(),i));
		}		
	}

}


void TaxiwayMarking3D::Draw( C3DView* pView )
{
	std::string str = m_pTaxiway->GetMarking();									
	double _angle;	
	CPoint2008 pos;					
	m_pTaxiway->GetMarkingPosition(pos,_angle);
	DistanceUnit width= m_pTaxiway->GetWidth();

	ARCVector2 vdir( -sin(ARCMath::DegreesToRadians(_angle)),cos(ARCMath::DegreesToRadians(_angle)) );
	vdir.SetLength(width*0.33);

	pos -= CPoint2008( vdir[VX],vdir[VY], 0 );
	m_pPos = ARCVector3(pos);

	glDisable(GL_LIGHTING);	
	glDisable(GL_TEXTURE_2D);	
	TEXTMANAGER3D->DrawOutlineText(CString(str.c_str()),ARCVector3(pos.getX(),pos.getY(),0.0),(float)_angle,(float)width); 	
	glEnable(GL_LIGHTING);
}

void TaxiwayMarking3D::DrawSelect( C3DView * pView )
{	
	glLoadName( pView->GetSelectionMap().NewSelectable(this) );
	Draw(pView);
}

void TaxiwayMarking3D::OnMove( DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /*= 0*/ )
{
	if (m_pTaxiway->GetLocked())
	{
		return;
	}

	double _angle;	
	CPoint2008 pos;					
	m_pTaxiway->GetMarkingPosition(pos,_angle);
	double degreeAngle = ARCMath::DegreesToRadians(_angle);
	ARCVector3 vdir( cos(degreeAngle),sin(degreeAngle),0 );
	double d = vdir.dot(ARCVector3(dx,dy,dz))/vdir.Length();
	double Realdist = m_pTaxiway->GetPath().GetIndexDist((float)m_pTaxiway->GetMarkingPos()/100);
	Realdist+=d;
	double relatDist = m_pTaxiway->GetPath().GetDistIndex(Realdist);
	m_pPos = m_pTaxiway->GetPath().GetDistPoint(Realdist);
	m_pTaxiway->SetMarkingPos(relatDist*100);
	
}

ARCPoint3 TaxiwayMarking3D::GetLocation() const
{
	return m_pPos;
}

void TaxiwayMarking3D::AfterMove()
{
	m_pTaxiway->UpdateObject(m_pTaxiway->getID());
}

void TaxiwayHoldPosition3D::UpdateLine()
{
	GetTaxiway()->GetHoldPositionLine(m_nid,m_line);
}

HoldPosition & TaxiwayHoldPosition3D::GetHoldPosition()
{
	return GetTaxiway()->m_vAllHoldPositions[m_nid];
}



void CTaxiInterruptLine3D::Draw(C3DView* pView)
{
	
	std::auto_ptr<Taxiway> pTaxiway(new Taxiway);
	pTaxiway->ReadObject(m_TaxiwayID);

	double DistToStart = pTaxiway->GetPath().GetPointDist(m_Pt);
	//int Count = pTaxiway->GetPath().getCount();
	//CPoint2008 pos; 
	//double Pt2Start;
	//double NextPt2Start;
	////
	//int FrontPtID;
	//for (int i = 0 ; i<Count-1 ; i++ )
	//{
	//	pos = pTaxiway->GetPath().getPoint(i);
	//	Pt2Start = pTaxiway->GetPath().GetPointDist(pos);
	//	pos = pTaxiway->GetPath().getPoint(i+1);
	//	NextPt2Start = pTaxiway->GetPath().GetPointDist(pos);
	//	if (DistToStart>=Pt2Start&&DistToStart<NextPt2Start)
	//	{
	//		FrontPtID = i;
	//		break;
	//	}
	//}
	//if(FrontPtID >= Count)
	//	return;
	
	//CPoint2008 dir = CPoint2008(pTaxiway->GetPath().getPoint(FrontPtID) - pTaxiway->GetPath().getPoint(FrontPtID+1));
	//dir.setZ(0);
	//dir.Normalize();
	//dir.rotate(90);
	//dir.length(pTaxiway->GetWidth()*0.5);
	//pos = pTaxiway->GetPath().GetDistPoint(DistToStart);
	//
	//CLine2008 line( pos + dir, pos - dir);

		CPoint2008 dir = CPoint2008(pTaxiway->GetPath().GetDistDir(DistToStart));// CPoint2008(pTaxiway->GetPath().getPoint(FrontPtID) - pTaxiway->GetPath().getPoint(FrontPtID+1));
		dir.setZ(0);
		dir.Normalize();
		dir.rotate(90);
		dir.length(pTaxiway->GetWidth()*0.5);
		CPoint2008 pos = pTaxiway->GetPath().GetDistPoint(DistToStart);
		CLine2008 line( pos + dir, pos - dir);

		CTexture * pTexture = pView->getTextureResource()->getTexture("HoldPosition");
		if(pTexture)
			pTexture->Apply();
        glColor3f(1.0,1.0,0.0);
		glNormal3f(0.0,0.0,1.0);
	
	//glColor3f(1.0,1.0,0.0);
	//glNormal3f(0.0,0.0,1.0);

	CPoint2008 p1 = line.GetPoint1();
	CPoint2008 p2 = line.GetPoint2();

	DistanceUnit distance = p1.distance3D(p2);
	CPath2008  linePath;
	linePath.init(2);

	linePath[0]=p1;
	linePath[1]=p2;
	
	ARCPipe pipePath(linePath,300);
	double texcordT = distance * 0.004;
	
	glBegin(GL_QUAD_STRIP);
	glTexCoord2d(0,0);
	glVertex3dv(pipePath.m_sidePath1[0]);
	glTexCoord2d(0,1);
	glVertex3dv(pipePath.m_sidePath2[0]);

	glTexCoord2d(texcordT,0);
	glVertex3dv(pipePath.m_sidePath1[1]);
	glTexCoord2d(texcordT,1);
	glVertex3dv(pipePath.m_sidePath2[1]);
	glEnd();

	if(pTexture)
		pTexture->UnApply();
	
}

/*
void CTaxiInterruptLine3D::Draw(C3DView* pView)
{
std::vector<TaxiwayIntersectItem*> vTaxiwayItems1 = m_FrontIntersectionNode.GetTaxiwayIntersectItemList();
std::vector<TaxiwayIntersectItem*> vTaxiwayItems2 = m_BackIntersectionNode.GetTaxiwayIntersectItemList();

TaxiwayIntersectItem* pIntersectItem = NULL;
int nNum1 = vTaxiwayItems1.size();
int nNum2 = vTaxiwayItems2.size();

for(int i = 0; i< nNum1;i++)
{
for (int j =0; j <nNum2; j++)
{
if (vTaxiwayItems1[i]->GetObjectID() == vTaxiwayItems2[j]->GetObjectID())
{
pIntersectItem = vTaxiwayItems1.at(i);
break;
}
}
}

if( pIntersectItem != NULL )
{
Taxiway* pTaxiway = pIntersectItem->GetTaxiway();
CPath2008 pTaxiwayPath = pTaxiway->GetPath();
double fullLength = pTaxiwayPath.GetTotalLength();
if(pIntersectItem->GetDistInItem()+m_dDistanceToInterNode>fullLength)
m_dDistanceToInterNode = fullLength - pIntersectItem->GetDistInItem();
if(pIntersectItem->GetDistInItem()+m_dDistanceToInterNode<0)
m_dDistanceToInterNode = - pIntersectItem->GetDistInItem();
CPoint2008 ptHoldShort = pTaxiwayPath.GetDistPoint(pIntersectItem->GetDistInItem()+m_dDistanceToInterNode);
float relateLen = pTaxiwayPath.GetDistIndex(pIntersectItem->GetDistInItem()+m_dDistanceToInterNode);
int npt = (int)relateLen;
int nptnext;
if(npt >= pTaxiwayPath.getCount() -1 )
{
nptnext = pTaxiwayPath.getCount() -2;
npt = nptnext +1;
}
else 
{
nptnext = npt +1;
}

CPoint2008 dir = pTaxiwayPath.getPoint(npt) - pTaxiwayPath.getPoint(nptnext);
dir.setZ(0);
dir.Normalize();
dir.rotate(90);
dir.length(pTaxiway->GetWidth()*0.5);
CLine2008 line( ptHoldShort + dir, ptHoldShort -dir);

CTexture * pTexture = pView->getTextureResource()->getTexture("HoldPosition");
if(pTexture)
pTexture->Apply();
glColor3f(1.0,1.0,0.0);
glNormal3f(0.0,0.0,1.0);

CPoint2008 p1 = line.GetPoint1();
CPoint2008 p2 = line.GetPoint2();

DistanceUnit distance = p1.distance3D(p2);
CPath2008  linePath;
linePath.init(2);

linePath[0]=p1;
linePath[1]=p2;

ARCPipe pipePath(linePath,300);
double texcordT = distance * 0.004;

glBegin(GL_QUAD_STRIP);
glTexCoord2d(0,0);
glVertex3dv(pipePath.m_sidePath1[0]);
glTexCoord2d(0,1);
glVertex3dv(pipePath.m_sidePath2[0]);

glTexCoord2d(texcordT,0);
glVertex3dv(pipePath.m_sidePath1[1]);
glTexCoord2d(texcordT,1);
glVertex3dv(pipePath.m_sidePath2[1]);
glEnd();
pTexture->UnApply();
}
}
*/

void CTaxiInterruptLine3D::DrawSelect(C3DView* pView)
{
	//Draw(pView);
}

void CTaxiInterruptLine3D::MoveLocation(DistanceUnit dx, DistanceUnit dy, DistanceUnit dz/* =0 */)
{
	std::vector<TaxiwayIntersectItem*> vTaxiwayItems1 = m_FrontIntersectionNode.GetTaxiwayIntersectItemList();
	std::vector<TaxiwayIntersectItem*> vTaxiwayItems2 = m_BackIntersectionNode.GetTaxiwayIntersectItemList();

	TaxiwayIntersectItem* pIntersectItem = NULL;
	int nNum1 = vTaxiwayItems1.size();
	int nNum2 = vTaxiwayItems2.size();

	for(int i = 0; i< nNum1;i++)
	{
		for (int j =0; j <nNum2; j++)
		{
			if (vTaxiwayItems1[i]->GetObjectID() == vTaxiwayItems2[j]->GetObjectID())
			{
				pIntersectItem = vTaxiwayItems1.at(i);
				break;
			}
		}
	}

	if( pIntersectItem != NULL )
	{
		
		CPath2008 taxiPath = pIntersectItem->GetTaxiway()->GetPath();
		double relateLen = taxiPath.GetDistIndex(pIntersectItem->GetDistInItem() + m_dDistanceToInterNode);
		int npt = (int)relateLen;
		if(npt>=taxiPath.getCount()-1)
			npt = taxiPath.getCount()-2;
        ARCVector3 vdir = taxiPath.getPoint(npt+1)-taxiPath.getPoint(npt);
		vdir[VZ]=0;
		vdir.Normalize();
		double newDis = vdir.dot(ARCVector3(dx,dy,dz))/vdir.Length();
        m_dDistanceToInterNode += newDis;
        double fullLength = taxiPath.GetTotalLength();
		if(pIntersectItem->GetDistInItem() + m_dDistanceToInterNode > fullLength)
			m_dDistanceToInterNode = fullLength - pIntersectItem->GetDistInItem();
		if(pIntersectItem->GetDistInItem() + m_dDistanceToInterNode <0)
			m_dDistanceToInterNode = - pIntersectItem->GetDistInItem();
	}
	
}

void CTaxiInterruptLine3D::FlushChangeOf()
{
	CTaxiInterruptLine* holdShortLineNode;
	TaxiInterruptLineList  holdshortlines;
	holdshortlines.ReadData(-1);
	holdShortLineNode = holdshortlines.GetItem(m_nItem);
	holdShortLineNode->m_dDistToFrontInterNode = m_dDistanceToInterNode;    
	
	holdShortLineNode->UpdateData();
}

CTaxiInterruptLine3D::CTaxiInterruptLine3D(int nFrontInterNodeId,double distance,int nBackInterNodeId, int item, CString strName,int TaxiwayID, CPoint2008 position) 
:m_nFrontIntersectionID(nFrontInterNodeId)
,m_nBackIntersectionID(nBackInterNodeId)
,m_dDistanceToInterNode(distance)
,m_strName(strName)
,m_TaxiwayID(TaxiwayID)
,m_Pt(position)
{
	m_FrontIntersectionNode.ReadData(m_nFrontIntersectionID);
	m_BackIntersectionNode.ReadData(m_nBackIntersectionID);
	m_nItem = item;
}

