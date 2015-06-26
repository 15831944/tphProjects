#include "StdAfx.h"
#include ".\pathedit3dnode.h"
#include <Renderer/RenderEngine/ogreutil.h>
#include <renderer/RenderEngine/SelectionManager.h>
#include <common/ARCStringConvert.h>
#include <InputOnBoard/Seat.h>
#include "./AircraftLayoutScene.h"
#include <inputonboard/AircraftLayoutEditContext.h>
#include <inputonboard/AircaftLayOut.h>
#include <InputOnBoard/Seat.h>
#include <InputOnBoard/AircraftPlacements.h>
#include <boost/bind.hpp>
#include ".\materialdef.h"
#include "CustomizedRenderQueue.h"
#include "ShapeBuilder.h"

#define MatVertexColorLtOn _T("VertexColorLightOn")
#define MatVertexColorLtOff _T("VertexColorLightOff")

#define OBJECT_Z_OFFSET     1.0

using namespace Ogre;

void CEditPoint3DNode::Update( EditPointKey ptKey, double dSize ,const Ogre::ColourValue& c)
{
	if(!GetSceneNode()){ ASSERT(FALSE); return; }

	m_nidx = ptKey;

	Ogre::ManualObject* pobj = GetPointObject();
	if(pobj)
	{
		double ptSize = dSize;
		pobj->clear();
		pobj->begin(MatVertexColorLtOn,RenderOperation::OT_TRIANGLE_FAN );
		pobj->position(-ptSize,-ptSize,OBJECT_Z_OFFSET); pobj->colour(c);
		pobj->normal(0,0,1);
		pobj->position(ptSize,-ptSize,OBJECT_Z_OFFSET);
		pobj->position(ptSize,ptSize,OBJECT_Z_OFFSET);
		pobj->position(-ptSize,ptSize,OBJECT_Z_OFFSET);
		pobj->end();
		AddObject(pobj,1);
		pobj->setRenderQueueGroup(RenderObject_EditHelper);		
	}
}

Ogre::ManualObject* CEditPoint3DNode::GetPointObject()
{
	return OgreUtil::createOrRetrieveManualObject(GetIDName(),GetScene());
}

void CEditMultiPath3DNode::Update()
{
	UpdateLines();
	UpdateEditPoints();
}

void CEditMultiPath3DNode::UpdateEditPoints()
{
	if(!GetSceneNode()){ ASSERT(FALSE); return; }

	int nPointCount = 0;
	int nPathCount = GetPathCount();
	for(int i=0;i<nPathCount;i++)
	{
		if (GetPathConfig(i).bShowNode)
		{
			nPointCount += GetPathConfig(i).path.getCount();
		}
	}

	int nPoint3DCount = GetCount();
	if( nPoint3DCount<nPointCount)
	{
		for(int i=nPoint3DCount;i<nPointCount;i++)
		{				
			CEditPoint3DNode* pNewNode = new CEditPoint3DNode(GetPtNode(i));			
			mvNodeList.push_back( NodeTypePtr(pNewNode) );
		}		
	}
	else if( nPoint3DCount>nPointCount)
		mvNodeList.resize(nPointCount);

	std::for_each(mvNodeList.begin(), mvNodeList.end(), boost::BOOST_BIND(&C3DNodeObject::AttachTo, _1, *this));

	int nNodeIndex = 0;
	for(int i=0;i<nPathCount;i++)
	{
		const EditPathConfig& pathConfig = GetPathConfig(i);
		ColourValue c = OGRECOLOR(pathConfig.clr);
		int nPointCountInPath = pathConfig.path.getCount();
		for(int j=0;j<nPointCountInPath;j++)
		{
			if (pathConfig.bShowNode)
			{
				NodeTypePtr node = GetItem(nNodeIndex);
				ASSERT(node.get());
				ARCVector3 pt = pathConfig.path.getPoint(j); pt.z=0;
				node->Update(EditPointKey(i, j), pathConfig.ptSize, c);
				node->SetPosition(pt);
				nNodeIndex++;
			}
		}
	}
}

void CEditMultiPath3DNode::Clear()
{
	int nPathCount = GetPathCount();
	for (int i=0;i<nPathCount;i++)
	{
		Ogre::ManualObject* pLine = GetPathLine(i);
		pLine->clear();
	}
	mvNodeList.clear();
}

Ogre::SceneNode* CEditMultiPath3DNode::GetPtNode( int nNodeIndex )
{
	CString strID;
	strID.Format(_T("%s/Pt%d"), GetIDName(), nNodeIndex);
	return OgreUtil::createOrRetrieveSceneNode(strID, GetScene());
}

Ogre::ManualObject* CEditMultiPath3DNode::GetPathLine(int nPathIndex)
{
	CString strName;
	strName.Format(_T("%s/Ln%d"), GetIDName(), nPathIndex);
	return OgreUtil::createOrRetrieveManualObject(strName, GetScene());
}



void CEditMultiPath3DNode::Init(int nPathCount)
{
	m_pathConfigs.resize(nPathCount);
	std::for_each(m_pathConfigs.begin(), m_pathConfigs.end(), std::mem_fun_ref(&EditPathConfig::Init));
}

bool CEditMultiPath3DNode::OnDrag( Ogre::MovableObject* pobj, const ARCVector3& offset, EditPointKey& ptKey )
{
	for(int i=0;i<GetCount();i++)
	{
		NodeTypePtr ptr = GetItem(i);
		if(ptr->HasMovableObj(pobj))
		{
			ptKey = ptr->getKey();
			CPoint2008& ptPos = GetPathConfig(ptKey.nPathIndex).path[ptKey.nNodeIndex];
			ptPos += CPoint2008(offset[VX],offset[VY],offset[VZ]);ptPos.setZ(0);
			ptr->SetPosition(ptPos);			
			UpdateLine(ptKey.nPathIndex);
			return true;
		}
	}
	
	return false;
}

bool CEditMultiPath3DNode::HitTest( Ogre::MovableObject* pobj, int& nPathIndex, int& nNodeIndex )
{
	if (HasMovableObj(pobj, false))
	{
		nPathIndex = any_cast<int>(pobj->getUserAny());
		nNodeIndex = -1;
		return true;
	}
	for(int i=0;i<GetCount();i++)
	{
		NodeTypePtr ptr = GetItem(i);
		if(ptr->HasMovableObj(pobj, false))
		{
			EditPointKey ptKey = ptr->getKey();
			nPathIndex = ptKey.nPathIndex;
			nNodeIndex = ptKey.nNodeIndex;
			return true;
		}
	}

	return false;
}

void CEditMultiPath3DNode::UpdateLines()
{
	mpNode->detachAllObjects();
	int nCount = GetPathCount();
	for(int i=0;i<nCount;i++)
	{
		UpdateLine(i);
	}
}

void CEditMultiPath3DNode::UpdateLine(int nPathIndex)
{
	if(!GetSceneNode()){ ASSERT(FALSE); return; }
	Ogre::ManualObject* pLine = GetPathLine(nPathIndex);
	if(pLine)
	{
		const EditPathConfig& pathConfig = GetPathConfig(nPathIndex);
		ColourValue c = OGRECOLOR(pathConfig.clr);

		const CPath2008& path = pathConfig.path;
		ProcessorBuilder::DrawColorPath(pLine, ColourValue(c.r, c.g, c.b, c.a), path, 6.0, pathConfig.bLoop, OBJECT_Z_OFFSET);

		pLine->clear();
		pLine->begin(MatVertexColorLtOff,RenderOperation::OT_LINE_STRIP);
		for(int i=0;i<path.getCount();i++)
		{	
			const CPoint2008& pt = path[i];//.getPoint(i);
			pLine->position(pt.getX(),pt.getY(),OBJECT_Z_OFFSET);
			pLine->colour(c);
		}
		if(pathConfig.bLoop && path.getCount())
		{
			const CPoint2008& pt = path[0];
			pLine->position(pt.getX(),pt.getY(),OBJECT_Z_OFFSET);
		}
		pLine->end();
		AddObject(pLine,1);
		pLine->setRenderQueueGroup(RenderObject_EditHelper);
		pLine->setUserAny(Any(nPathIndex));
		
	}
}

EditPathConfig& CEditMultiPath3DNode::GetPathConfig( int nPathIndex )
{
	ASSERT(nPathIndex>=0 && nPathIndex<GetPathCount());
	return m_pathConfigs[nPathIndex];
}

const EditPathConfig& CEditMultiPath3DNode::GetPathConfig( int nPathIndex ) const
{
	ASSERT(nPathIndex>=0 && nPathIndex<GetPathCount());
	return m_pathConfigs[nPathIndex];
}

int CEditMultiPath3DNode::GetPathCount() const
{
	return (int)m_pathConfigs.size();
}

void CEditMultiPath3DNode::InitPathConfig( int nPathCount )
{
	m_pathConfigs.clear();
	m_pathConfigs.resize(nPathCount);
}

void CEditMultiPath3DNode::setOnlyPath( const CPath2008& pathIn )
{
	InitPathConfig(1);
	GetPathConfig(0).path = pathIn;
}

void CEditMultiPath3DNode::DoOffset( const ARCVector3& ptOffset )
{
// 	int nPathCount = GetPathCount();
// 	int nNodeIndex = 0;
// 	for(int i=0;i<nPathCount;i++)
// 	{
// 		EditPathConfig& pathConfig = GetPathConfig(i);
// 		pathConfig.path.DoOffset(ptOffset.x, ptOffset.y, ptOffset.z);
// 		int nPointCountInPath = pathConfig.path.getCount();
// 		for(int j=0;j<nPointCountInPath;j++)
// 		{
// 			if (pathConfig.bShowNode)
// 			{
// 				NodeTypePtr node = GetItem(nNodeIndex);
// 				ASSERT(node.get());
// 				node->SetPosition(node->GetPosition() + ptOffset);
// 				nNodeIndex++;
// 			}
// 		}
// 	}
	SetPosition(GetPosition() + ptOffset);
}
///
//void CEditSeatGroupNode::Update( CSeatGroup* pSeatGrp, CAircraftLayoutScene& scene )
//{
//	CString strNode = GetIDName() + _T("editpath");
//	m_pathNode = OgreUtil::createOrRetrieveSceneNode(strNode,GetScene());
//
//	m_pathNode.setPath(pSeatGrp->getPath() );
//	m_pathNode.Update();
//	m_pathNode.AttachTo(*this);
//
//	MovableObject* pLine = m_pathNode.GetPathLine();
//	pLine->setUserAny( Any(pSeatGrp) );
//	for(int i=0;i<pSeatGrp->m_path.getCount();i++)
//	{
//		MovableObject* pPoint = m_pathNode.GetItem(i)->GetPointObject();
//		pPoint->setUserAny( Any(CSeatGroup::PathPointInfo(pSeatGrp,i)) );
//	}
//	
//}
//
//
//void CEditSeatGroupNode::ShowEditPath( BOOL bshow )
//{
//	if(bshow){
//		m_pathNode.Detach();
//	}
//	else{ 
//		m_pathNode.AttachTo(*this);
//	}
//}
//


