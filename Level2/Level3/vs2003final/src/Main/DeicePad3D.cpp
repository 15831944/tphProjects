#include "StdAfx.h"
#include ".\deicepad3d.h"
#include ".\..\InputAirside\DeicePad.h"
#include ".\..\InputAirside\ALTObjectDisplayProp.h"
#include "glrender/glTextureResource.h"
#include ".\3DView.h"
#include "ShapeRenderer.h"
#include "./../InputAirside/Taxiway.h"
#include "glrender/glShapeResource.h"

CDeicePad3D::CDeicePad3D(int id): ALTObject3D(id)
{
	m_pObj =  new DeicePad;
	//m_pDisplayProp = new DeicePadDisplayProp;
	m_bInEdit = false;
}

CDeicePad3D::~CDeicePad3D(void)
{
}

void CDeicePad3D::DrawOGL( C3DView * pView )
{
	if(IsSelected()){
		glLineWidth(3.0f);
	}

	if(IsNeedSync()){
		DoSync();
	}

	DeicePad * pObj = (DeicePad * )m_pObj.get();
	DeicePadDisplayProp  * pDisplay = (DeicePadDisplayProp  *)GetDisplayProp();
	CPoint2008 serverpt = pObj->GetServicePoint();
	

	//draw pad
	//RenderSurface(pView);


	RenderStand(pView);
	
	RenderTruck(pView);

	RenderInOutConstrain(pView);

	
	if(IsInEdit()){
		glColor3ub(255,255,255);
		glNormal3f(0,0,1);
		for(ALTObjectControlPoint2008List::iterator itr = m_vInConstPoint.begin();itr!=m_vInConstPoint.end();itr++){
			(*itr)->DrawSelect(pView);		 
		}

		if(!GetDeicePad()->IsUsePushBack()){
			for(ALTObjectControlPoint2008List::iterator itr = m_vOutConstPoint.begin();itr!=m_vOutConstPoint.end();itr++){
				(*itr)->DrawSelect(pView);		 
			}
		}
	}
	
	

	if(IsSelected()){
		glLineWidth(1.0f);
	}
}

void CDeicePad3D::DrawSelect( C3DView * pView )
{
	/*DeicePad * pObj = (DeicePad * )m_pObj.get();
	CPoint2008 serverpt = pObj->GetServicePoint();
	const Path& inConstrain = pObj->GetInContrain();

	
	int nCont;
	if( ( nCont = inConstrain.getCount() ) >  1 ){
		ARCVector2 vdir(inConstrain.getPoint(nCont-1));
		vdir -= inConstrain.getPoint(nCont-2);		

		glPushMatrix();
		glTranslated(serverpt.getX(),serverpt.getY(),serverpt.getZ());
		glRotated(vdir.AngleFromCoorndinateX(),0,0,1);
		DrawFlatSquare(pObj->GetWinSpan(),pObj->GetLength());
		glPopMatrix();
	}	*/
	RenderSurface(pView);


	RenderStand(pView);
	if(IsInEdit()){
		RenderEditPoint(pView);
	}
}

ARCPoint3 CDeicePad3D::GetLocation( void ) const
{
	DeicePad * pObj = (DeicePad*)m_pObj.get(); 
	return pObj->GetServicePoint();
}

DeicePad * CDeicePad3D::GetDeicePad()
{
	return (DeicePad*)m_pObj.get();
}

//DeicePadDisplayProp * CDeicePad3D::GetDisplayProp()
//{
//	return (DeicePadDisplayProp*)m_pDisplayProp.get();
//}

void CDeicePad3D::OnMove( DistanceUnit dx, DistanceUnit dy,DistanceUnit dz/*= 0*/ )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	GetDeicePad()->m_servicePoint.DoOffset(dx,dy,dz);
	GetDeicePad()->m_inContrain.DoOffset(dx,dy,dz);
	GetDeicePad()->m_outConstrain.DoOffset(dx,dy,dz);
}

void CDeicePad3D::DoSync()
{
	m_vInConstPoint.clear();
	m_vOutConstPoint.clear();

	m_pServePoint = new CALTObjectPointControlPoint2008(this,GetDeicePad()->m_servicePoint);

	const Path& InPath = GetDeicePad()->GetInContrain();
	for(int i=0;i<InPath.getCount();++i)
	{
		m_vInConstPoint.push_back( new CALTObjectPathControlPoint2008(this,GetDeicePad()->m_inContrain,i) );
	}

	const Path& OutPath = GetDeicePad()->GetOutConstrain();
	for(int i=0;i<OutPath.getCount();++i)
	{
		m_vOutConstPoint.push_back( new CALTObjectPathControlPoint2008(this,GetDeicePad()->m_outConstrain,i) );
	}

	SyncDateType::DoSync();
}

void CDeicePad3D::FlushChange()
{
	try
	{
		GetDeicePad()->UpdateObject(m_nID);
		GetDeicePad()->UpdateInConstraint();
		GetDeicePad()->UpdateOutConstraint();
		//GetDeicePad()->Update

	}
	catch (CADOException& e)
	{
		CString strErr = e.ErrorMessage(); 
	}
	SyncDateType::Update();
}

void CDeicePad3D::RenderSurface( C3DView * pView )
{
	DeicePad * pObj = (DeicePad * )m_pObj.get();
	DeicePadDisplayProp  * pDisplay = (DeicePadDisplayProp  *)GetDisplayProp();//.get();
	CPoint2008 serverpt = pObj->GetServicePoint();

	glEnable(GL_LIGHTING);
	if( pDisplay->m_dpPad.bOn){
		glColor4ubv(pDisplay->m_dpPad.cColor);
		
		ARCVector2 vdir = pObj->GetDir();
		CPoint2008 laypt;
		laypt.setX(serverpt.getX() - pObj->GetLength()*cos(ARCMath::DegreesToRadians(vdir.AngleFromCoorndinateX()))*0.5);
		laypt.setY(serverpt.getY() - pObj->GetLength()*sin(ARCMath::DegreesToRadians(vdir.AngleFromCoorndinateX()))*0.5);
		laypt.setZ(serverpt.getZ());

		glPushMatrix();
		glNormal3f(0,0,1);
		glTranslated(laypt.getX(),laypt.getY(),laypt.getZ());
		glRotated(vdir.AngleFromCoorndinateX(),0,0,1);
		DrawFlatSquare(pObj->GetLength(),pObj->GetWinSpan());
		glPopMatrix();
		
	}
	
}

void CDeicePad3D::RenderStand( C3DView * pView )
{
	DeicePad * pObj = (DeicePad * )m_pObj.get();
	DeicePadDisplayProp  * pDisplay = (DeicePadDisplayProp  *)GetDisplayProp();//.get();
	CPoint2008 serverpt = pObj->GetServicePoint();
	const Path& inConstrain = pObj->GetInContrain();


	if(pDisplay->m_dpShape.bOn){
		CTexture * pTex = pView->getTextureResource()->getTexture("StandBar");
		if(pTex)pTex->Apply();
		GLUquadric* pQuadObj = gluNewQuadric();
		gluQuadricTexture(pQuadObj,GL_TRUE);
		glPushMatrix();
		glTranslated(serverpt.getX(),serverpt.getY(),serverpt.getZ()+ 1);						
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
	
	

	
	
}

void CDeicePad3D::RenderInOutConstrain( C3DView * pView )
{
	
	glDisable(GL_LIGHTING);
	//if(IsSelected())glLineWidth(3.0f);

	DeicePad * pObj = (DeicePad * )m_pObj.get();
	DeicePadDisplayProp  * pDisplay = (DeicePadDisplayProp  *)GetDisplayProp();//.get();
	
	//draw in constrain
	if( pDisplay->m_dpInConstr.bOn && !(pObj->IsUsePushBack())){
		glColor4ubv(pDisplay->m_dpInConstr.cColor);
		ARCGLRender::DrawArrowPath(pObj->GetInContrain(),200);
		if(IsInEdit()){	
			glNormal3f(0,0,1);
			for(ALTObjectControlPoint2008List::iterator itr = m_vInConstPoint.begin();itr!=m_vInConstPoint.end();itr++){
				(*itr)->DrawSelect(pView);		 
			}			
		}
	}

	//draw out constrain
	if( pDisplay->m_dpOutConstr.bOn && !(pObj->IsUsePushBack())){
		glColor4ubv(pDisplay->m_dpOutConstr.cColor);
		ARCGLRender::DrawArrowPath(pObj->GetOutConstrain(),200);

		if(IsInEdit()){		
			glNormal3f(0,0,1);			
			for(ALTObjectControlPoint2008List::iterator itr = m_vOutConstPoint.begin();itr!=m_vOutConstPoint.end();itr++){
				(*itr)->DrawSelect(pView);		 
			}			
		}
	}
	//draw Push Back ways
	if(pObj->IsUsePushBack())
	{
		glColor4ubv(pDisplay->m_dpOutConstr.cColor);
		ARCGLRender::DrawArrowPath(pObj->GetInContrain(),200);
		Path reversPath = pObj->GetInContrain();
		std::reverse(&reversPath[0],&reversPath[0] + reversPath.getCount());
		ARCGLRender::DrawArrowPath(reversPath,200);
		if(IsInEdit()){		
			glNormal3f(0,0,1);
			for(ALTObjectControlPoint2008List::iterator itr = m_vInConstPoint.begin();itr!=m_vInConstPoint.end();itr++){
				(*itr)->DrawSelect(pView);		 
			}			
		}
	}	

	//if(IsSelected())glLineWidth(1.0f);
	glEnable(GL_LIGHTING);


	
}

void CDeicePad3D::RenderTruck( C3DView * pView )
{
	
	DeicePad * pObj = (DeicePad * )m_pObj.get();
	DeicePadDisplayProp  * pDisplay = (DeicePadDisplayProp  *)GetDisplayProp();//.get();

	if(pDisplay->m_dpTruck.bOn){


		CPoint2008 serverpt = pObj->GetServicePoint();
		ARCVector2 vdir = pObj->GetDir();
		double dAngleFromX =- ARCMath::RadiansToDegrees(acos(vdir[VX]));

		vdir.Rotate(90);

		ARCVector2 vSpan = 0.4 * pObj->GetWinSpan() * vdir;
		ARCVector2 vLength = 0.4 * pObj->GetLength()* pObj->GetDir();


		ARCVector3 vpos1 = ARCVector3(pObj->GetServicePoint() ) + ARCVector3( vSpan[VX],vSpan[VY],0) + ARCVector3(vLength[VX],vLength[VY] ,0.0) ;
		ARCVector3 vpos2 = ARCVector3 (pObj->GetServicePoint()) - ARCVector3( vSpan[VX],vSpan[VY], 0 ) + ARCVector3(vLength[VX],vLength[VY],0.0 );		

		CglShape * pShape = pView->GetShapeResource()->GetShape("DeiceTruck");
		glColor3ubv(pDisplay->m_dpTruck.cColor);
		
		glPushMatrix();
		glTranslated(vpos1[VX],vpos1[VY],vpos1[VZ]);
		glRotated(dAngleFromX,0,0,1);
		if(pShape) pShape->display();

		glPopMatrix();

		glPushMatrix();
		glTranslated(vpos2[VX],vpos2[VY],vpos2[VZ]);
		glRotated(dAngleFromX+90,0,0,1);
		if(pShape)pShape->display();
		glPopMatrix();
	}
}

void CDeicePad3D::SnapTo( const ALTObject3DList& TaxiList )
{
	for(ALTObject3DList::const_iterator itr = TaxiList.begin();itr!=TaxiList.end();itr++){
		const ALTObject3D * pObj  = (*itr).get();
		SnapTo(pObj);
	}

}

void CDeicePad3D::SnapTo( const ALTObject3D* Taxi )
{
	const ALTObject * pObj = Taxi->GetObject();
	if(pObj->GetType() == ALT_TAXIWAY  )
	{
		Taxiway* pTaxi = (Taxiway*)pObj;

		double width = pTaxi->GetWidth();

		int inContCnt = GetDeicePad()->GetInContrain().getCount();
		int outContCnt = GetDeicePad()->GetOutConstrain().getCount();
		CPoint2008 retPt;
		if(inContCnt > 0 ){
			if( pTaxi->GetPath().GetSnapPoint(width, GetDeicePad()->GetInContrain().getPoint(0), retPt ) )
				GetDeicePad()->m_inContrain[0] = retPt; 
		}

		if(outContCnt > 1 ){
			if( pTaxi->GetPath().GetSnapPoint(width, GetDeicePad()->GetOutConstrain().getPoint(outContCnt-1), retPt ) )
				GetDeicePad()->m_outConstrain[outContCnt-1] = retPt; 
		}

	}

}

void CDeicePad3D::RenderEditPoint( C3DView * pView )
{
	//glLoadName( pView->GetSelectionMap().NewSelectable( (m_pServePoint).get() ) );
	//m_pServePoint->DrawSelect(pView);

	for(ALTObjectControlPoint2008List::iterator itr = m_vInConstPoint.begin();itr!=m_vInConstPoint.end();itr++){
		glLoadName( pView->GetSelectionMap().NewSelectable( (*itr).get() ) );
		(*itr)->DrawSelect(pView);		 
	}
	
	if(!GetDeicePad()->IsUsePushBack())
	for(ALTObjectControlPoint2008List::iterator itr = m_vOutConstPoint.begin();itr!=m_vOutConstPoint.end();itr++){
		glLoadName( pView->GetSelectionMap().NewSelectable( (*itr).get() ) );
		(*itr)->DrawSelect(pView);		 
	}

}

void CDeicePad3D::OnRotate( DistanceUnit dx )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
}

//void CDeicePad3D::UpdateIntersectNodes( const ALTObject3DList& otherObjList )
//{
//	ALTObjectList vAllObject = otherObjList.GetObjectList();
//	GetDeicePad()->UpdateIntersectNodes(vAllObject);
//}