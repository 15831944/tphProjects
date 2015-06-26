#include "StdAfx.h"
#include ".\mouse3dstate.h"
#include "3DSceneNode.h"
#include "ShapeBuilder.h"
#include "MaterialDef.h"
#include "Landside\LandsideRoundabout.h"
#include "Lines3D.h"
#include "3DScene.h"
#include "Render3DScene.h"
#include "Common\WinMsg.h"
#include "LandsideStretch3D.h"
#include "CommonData\QueryData.h"
#include "SceneState.h"
#include "SceneNodeQueryData.h"
#include "Landside\LandsideQueryData.h"
#include "Common\IARCportLayoutEditContext.h"
#include "Landside\ILandsideEditContext.h"

using namespace Ogre;
class RenderArrowNode : public C3DSceneNode
{
	SCENE_NODE(RenderArrowNode,C3DSceneNode)
public:
	virtual void Update3D(const Vector3& pos1, const Vector3& pos2){
		ManualObject* pObj = _GetOrCreateManualObject(GetName());
		pObj->clear();
		OgrePainter painter;
		painter.SetMat(VERTEXCOLOR_LIGHTOFF_NOCULL);
		painter.UseColor(ColourValue::Red);
		CShapeBuilder::DrawArrowPath(pObj,pos1,pos2,painter);
		AddObject(pObj);
	}

	void Hide(){ this->RemoveFromParent(); }
	void Show(){ this->AddToRootNode(); }

};
//////////////////////////////////////////////////////////////////////////
#define ModelPickLine _T("ModelPickLine")
class R3DScenePickPoints : public IRenderSceneMouseState
{
public:
	R3DScenePickPoints(int nPtCount=-1):m_nMaxPtCount(nPtCount){}

	virtual void OnMouseMove(CRender3DScene& scene,const MouseSceneInfo& mouseInfo);
	virtual void OnPick(CRender3DScene& scene,const MouseSceneInfo& mouseInfo);
	virtual void OnLastPick(CRender3DScene& scene,const MouseSceneInfo& mouseInfo);
	virtual CString getHintText()const;
	virtual R3DScenePickPoints* toPickPoints(){ return this; }

protected:
	virtual void OnDoneMouseOp(CRender3DScene& scene);

	void Update3DLine(CRender3DScene& scene,const ARCPath3& path)
	{
		CLines3D pickline = scene.GetSceneNode(ModelPickLine);
		pickline.Update(path);	
		pickline.AttachToRoot();
	}
	void Hide3DLine(CRender3DScene& scene)
	{
		CLines3D pickline = scene.GetSceneNode(ModelPickLine);
		pickline.Detach();
	}
	ARCPath3 m_vMousePosList;
	size_t m_nMaxPtCount; ///-1 means infinite
};
//distance measure
class R3DSceneDistanceMeasure : public R3DScenePickPoints
{
public:	
	virtual void OnDoneMouseOp(CRender3DScene& scene);
	virtual CString getHintText()const{ return "Distance Measurement"; }
	virtual R3DSceneDistanceMeasure* toDistanceMeasureState(){ return this; }	
};

//place marker
class R3DScenePlaceMarker : public IRenderSceneMouseState
{
public:
	virtual CString getHintText()const{ return "Place alignment marker"; }
	virtual R3DScenePlaceMarker* toPlaceMaker(){ return this; }
};

//landside link intersection node
#include "landside/LandsideIntersectionNode.h"
class ILandsideEditContext;
class R3DSceneIntersectionLinkStretch : public IRenderSceneMouseState
{
public:
	virtual void OnMouseMove(CRender3DScene& scene,const MouseSceneInfo& mouseInfo);
	virtual void OnPick(CRender3DScene& scene,const MouseSceneInfo& mouseInfo);
	virtual void OnLastPick(CRender3DScene& scene,const MouseSceneInfo& mouseInfo);
	virtual R3DSceneIntersectionLinkStretch* toLinkstretch(){ return this; }
	virtual CString getHintText()const{ return "Link the Stretch"; }

protected:
	virtual void OnDoneMouseOp(CRender3DScene& scene);
	bool PickLaneLinkNode(ILandsideEditContext* pLCtx,const LaneNode& pLaneNode );
	bool IsLaneLinkageValid(ILandsideEditContext* pLCtx);
protected:
	std::vector<LaneLinkage> vNewLinkags;
	LaneLinkage m_curLinkage;
};

//landside select lane nodes
class R3DSceneSelectLaneNodes : public IRenderSceneMouseState
{
public:
	virtual void OnPick(CRender3DScene& scene,const MouseSceneInfo& mouseInfo);
	virtual void OnLastPick(CRender3DScene& scene,const MouseSceneInfo& mouseInfo){
		OnPick(scene,mouseInfo);
		OnDoneMouseOp(scene);
	}
	R3DSceneSelectLaneNodes* toSelectLaneNodes(){ return this; }
	virtual void OnDoneMouseOp(CRender3DScene& scene)
	{
		::SendMessage(m_hFallbackWnd,TP_DATA_BACK,(WPARAM)&m_vPickNodes,m_lFallbackParam);
		scene.UpdateDrawing();
		scene.EndMouseOperation();					
	}
	virtual CString getHintText()const{ return "Pick the Lane Nodes"; }
protected:
	void PickParkingLotLaneNode( const LaneNode& node )
	{
		if( std::find(m_vPickNodes.begin(),m_vPickNodes.end(),node)==m_vPickNodes.end() )
            m_vPickNodes.push_back(node);
	}

	std::vector<LaneNode> m_vPickNodes;

};
//landside pick stretch position
class R3DScenePickStretchPos : public IRenderSceneMouseState
{
public:
	virtual void OnPick(CRender3DScene& scene,const MouseSceneInfo& mouseInfo)
	{
		LandsideStretch3D objNode = scene.GetSel3DNodeObj(mouseInfo);
		if(objNode.IsRealType())
		{					
			QueryData qdata = *objNode.getQueryData();
			double dPos = objNode.GetMousePosInStretch(mouseInfo);
			qdata.SetData(KeyPosInStretch,dPos);
			//qdata.SetData(KeyStretchHight, mouseInfo.mWorldPos.z);
			::SendMessage(m_hFallbackWnd,TP_DATA_BACK,(WPARAM)&qdata,m_lFallbackParam);
			scene.UpdateDrawing();
			scene.EndMouseOperation();					
		}
	}
	virtual R3DScenePickStretchPos * toPickStretchPos(){ return this; }
	CString getHintText()const
	{
		return "Pick the Position on Stretch";
	}
};

class R3DScenePickLandsideLayoutObject : public IRenderSceneMouseState
{
public:
	virtual void OnPick(CRender3DScene& scene,const MouseSceneInfo& mouseInfo)
	{
		CLayoutObject3DNode objNode = scene.GetSel3DNodeObj(mouseInfo);
					
		QueryData qdata = *objNode.getQueryData();
		::SendMessage(m_hFallbackWnd,TP_DATA_BACK,(WPARAM)&qdata,m_lFallbackParam);
		scene.UpdateDrawing();
		scene.EndMouseOperation();					
		
	}
	CString getHintText()const
	{
		return "Pick Landside Layout Object";
	}
};
//landside pick stretch two sides
class R3DScenePickStretchSides : public IRenderSceneMouseState
{
public:
	R3DScenePickStretchSides(size_t maxCnt = -1 ){ m_nMaxCount = maxCnt; }
	virtual void OnPick(CRender3DScene& scene,const MouseSceneInfo& mouseInfo)
	{
		StretchSideNode3D side3d = scene.GetSel3DNodeObj(mouseInfo);
		
		if(side3d.IsRealType())
		{
			StretchSideInfo sport;
			if( side3d.getNodeData(sport) )
			{
				
				StretchSide side;
				side.m_pStretch = sport.pStretch;
				side.ntype = sport.mSide;
				AddSide(side);
				if(m_vSides.size()>=m_nMaxCount)
				{
					DonePick(scene);
				}
			}
		}
	}
	virtual void OnLastPick(CRender3DScene& scene,const MouseSceneInfo& mouseInfo)
	{
		OnPick(scene,mouseInfo);		
		DonePick(scene);
	}

	void DonePick(CRender3DScene& scene)
	{
		Hide3DLine(scene);
		::SendMessage(m_hFallbackWnd,TP_DATA_BACK,(WPARAM)&m_vSides,m_lFallbackParam);
		scene.EndMouseOperation();
	}
	virtual R3DScenePickStretchSides* toPickStretchSides(){ return this; }
		
	CString getHintText()const
	{
		return "Pick Stretch Sides";
	}

	virtual void OnMouseMove(CRender3DScene& scene,const MouseSceneInfo& mouseInfo)
	{
		ILandsideEditContext* pmodel = scene.mpEditor->GetLandsideContext();
		ARCPath3 path;
		path.resize(m_vSides.size()+1);
		for(size_t i=0;i<m_vSides.size();i++){
			CPoint2008 pos1,pos2; ARCVector3 dir;
			if(m_vSides.at(i).GetPosDir(pos1,pos2,dir)){
				CPoint2008 ptc = (pos2+pos1)*0.5;
				if(pmodel){
					pmodel->ConvertToVisualPos(ptc);
				}
				path[i] = ptc;
			}
		}
		if(!path.empty())
			path.back() = ARCVECTOR(mouseInfo.mWorldPos);
		Update3DLine(scene,path);
	}

	void Update3DLine(CRender3DScene& scene,const ARCPath3& path)
	{
		CLines3D pickline = scene.GetSceneNode(ModelPickLine);
		pickline.Update(path);	
		pickline.AttachToRoot();
	}
	void Hide3DLine(CRender3DScene& scene)
	{
		CLines3D pickline = scene.GetSceneNode(ModelPickLine);
		pickline.Detach();
	}
protected:
	
	void AddSide(StretchSide& info)
	{
		bool bAlreadyPikced = (std::find(m_vSides.begin(),m_vSides.end(),info)!=m_vSides.end() );
		if(bAlreadyPikced)
		{
			return;
		}
		m_vSides.push_back(info);
	}
	std::vector<StretchSide> m_vSides;
	size_t m_nMaxCount;
};

void R3DScenePickPoints::OnMouseMove( CRender3DScene& scene,const MouseSceneInfo& mouseInfo )
{
	ARCPath3 activePath=m_vMousePosList;
	ARCVector3 vecPos = ARCVECTOR(mouseInfo.mWorldPos);
	activePath.push_back(vecPos);

	Update3DLine(scene,activePath);	
	scene.UpdateDrawing();
	
}

void R3DScenePickPoints::OnPick( CRender3DScene& scene,const MouseSceneInfo& mouseInfo )
{
	ARCVector3 vecPos = ARCVECTOR(mouseInfo.mWorldPos);
	m_vMousePosList.push_back(vecPos);
	if(m_vMousePosList.size()>=m_nMaxPtCount)
	{
		OnDoneMouseOp(scene);
	}
}

void R3DScenePickPoints::OnLastPick( CRender3DScene& scene ,const MouseSceneInfo& mouseInfo )
{
	ARCVector3 vecPos = ARCVECTOR(mouseInfo.mWorldPos);
	m_vMousePosList.push_back(vecPos);

	if(m_vMousePosList.size()<2 && m_nMaxPtCount==-1)
	{
		AfxMessageBox(_T("At least Pick two Points"));
		m_vMousePosList.clear();
		return;
	}
	
	OnDoneMouseOp(scene);
}

void R3DScenePickPoints::OnDoneMouseOp( CRender3DScene& scene )
{
	::SendMessage(m_hFallbackWnd, TP_DATA_BACK, (WPARAM)&m_vMousePosList, m_lFallbackParam);
	Hide3DLine(scene);
	scene.UpdateDrawing();
	scene.EndMouseOperation();
}

CString R3DScenePickPoints::getHintText()const
{
	switch(m_nMaxPtCount)
	{
	case 1:
		return "Select a coordinate";
	case 2:
		return "Select two points";
	default:
		return "Select points, RC when done";		
	}
}
//////////////////////////////////////////////////////////////////////////
void R3DSceneDistanceMeasure::OnDoneMouseOp( CRender3DScene& scene )
{
	double dLen = m_vMousePosList.GetLength();
	::SendMessage(m_hFallbackWnd, TP_DATA_BACK, (WPARAM)&dLen, m_lFallbackParam);
	Hide3DLine(scene);
	scene.UpdateDrawing();
	scene.EndMouseOperation();
}
//////////////////////////////////////////////////////////////////////////
#include "Landside/ILandsideEditContext.h"
#include "Landside\InputLandside.h"
#include "landside\LandsideIntersectionNode.h"
#include "LandsideStretch3D.h"



void R3DSceneIntersectionLinkStretch::OnPick( CRender3DScene& scene,const MouseSceneInfo& mouseInfo )
{
	ILandsideEditContext* pLCtx = scene.GetEditContext()->toLandside();
	LaneLinkageNode3D node = scene.GetSel3DNodeObj(mouseInfo);
	if(node.IsRealType())
	{			
		LaneNode laneNode;
		if(node.getNodeData(laneNode)) 
		{
			PickLaneLinkNode(pLCtx,laneNode);
		}			
	}
}

void R3DSceneIntersectionLinkStretch::OnLastPick( CRender3DScene& scene,const MouseSceneInfo& mouseInfo )
{
	OnPick(scene,mouseInfo);
	OnDoneMouseOp(scene);

}



void R3DSceneIntersectionLinkStretch::OnDoneMouseOp( CRender3DScene& scene )
{
	RenderArrowNode node = scene.GetSceneNode(_T("PickIntersectionArrow"));
	node.Hide();
	::SendMessage(m_hFallbackWnd,TP_DATA_BACK,(WPARAM)&vNewLinkags,m_lFallbackParam);		
	scene.UpdateDrawing();
	scene.EndMouseOperation();
}


bool R3DSceneIntersectionLinkStretch::PickLaneLinkNode( ILandsideEditContext* pLCtx,const LaneNode& pLaneNode )
{
	if (!m_curLinkage.mLaneEntry.IsValid() && !m_curLinkage.mLaneExit.IsValid())
	{
		if (pLaneNode.IsLaneEntry())
		{
			m_curLinkage.mLaneEntry = pLaneNode;
			return false;
		} 
		else
		{
			m_curLinkage.mLaneExit = pLaneNode;
			return false;
		}
	}
	if (m_curLinkage.mLaneEntry.IsValid())
	{
		if (pLaneNode.IsLaneExit())
		{
			m_curLinkage.mLaneExit = pLaneNode;
			if(IsLaneLinkageValid(pLCtx))
			{
				vNewLinkags.push_back(m_curLinkage);
				m_curLinkage = LaneLinkage();		
				return true;
			}
		} 
		else
		{
			MessageBox(NULL,_T("Must link same direction stretch ?not opposing direction stretch!"),"Warning",MB_OK);
			return false;
		}
	} 
	else
	{
		if (pLaneNode.IsLaneEntry())
		{
			m_curLinkage.mLaneEntry = pLaneNode;
			if (IsLaneLinkageValid(pLCtx))
			{
				vNewLinkags.push_back(m_curLinkage);
				m_curLinkage = LaneLinkage();
				return true;
			}
		} 
		else
		{
			MessageBox(NULL,_T("Must link same direction stretch ?not opposing direction stretch!"),"Warning",MB_OK);
			return false;
		}
	}
	return false;
}

bool R3DSceneIntersectionLinkStretch::IsLaneLinkageValid( ILandsideEditContext* pLCtx )
{
	LandsideFacilityLayoutObjectList& objlist = pLCtx->getInput()->getObjectList();
	for(int i=0;i<objlist.getCount();i++)
	{
		LandsideFacilityLayoutObject* pobj = objlist.getObject(i);
		if(pobj->GetType() == ALT_LINTERSECTION)
		{
			LandsideIntersectionNode* pNode = (LandsideIntersectionNode*)pobj;
			if(pNode->HasLinkage(m_curLinkage))
				return false;
		}
	}
	if(std::find(vNewLinkags.begin(),vNewLinkags.end(),m_curLinkage)!=vNewLinkags.end())
	{
		return false;
	}
	return true;
}

void R3DSceneIntersectionLinkStretch::OnMouseMove( CRender3DScene& scene,const MouseSceneInfo& mouseInfo )
{
	RenderArrowNode node = scene.GetSceneNode(_T("PickIntersectionArrow"));
	node.Hide();
	if( m_curLinkage.mLaneExit.IsValid() )
	{
		CPoint2008 pos;ARCVector3 dir;
		if( m_curLinkage.mLaneExit.GetPosDir(pos,dir) )
		{
			scene.mpEditor->GetLandsideContext()->ConvertToVisualPos(pos);
			node.Update3D(OGREVECTOR(pos),mouseInfo.mWorldPos);
			node.Show();
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void R3DSceneSelectLaneNodes::OnPick( CRender3DScene& scene,const MouseSceneInfo& mouseInfo )
{
	LaneLinkageNode3D node = scene.GetSel3DNodeObj(mouseInfo);
	if(node.IsRealType())
	{
		LaneNode laneNode;
		if(node.getNodeData(laneNode))
			PickParkingLotLaneNode(laneNode);
		
	}
}


//dynamic create
IRenderSceneMouseState* IRenderSceneMouseState::createState( int nType )
{
	switch(nType)
	{
	case DISTANCE_MEARSURE:
		return new R3DSceneDistanceMeasure();
		break;
	case PICK_ONEPOINT:
		return new R3DScenePickPoints(1);
		break;
	case PICK_TWOPOINTS:
		return new R3DScenePickPoints(2);
		break;
	case PICK_MANYPOINTS:
		return new R3DScenePickPoints();
		break;
	case PLACE_ALIGN_MARKER:
		return new R3DScenePlaceMarker();
		break;
	case PICK_STRETCHSIDE_LIST:
		return new R3DScenePickStretchSides();
		break;
	case PICK_STRETCHSIDE:
		return new R3DScenePickStretchSides(1);
		break;
	case PICK_STRETCH_POS:
		return new R3DScenePickStretchPos();
		break;
	case LINK_STRETCH:
		return new R3DSceneIntersectionLinkStretch();
		break;
	case PICK_PARKLOT_ENTTRY_LANENODE:
		return new R3DSceneSelectLaneNodes();
		break;
	case PICK_ALTOBJECT:
		return new R3DScenePickLandsideLayoutObject();
		break;
	default:
		break;
	}
	return NULL;
}