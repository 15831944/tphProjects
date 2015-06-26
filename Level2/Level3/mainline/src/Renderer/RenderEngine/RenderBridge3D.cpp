#include "stdafx.h"
#include ".\renderbridge3d.h"

#include "Render3DScene.h"
#include <CommonData/ProcessorProp.h>
#include <CommonData/ExtraProcProp.h>
#include "ShapeBuilder.h"
#include <common/ARCStringConvert.h>
#define GLASS_MAT _T("GlassMat")

enum DrawSqureCode
{
	draw_all,
	draw_topbottom,
	draw_side,
};
using namespace Ogre;

static void DrawSqare(ManualObject* pobj,const String& matName, Real dwidth, Real dheight, const Vector3& vFrom,const Vector3& vTo, DrawSqureCode bCode = draw_all )
{
	Vector3 vdir = (vTo - vFrom).normalisedCopy();
	Vector2 vdir2 = Vector2(vdir.x,vdir.y).perpendicular() * dwidth*0.5f;
	Vector3 widthdir(vdir2.x,vdir2.y,0);

	Vector3 heightdir = Vector3::UNIT_Z*dheight;
	
	int nVertexStart = 0;

	pobj->begin(matName,RenderOperation::OT_TRIANGLE_LIST);
	
	if(bCode==draw_all || bCode == draw_side)
	{
		//side left
		pobj->position(vFrom-widthdir);pobj->normal(vdir.crossProduct(heightdir).normalisedCopy());
		pobj->position(vTo-widthdir);
		pobj->position(vFrom-widthdir+heightdir);
		pobj->position(vTo-widthdir+heightdir);
		pobj->quad(nVertexStart,nVertexStart+1,nVertexStart+3,nVertexStart+2);
		nVertexStart+=4;
	}
	
	if(bCode == draw_all || bCode == draw_topbottom)//side top
	{
		pobj->position(vFrom-widthdir+heightdir); pobj->normal(Vector3::UNIT_Z);
		pobj->position(vTo-widthdir+heightdir);
		pobj->position(vFrom+widthdir+heightdir);
		pobj->position(vTo+widthdir+heightdir);		
		pobj->quad(nVertexStart,nVertexStart+1,nVertexStart+3,nVertexStart+2);
		nVertexStart+=4;
	}
	
	if(bCode==draw_all || bCode == draw_side)
	{
		//side right
		pobj->position(vFrom+widthdir+heightdir);pobj->normal(vdir.crossProduct(-heightdir).normalisedCopy());
		pobj->position(vTo+widthdir+heightdir);
		pobj->position(vFrom+widthdir);
		pobj->position(vTo+widthdir);
		pobj->quad(nVertexStart,nVertexStart+1,nVertexStart+3,nVertexStart+2);
		nVertexStart+=4;
	}

	if(bCode==draw_all || bCode == draw_topbottom)//side bottom
	{
		pobj->position(vFrom-widthdir); pobj->normal(Vector3::UNIT_Z);
		pobj->position(vTo-widthdir);
		pobj->position(vFrom+widthdir);
		pobj->position(vTo+widthdir);		
		pobj->quad(nVertexStart,nVertexStart+1,nVertexStart+3,nVertexStart+2);
		nVertexStart+=4;
	}
	pobj->end();
}

void DrawConnectorPtr(ManualObject* pobj,const String& shapemat, const Vector3& vFrom,const Vector3& vTo1,Real dwidth,Real dheight,Real dLen)
{
	Vector3 cDir = (vTo1 - vFrom).normalisedCopy();	
	
	Real ConPart = 100+40+20+40+10;	

	DrawSqare(pobj,shapemat, dwidth,dheight,vFrom+cDir*(dLen-ConPart), vFrom+cDir*(dLen-ConPart+100) );
	DrawSqare(pobj,shapemat, dwidth,dheight,vFrom+cDir*(dLen-ConPart+140), vFrom+cDir*(dLen-ConPart+160) );
	DrawSqare(pobj,shapemat, dwidth,dheight,vFrom+cDir*(dLen-ConPart+200), vFrom+cDir*dLen );

	for(double dAccLen = 0;dAccLen<dLen-ConPart;dAccLen+=150)
	{
		DrawSqare(pobj,shapemat,dwidth-10,dheight-5,vFrom+cDir*dAccLen, vFrom+cDir*(dAccLen+10),draw_side);
	}
	
	DrawSqare(pobj,shapemat,dwidth-10,dheight-5,vFrom+cDir*10,vFrom+ cDir*(dLen-ConPart),draw_topbottom );
	
	
	////draw connector side to flight
	//GLfloat color[4] = { 0.4f,0.4f,0.4f,1 };
	//glColor4fv(color);
	//glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,color);

	MaterialPtr subMat = OgreUtil::createOrRetrieveColormaterialNoCull(ARCColor3(102,102,102));
	DrawSqare(pobj,subMat->getName(),dwidth-10,dheight-10,vFrom+cDir*(dLen-ConPart),vFrom+cDir*dLen);	

	//draw blender square	
	DrawSqare(pobj,GLASS_MAT,dwidth-11,dheight-5,vFrom, vFrom+cDir*(dLen-ConPart),draw_side );
}

void CRenderBridge3D::Update3D( CProcessor2Base* pProc )
{
	//CProcessor2Base* pProc = GetProcessor();
	ASSERT(pProc);
	ProcessorProp* pProcProp = pProc->GetProcessorProp();
	ASSERT(pProcProp && pProcProp->getProcessorType() == BridgeConnectorProc);
	ExtraProcProp* pExtraProp = pProcProp->GetExtraProp();
	ASSERT(pExtraProp && pExtraProp->GetPropType() == ExtraProcProp::BridgeConnector_Prop_Type);
	BridgeConnectorProp* pConnector = (BridgeConnectorProp*)pExtraProp;
	const CProcessor2Base::CProcDispProperties& pdp = pProc->m_dispProperties;

	ARCVector3 center = GetWorldPosition();
	Vector3 centerOffset(-center.x,-center.y,0);

	if (!pdp.bDisplay[PDP_DISP_SHAPE])
	{
		RemoveFromParent();
	}


	const static Real dheight = 250.0f;
	if(pdp.bDisplay[PDP_DISP_SHAPE])
	{
		//pObj->clear();
		
		MaterialPtr pmat = OgreUtil::createOrRetrieveColormaterialNoCull(pdp.color[PDP_DISP_SHAPE]);

		//for(size_t ii=0;ii<pConnector->m_vConnectPoints.size();ii++)
		{
			CString sName  = GetName()+_T("/Shape") ;//+ ARCStringConvert::toString((int)ii);
			ManualObject* pObj = _GetOrCreateManualObject(sName); //OgreUtil::createOrRetrieveManualObject(sName,GetScene() );
			pObj->clear();

			const BridgeConnectorProp::ConnectPoint& pConectPoint =  pConnector->m_connectPoint;//[ii];	

			Vector3 vFrom = Vector3(pConectPoint.m_Location.getX(),pConectPoint.m_Location.getY(),0) + centerOffset;
			Vector3 vTo = Vector3(pConectPoint.m_dirFrom.getX(),pConectPoint.m_dirFrom.getY(),0) + centerOffset;

			
			DrawConnectorPtr(pObj,pmat->getName(),vFrom,vTo,pConectPoint.m_dWidth,dheight,pConectPoint.m_dLength);			
			AddObject(pObj,true);		
		}		
		
	}

}

void CRenderBridge3D::UpdateSelected( bool b )
{

}


void CRenderBridge3D::DrawConnector( int idx, bool bMoving, const CPoint2008& ptFrom, const CPoint2008& ptTo,double dwidth,const ARCColor3& c )
{

	ASSERT(GetParent());
	if(idx>=(int)m_conState.size())
	{
		m_conState.resize(idx+1,_empty);
	}
 
	bool bRedraw = false;
	if(bMoving )
	{
		m_conState[idx]=_moving;
		bRedraw = true;			
	}
	else if( m_conState[idx] != _static)
	{
		m_conState[idx]=_static;
		bRedraw = true;			
	}
	if(!bRedraw)return;

	ARCVector3 center = GetWorldPosition();
	Vector3 centerOffset(-center.x,-center.y,0);

	const static Real dheight = 250.0f;
	Vector3 vFrom = Vector3(ptFrom.getX(),ptFrom.getY(),ptFrom.getZ()) + centerOffset;
	Vector3 vTo = Vector3(ptTo.getX(),ptTo.getY(),ptTo.getZ())+centerOffset;	

	MaterialPtr pmat = OgreUtil::createOrRetrieveColormaterialNoCull(c);

	CString sName  = GetName()+_T("/Shape") + ARCStringConvert::toString(idx);
	ManualObject* pObj = _GetOrCreateManualObject(sName); //OgreUtil::createOrRetrieveManualObject(sName,GetScene() );
	pObj->clear();
	DrawConnectorPtr(pObj,pmat->getName(),vFrom,vTo,dwidth,dheight,(ptTo-ptFrom).Length() );	

	AddObject(pObj,true);
}




void CRenderBridge3D::Setup3D( CProcessor2Base* pProc, int idx )
{
	ARCVector3 center = pProc->GetLocation();
	SetPosition(center);
	Update3D(pProc);
	UpdateNoShape(pProc,DRAWALL);
}
