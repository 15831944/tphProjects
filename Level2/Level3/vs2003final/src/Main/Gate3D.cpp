#include "StdAfx.h"
#include ".\gate3d.h"
#include ".\..\InputAirside\stand.h"
#include ".\..\InputAirside\ALTObjectDisplayProp.h"
#include ".\ShapeRenderer.h"
#include ".\3DView.h"
#include "./Taxiway3D.h"
#include "./../InputAirside/Taxiway.h"
#include "../Common/PathSnapFunctor.h"
#include "glrender/glTextureResource.h"

CStand3D::CStand3D(int id):ALTObject3D(id)
{
	m_pObj = new Stand;
	//m_pDisplayProp  = new StandDisplayProp;
	m_bInEdit = false;
}

CStand3D::~CStand3D(void)
{
}

void CStand3D::DrawOGL( C3DView * pView )
{
	if(IsSelected()){
		glLineWidth(3.0f);
	}
	if(IsNeedSync()){
		DoSync();
	}
	glEnable(GL_LIGHTING);
	Stand * pObj = (Stand*) m_pObj.get();
	StandDisplayProp * pDisplay = (StandDisplayProp * )GetDisplayProp();//.get();

	CPoint2008 pt = pObj->GetServicePoint();

	//draw shape
	if( pDisplay->m_dpShape.bOn ) {
		/*glPushMatrix();
		glTranslated(pt.getX(),pt.getY(),pt.getZ());
		
		CglShape* pShape =  pView->GetShapeResource()->GetShape("Stand");
		if(pShape) pShape->display();

		glPopMatrix();*/
		glColor4ubv(pDisplay->m_dpShape.cColor);
		CTexture * pTex = pView->getTextureResource()->getTexture("StandBar");
		if(pTex)pTex->Apply();
		GLUquadric* pQuadObj = gluNewQuadric();
		gluQuadricTexture(pQuadObj,GL_TRUE);
		glPushMatrix();
		glTranslated(pt.getX(),pt.getY(),pt.getZ());						
		gluCylinder(pQuadObj, 25.0, 25.0, 500.0, 16, 5);

		glDisable(GL_TEXTURE_2D);
		glPushMatrix();					
		glTranslated(0,0,500);
		glColor3ubv(ARCColor3::BLACK);		
		gluDisk(pQuadObj,0.0,25.0,16,1);
		glPopMatrix();

		glColor4ubv(pDisplay->m_dpShape.cColor);
		gluDisk(pQuadObj, 0.0, 250.0, 16, 1);

		glPopMatrix();
		gluDeleteQuadric(pQuadObj);
	}
	
	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);
	////draw in constrain
	//if( pDisplay->m_dpInConstr.bOn && !(pObj->IsUsePushBack())){
	//	glColor4ubv(pDisplay->m_dpInConstr.cColor);
	//	ARCGLRender::DrawArrowPath(pObj->GetInContrain(),68);		
	//}

	////draw out constrain
	//if( pDisplay->m_dpOutConstr.bOn && !(pObj->IsUsePushBack())){
	//	glColor4ubv(pDisplay->m_dpOutConstr.cColor);
	//	ARCGLRender::DrawArrowPath(pObj->GetOutConstrain(),68);		
	//}
	//draw Push Back ways
	//if(pObj->IsUsePushBack() && pObj->GetInContrain().getCount() )
	//{
	//	glColor4ubv(pDisplay->m_dpOutConstr.cColor);
	//	ARCGLRender::DrawArrowPath(pObj->GetInContrain(),68);
	//	CPath2008 reversPath = pObj->GetInContrain();
	//	std::reverse(&reversPath[0],&reversPath[0] + reversPath.getCount());
	//	ARCGLRender::DrawArrowPath(reversPath,68);
	//	if(IsInEdit()){		
	//		glNormal3f(0,0,1);
	//		for(ALTObjectControlPointList::iterator itr = m_vInConstPoint.begin();itr!=m_vInConstPoint.end();itr++){
	//			(*itr)->DrawSelect(pView);		 
	//		}			
	//	}
	//}	

	RenderLeadInOutLines(pView);

	if( IsInEdit() )
	{
		RenderEditPoint(pView);
	}

	glDepthMask(GL_TRUE);

	if(IsSelected()){
		glLineWidth(1.0f);
	}
}

void CStand3D::DrawSelect( C3DView * pView )
{	
	DrawOGL(pView);	
	if(IsInEdit())
        RenderEditPoint(pView,true);
}

ARCPoint3 CStand3D::GetLocation( void ) const
{
	Stand * pObj = (Stand*)m_pObj.get(); 
	return pObj->GetServicePoint();
}


void CStand3D::OnMove( DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /*= 0*/ )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	GetStand()->DoOffset(dx,dy,dz);
}

Stand * CStand3D::GetStand()
{
	return (Stand*) m_pObj.get();
}

//StandDisplayProp * CStand3D::GetDisplayProp()
//{
//	return (StandDisplayProp*)m_pDisplayProp.get();
//}

void CStand3D::DoSync()
{	
	m_vInConstPoint.clear();
	m_vOutConstPoint.clear();

	m_pServePoint = new CALTObjectPointControlPoint2008(this,GetStand()->m_servicePoint);
	//const CPath2008& InPath = GetStand()->GetInContrain();
	//for(int i=0;i<InPath.getCount();++i){
	//	m_vInConstPoint.push_back( new CALTObjectPathControlPoint2008(this,GetStand()->m_inContrain,i) );
	//}

	//const CPath2008& OutPath = GetStand()->GetOutConstrain();
	//for(int i=0;i<OutPath.getCount();++i){
	//	m_vOutConstPoint.push_back( new CALTObjectPathControlPoint2008(this,GetStand()->m_outConstrain,i) );
	//}

	//
	m_vLeadInLinesPoints.clear();
	m_vLeadOutLinesPoints.clear();
	m_vLeadInLinesPoints.reserve( GetStand()->m_vLeadInLines.GetItemCount() );
	m_vLeadOutLinesPoints.reserve( GetStand()->m_vLeadOutLines.GetItemCount() );
	for(size_t i=0;i< GetStand()->m_vLeadInLines.GetItemCount();i++)
	{
		CPath2008& path = GetStand()->m_vLeadInLines.ItemAt(i).getPath();
		ALTObjectControlPoint2008List ptList;
		ptList.reserve(path.getCount());
		for(int j=0;j<path.getCount();j++)
		{
			ptList.push_back( new CALTObjectPathControlPoint2008(this,path,j) );
		}
		m_vLeadInLinesPoints.push_back(ptList);
	}
	for(size_t i=0;i< GetStand()->m_vLeadOutLines.GetItemCount();i++ )
	{
		CPath2008& path = GetStand()->m_vLeadOutLines.ItemAt(i).getPath();
		ALTObjectControlPoint2008List ptList;
		ptList.reserve(path.getCount());
		for(int j=0;j<path.getCount();j++)
		{
			ptList.push_back( new CALTObjectPathControlPoint2008(this,path,j) );
		}
		m_vLeadOutLinesPoints.push_back(ptList);
	}


	SyncDateType::DoSync();
}



void CStand3D::FlushChange()
{
	try
	{
		GetStand()->UpdateObject(m_nID);
		//GetStand()->UpdateInConstraint();
		//GetStand()->UpdateOutConstraint();
		GetStand()->m_vLeadInLines.SaveData(GetStand()->getID());
		GetStand()->m_vLeadOutLines.SaveData(GetStand()->getID());

	}
	catch (CADOException& e)
	{
		CString strErr = e.ErrorMessage(); 
	}	
}

void CStand3D::SnapTo( const ALTObject3DList& TaxiList )
{
	
	{ //leading lines
		bool bSnaped = false;
		StandLeadInLineList& leadInLineList = GetStand()->m_vLeadInLines;
		for(int i=0;i<(int)leadInLineList.GetItemCount();i++)
		{
			CPath2008& path = leadInLineList.ItemAt(i).getPath();
			int nPtCnt = path.getCount();
			if(nPtCnt < 2 )continue;

			CPoint2008 headPt = path.getPoint(0);
			CPoint2008 vHead = CPoint2008(headPt - path.getPoint(1));
			CPoint2008 snapHead = headPt;
			DistanceUnit minDistHead = ARCMath::DISTANCE_INFINITE;
			bool bSnapHead = false;

			for(ALTObject3DList::const_iterator itr = TaxiList.begin();itr!=TaxiList.end();itr++)
			{
				const ALTObject3D* pObj3D = (*itr).get();
				if(pObj3D->GetObjectType()!= ALT_TAXIWAY)
					continue;

				Taxiway* pTaxiway = (Taxiway*)pObj3D->GetObject();
				PathSnapFunctor pathSnapFuc(pTaxiway->getPath(), pTaxiway->GetWidth()*0.5);
				CPoint2008 snapPt;
				if(pathSnapFuc.GetSnapPoint(headPt, vHead, snapPt))
				{
					DistanceUnit snapDist = pathSnapFuc.mindist;
					if(snapDist < minDistHead)
					{
						minDistHead = snapDist;
						snapHead = snapPt;
						bSnapHead = true;
					}
				}
			}

			if(bSnapHead)
			{
				if( path[0].distance3D(snapHead) > ARCMath::EPSILON )
				{
					bSnaped = true;
					path[0]  = snapHead;
				}
			}

		}
		if(bSnaped)
			leadInLineList.SaveData(GetStand()->getID());

	}
	{  //lead out lines
		bool bSnaped = false;
		StandLeadOutLineList& leadOutLIneList = GetStand()->m_vLeadOutLines;
		for(int i=0;i<(int)leadOutLIneList.GetItemCount(); i++)
		{
			CPath2008& path = leadOutLIneList.ItemAt(i).getPath();
			int nPtCnt = path.getCount();
			if(nPtCnt < 2) continue;

			CPoint2008 trailPt = path.getPoint(nPtCnt -1);
			CPoint2008 vTail = CPoint2008(path.getPoint(nPtCnt -2) - trailPt);
			CPoint2008 snapTail = trailPt;
			DistanceUnit minDistTrail = ARCMath::DISTANCE_INFINITE;
			bool bSnapTrail = false;

			
			for(ALTObject3DList::const_iterator itr = TaxiList.begin();itr!=TaxiList.end();itr++)
			{
				const ALTObject3D* pObj3D = (*itr).get();
				if(pObj3D->GetObjectType()!= ALT_TAXIWAY)
					continue;

				Taxiway* pTaxiway = (Taxiway*)pObj3D->GetObject();
				PathSnapFunctor pathSnapFuc(pTaxiway->getPath(), pTaxiway->GetWidth()*0.5);
				CPoint2008 snapPt;
				if(pathSnapFuc.GetSnapPoint(trailPt, vTail, snapPt))
				{
					DistanceUnit snapDist = pathSnapFuc.mindist;
					if(snapDist < minDistTrail)
					{
						minDistTrail = snapDist;
						snapTail = snapPt;
						bSnapTrail = true;
					}
				}
			}
			
			if( bSnapTrail) 
			{
				if( path[nPtCnt-1].distance3D(snapTail) > ARCMath::EPSILON )
				{
					bSnaped = true;
					path[nPtCnt-1] =  snapTail;
				}
			}
		}
		if(bSnaped )
			leadOutLIneList.SaveData(GetStand()->getID());
	}

	
}

void CStand3D::SnapTo( const ALTObject3D* Taxi )
{
	const ALTObject * pObj = Taxi->GetObject();
	if(pObj->GetType() == ALT_TAXIWAY  )
	{
		Taxiway* pTaxi = (Taxiway*)pObj;

		double width = pTaxi->GetWidth();

		PathSnapFunctor snapFunc(pTaxi->getPath(), pTaxi->GetWidth()*0.5);
		//lead in/out line
		StandLeadInLineList& leadInLineList = GetStand()->m_vLeadInLines;
		for(size_t i=0;i< leadInLineList.GetItemCount();i++)
		{
			CPath2008& path = leadInLineList.ItemAt(i).getPath();
			if( path.getCount() > 0)
			{
				CPoint2008 retPt;
				if( pTaxi->GetPath().GetSnapPoint(width, path.getPoint(0), retPt) )
				{
					path[0] = retPt;
				}
			}
		}
		StandLeadOutLineList& leadOutLineList = GetStand()->m_vLeadOutLines;
		for(size_t i=0; i < leadOutLineList.GetItemCount();i++ )
		{
			CPath2008& path = leadOutLineList.ItemAt(i).getPath();
			if( path.getCount() > 0 )
			{
				CPoint2008 retPt;
				if( pTaxi->GetPath().GetSnapPoint(width,path.getPoint(path.getCount()-1),retPt) )
				{
					path[path.getCount()-1] = retPt;
				}
			}
		}
	}
}

void CStand3D::RenderEditPoint( C3DView * pView, bool bSel )
{

	StandDisplayProp * pDisplay = (StandDisplayProp * )GetDisplayProp();//.get();

	//glLoadName( pView->GetSelectionMap().NewSelectable( (m_pServePoint).get() ) );
	//m_pServePoint->DrawSelect(pView);	

	//lead in /out control points
	if(pDisplay->m_dpInConstr.bOn)
	{
		
		glColor4ubv(pDisplay->m_dpInConstr.cColor);

		//for(ALTObjectControlPointList::iterator itr = m_vInConstPoint.begin();itr!=m_vInConstPoint.end();itr++){
		//	if(bSel)
		//		glLoadName( pView->GetSelectionMap().NewSelectable( (*itr).get() ) );
		//	(*itr)->DrawSelect(pView);		 
		//}

		for(int i =0;i< (int)m_vLeadInLinesPoints.size();i++)
		{
			for(ALTObjectControlPoint2008List::iterator itr= m_vLeadInLinesPoints[i].begin();itr!= m_vLeadInLinesPoints[i].end(); itr++)
			{
				if(bSel)
					glLoadName( pView->GetSelectionMap().NewSelectable( (*itr).get() ) );
				(*itr)->DrawSelect(pView);
			}
		}
	}

	if(pDisplay->m_dpOutConstr.bOn)
	{
	
		glColor4ubv(pDisplay->m_dpOutConstr.cColor);

		//if(!GetStand()->IsUsePushBack())
		//{
		//	for(ALTObjectControlPointList::iterator itr = m_vOutConstPoint.begin();itr!=m_vOutConstPoint.end();itr++){
		//		if(bSel)
		//			glLoadName( pView->GetSelectionMap().NewSelectable( (*itr).get() ) );
		//		(*itr)->DrawSelect(pView);		 
		//	}
		//}

		for(int i =0;i< (int)m_vLeadOutLinesPoints.size();i++)
		{
			for(ALTObjectControlPoint2008List::iterator itr= m_vLeadOutLinesPoints[i].begin();itr!= m_vLeadOutLinesPoints[i].end(); itr++)
			{
				if(bSel)
					glLoadName( pView->GetSelectionMap().NewSelectable( (*itr).get() ) );
				(*itr)->DrawSelect(pView);
			}
		}

	}

}

void CStand3D::OnRotate( DistanceUnit dx )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	GetStand()->OnRotate(dx, GetStand()->GetServicePoint() );	
}

void CStand3D::RenderLeadInOutLines( C3DView * pView )
{
	Stand * pStand = (Stand*) m_pObj.get();
	StandDisplayProp * pDisplay = (StandDisplayProp * )GetDisplayProp();//.get();
	if( pDisplay->m_dpInConstr.bOn ){
		glColor4ubv(pDisplay->m_dpInConstr.cColor);
		
		for(size_t i=0;i<pStand->m_vLeadInLines.GetItemCount();i++)
		{
			StandLeadInLine  *pLeadIn = &pStand->m_vLeadInLines.ItemAt(i); 
			ARCGLRender::DrawArrowPath(pLeadIn->getPath(),68);			
		}		
	}
	if( pDisplay->m_dpOutConstr.bOn)
	{
		glColor4ubv(pDisplay->m_dpOutConstr.cColor);
		for(size_t i=0;i<pStand->m_vLeadOutLines.GetItemCount();i++)
		{
			StandLeadOutLine * pLine = &pStand->m_vLeadOutLines.ItemAt(i);
			ARCGLRender::DrawArrowPath(pLine->getPath(),68);
			if (pLine->IsSetReleasePoint())
			{
				CPoint2008 pt = pLine->GetReleasePoint();
				ARCVector3 dir = (pLine->getPath().GetIndexDir(pLine->getPath().getCount()-1.0f));
				dir.RotateXY(90);
				dir.SetLength(68);				

				CPath2008 path;
				path.init(2);
				path[0] = pt + dir ;
				path[1] = pt - dir;
				ARCGLRender::DrawWidthPath(path,34);
			}
		}
	}

}
