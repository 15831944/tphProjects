#include "StdAfx.h"
#include ".\paxsimscene.h"
#include "SceneNodeQueryData.h"
#include "SceneState.h"
using namespace Ogre;

PaxSimScene::PaxSimScene(void)
{
}

PaxSimScene::~PaxSimScene(void)
{
}



#define SX "X"
#define SY "Y"
#define STYPE "type"
#define MAT_BLOCK "BlockMat"
#define MAT_PASS "PassMat"

class SpaceCell3D : public CRenderScene3DNode
{
public:
	SCENENODE_CONVERSION_DEF(SpaceCell3D,CRenderScene3DNode)
	PROTOTYPE_DECLARE(SpaceCell3D);

	virtual void OnLButtonDblClk( const MouseSceneInfo& mouseinfo )
	{
		FlipState();
	}

	virtual void OnMouseMove(const MouseSceneInfo& mouseinfo)
	{
		if(mouseinfo.mnFlags&MK_LBUTTON )
		{
			SetType(0);
		}
		if(mouseinfo.mnFlags&MK_RBUTTON)
		{
			SetType(1);
		}
	}

	void SetXY(int x, int y)
	{
		if(SceneNodeQueryData* pData = InitQueryData(true) )
		{
			pData->SetData(SX,x);
			pData->SetData(SY,y);
		}
	}

	void SetType(int t)
	{
		if(SceneNodeQueryData* pData = InitQueryData(true) )
		{			
			pData->SetData(STYPE,t);		
			if(t>0)
			{
				SetMaterial(MAT_BLOCK);
			}
			else
			{
				SetMaterial(MAT_PASS);
			}
		}
	}
	int GetType()const
	{
		int t = 0; 
		if(SceneNodeQueryData* pData = getQueryData() )
		{
			pData->GetData(STYPE,t);
		}
		return t;
	}

	int GetX()const
	{
		int x = 0; 
		if(SceneNodeQueryData* pData = getQueryData() )
		{
			pData->GetData(SX,x);
		}
		return x;

	}
	int GetY()const
	{
		int x = 0; 
		if(SceneNodeQueryData* pData = getQueryData() )
		{
			pData->GetData(SY,x);
		}
		return x;
	}

	void FlipState()
	{
		int nType= GetType();
		nType = !nType;
		SetType(nType);		
	}

	void Update3D(int x, int y, double dsize)
	{
		float dHfsize = dsize*0.5f;
		ManualObject* pobj = OgreUtil::createOrRetrieveManualObject(GetIDName(),GetScene());
		pobj->clear();
		pobj->begin("",RenderOperation::OT_TRIANGLE_FAN);
		pobj->position(-dHfsize,-dHfsize,0);
		pobj->position(dHfsize,-dHfsize,0);
		pobj->position(dHfsize,dHfsize,0);
		pobj->position(-dHfsize,dHfsize,0);
		pobj->end();
		AddObject(pobj,1);

		SetXY(x,y);
		SetPosition( ARCVector3(x*dsize + 0.5*dsize, y*dsize+0.5*dsize,0) );
	}

};

PROTYPE_INSTANCE(SpaceCell3D)

class SpaceGrid3D : public CRenderScene3DNode
{
public:
	SCENENODE_CONVERSION_DEF(SpaceGrid3D,CRenderScene3DNode)
	PROTOTYPE_DECLARE(SpaceGrid3D);
	SpaceCell3D GetCell(int x, int y)
	{
		CString str;
		str.Format("Cell%dx%d",x,y);
		return OgreUtil::createOrRetrieveSceneNode(str,GetScene());
	}
	
	
	

	void Update(int width, int height, float cellsize)
	{
		int dHfW = width/2;
		int dHfH = height/2;
		for(int i=0;i<width;i++)
		{
			for(int j=0; j<height;j++)
			{				
				SpaceCell3D cell3d = GetCell(i,j);
				cell3d.Update3D(i,j,cellsize);				
				cell3d.AttachTo(*this);
				cell3d.SetType(0);
			}
		}
	}

	

};
PROTYPE_INSTANCE(SpaceGrid3D)

void PaxSimScene::Setup()
{
	SpaceGrid3D grid3D = OgreUtil::createOrRetrieveSceneNode("Grid3D",getSceneManager());
	grid3D.Update(100,20,10);
	grid3D.AttachTo(GetRoot());

}

BOOL PaxSimScene::OnLButtonDblClk( const MouseSceneInfo& mouseInfo )
{
	if(!mouseInfo.mOnObject)
		return FALSE;

	CRenderScene3DNode node = mouseInfo.mOnObject->getParentSceneNode();
	if(node)
	{
		node.OnLButtonDblClk(mouseInfo);
	}
	return TRUE;
}

BOOL PaxSimScene::OnMouseMove( const MouseSceneInfo& mouseInfo )
{
	if(mouseInfo.mOnObject)
	{
		CRenderScene3DNode node = mouseInfo.mOnObject->getParentSceneNode();
		if(node)
		{
			node.OnMouseMove(mouseInfo);
		}
	}
	return TRUE;
}