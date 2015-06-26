#include "StdAfx.h"
#include ".\scenenodetomesh.h"
using namespace Ogre;


void Ogre::SceneNodeToMesh::VisitNode( Ogre::SceneNode* pNode )
{
	SceneNode::ObjectIterator itr =	pNode->getAttachedObjectIterator();
	while (itr.hasMoreElements())
	{
		MovableObject* pobj = itr.getNext();
		AddMovable(pobj);
	}
	//cascade
	{
		Node::ChildNodeIterator itr = pNode->getChildIterator();
		while(itr.hasMoreElements())
		{
			SceneNode* pChildNode = (SceneNode*)itr.getNext();	
			VisitNode(pChildNode);			
		}
	}	


	m_ogreMesh->_setBounds(pNode->_getWorldAABB());
	
}

void Ogre::SceneNodeToMesh::AddMovable( Ogre::MovableObject* pObj )
{
	pObj->visitRenderables(this);
}

void Ogre::SceneNodeToMesh::visit( Renderable* rend, ushort lodIndex, bool isDebug, Any* pAny /*= 0*/ )
{
	Matrix4 tanf = Matrix4::IDENTITY;
	rend->getWorldTransforms(&tanf);
	RenderOperation rop;
	rend->getRenderOperation(rop);
	//
	SubMesh* sm = m_ogreMesh->createSubMesh();
	sm->useSharedVertices = false;
	sm->operationType = rop.operationType;

	MaterialPtr mat= rend->getMaterial();
	mMaterials.push_back(mat);
	
	sm->setMaterialName( mat->getName() );
	// Copy vertex data; replicate buffers too
	sm->vertexData = rop.vertexData->clone(true);
	// Copy index data; replicate buffers too; delete the default, old one to avoid memory leaks
	//tanf*();

	VertexBufferBinding* pBind = sm->vertexData->vertexBufferBinding;
	VertexDeclaration* pDeclare = sm->vertexData->vertexDeclaration;
	if(pDeclare)
	{
		const VertexElement * pPosElm = NULL;
		const VertexElement * pNormElm = NULL;

		for(unsigned short i=0;i< pDeclare->getElementCount();i++)
		{
			const VertexElement* pElm = pDeclare->getElement(i);
			if(pElm->getSemantic()==VES_POSITION){ pPosElm = pElm; }
			if(pElm->getSemantic()==VES_NORMAL){ pNormElm = pElm;  }
		}

		Ogre::HardwareVertexBufferSharedPtr pbuf;
		Ogre::HardwareVertexBufferSharedPtr nbuf;
		float* pPos = NULL;
		float* pNorm = NULL;
		if(pPosElm && pPosElm->getType()==VET_FLOAT3)
		{
			pbuf = pBind->getBuffer( pPosElm->getSource() );	
			if(!pbuf.isNull())
			{
				if(pbuf->isLocked())pbuf->unlock();
				pPos = static_cast<float*>(
					pbuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));
			}
		}
		if(pNormElm && pNormElm->getType() ==VET_FLOAT3)
		{
			nbuf = pBind->getBuffer( pNormElm->getSource() );
			if(!nbuf.isNull())
			{
				if(nbuf->isLocked())nbuf->unlock();
				pNorm = static_cast<float*>(
					nbuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));
			}
		}

		if(pPos)
		{
			size_t nVertexCnt = sm->vertexData->vertexCount;
			for(size_t i=0;i<nVertexCnt;i++)
			{
				float * pCurPos = pPos + i* pbuf->getVertexSize()/sizeof(float);
				Vector3 vOrign(*pCurPos,*(pCurPos+1),*(pCurPos+2) );
				Vector3 vDest = tanf*vOrign;
				
				if(pNorm){
					float * pCurNorm = pNorm + (i * nbuf->getVertexSize()+pNormElm->getOffset())/sizeof(float);
					Vector3 vOrignNorm(*pCurNorm,*(pCurNorm+1),*(pCurNorm+2) );
					Vector3 vDestNorm = tanf*(vOrign+vOrignNorm)-vDest;
					vDestNorm.normalise();
					*pCurNorm = vDestNorm.x;pCurNorm++;
					*pCurNorm = vDestNorm.y;pCurNorm++;
					*pCurNorm = vDestNorm.z;pCurNorm++;

				}
				*pCurPos = vDest.x;pCurPos++;
				*pCurPos = vDest.y;pCurPos++;
				*pCurPos = vDest.z;pCurPos++;
			}

		}

		if (tanf.hasNegativeScale())
		{
			MirrorNode(rop,pNorm,sm->vertexData->vertexCount);
			MirrorNode(rop,pPos,sm->vertexData->vertexCount);
		}
		
		if(!pbuf.isNull())
			if(pbuf->isLocked())pbuf->unlock();
		if(!nbuf.isNull())
			if(nbuf->isLocked())nbuf->unlock();
	}

	// check if index data is present
	if (rop.indexData)
	{
		// Copy index data; replicate buffers too; delete the default, old one to avoid memory leaks
		OGRE_DELETE sm->indexData;
		sm->indexData = rop.indexData->clone(true);
		
	}
	
	if (tanf.hasNegativeScale())
	{
		MirrorNode(rop,sm->indexData);
	}
}

bool Ogre::SceneNodeToMesh::MirrorNode(const RenderOperation& rop,IndexData* pData)const
{
	unsigned short *pShort = static_cast<unsigned short*>(
		pData->indexBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD));
	
	if (rop.indexData)
	{
		size_t nStart = 0;
		if (rop.operationType == RenderOperation::OT_TRIANGLE_FAN)
		{
			nStart = 1;
		}
		for (size_t i = nStart, j = pData->indexCount-1; i < j; i++,j--)
		{
			std::swap(pShort[i],pShort[j]);
		}
	}
	if(pData->indexBuffer->isLocked())
		pData->indexBuffer->unlock();
	return true;
}

bool Ogre::SceneNodeToMesh::MirrorNode(const RenderOperation& rop,float * pCurPos,size_t numPoints)const
{
	if (!rop.indexData)
	{
		size_t nStart = 0;
		if (rop.operationType == RenderOperation::OT_TRIANGLE_FAN)
		{
			nStart = 3;
		}

		for (size_t i = nStart, j = numPoints-1; i < j; i+=3, j-=3)
		{
			std::swap(*(pCurPos+i),*(pCurPos+j-2));
			std::swap(*(pCurPos+i+1),*(pCurPos+j-1));
			std::swap(*(pCurPos+i+2),*(pCurPos+j));
		}
	}
	
	return true;
}

void Ogre::SceneNodeToMesh::Init( CString meshPrefix )
{
	szMeshPrefix = meshPrefix.GetString();

	Ogre::MeshManager* pMeshMgr = Ogre::MeshManager::getSingletonPtr();
	assert(pMeshMgr != NULL);

	pMeshMgr->unload(szMeshPrefix);
	pMeshMgr->remove(szMeshPrefix);
	m_ogreMesh = pMeshMgr->createManual(szMeshPrefix,
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	
}

void Ogre::SceneNodeToMesh::Save( CString folder )
{	
	String sFold = folder.GetString();
	//save material
	SaveMaterial(sFold);

	CopyTexures(sFold);

	SaveMesh(sFold);
}

void Ogre::SceneNodeToMesh::SaveMaterial(const String& folder )
{
	Ogre::MaterialSerializer matSer;
	bool bExport  =false;
	for(size_t i=0;i<mMaterials.size();i++)
	{
		MaterialPtr mat = mMaterials[i];
		visiteMaterial(mat);
		matSer.queueForExport(mat);
		bExport = true;
	}
	if(bExport)
		matSer.exportQueued( folder + szMeshPrefix+".material");	
}

void Ogre::SceneNodeToMesh::SaveMesh( const String& folder )
{
	Ogre::MeshSerializer meshSer;	
	meshSer.exportMesh(m_ogreMesh.getPointer(), folder+szMeshPrefix+".mesh");
	Ogre::MeshManager::getSingleton().remove(m_ogreMesh->getHandle());

}

void Ogre::SceneNodeToMesh::CopyTexures( const String& folder )
{
	for(std::map<String,String>::const_iterator itr=mTexSource.begin();itr!=mTexSource.end();++itr)
	{
		const String& texPath = itr->second;
		String destPath = folder+ itr->first;
		if(destPath==texPath)
			continue;
		::CopyFile(texPath.c_str(),destPath.c_str(),false);
	}
}
#include <Shlwapi.h>

static bool FindTextLocation(const TexturePtr& tex, String& path)
{
	if(tex.isNull())
		return false;

	FileInfoListPtr fileInfo =  ResourceGroupManager::getSingleton().findResourceFileInfo(tex->getGroup(),tex->getName());
	if(!fileInfo.isNull() && fileInfo->size())
	{
		FileInfo& info = fileInfo->at(0);		
		path = info.archive->getName() + info.filename;
		return true;
	}
	return false;
	
}
void Ogre::SceneNodeToMesh::visiteMaterial( MaterialPtr& mat )
{

	for(unsigned short i=0;i<mat->getNumTechniques();i++)
	{
		Technique* pTech = mat->getTechnique(i);
		for(unsigned short j =0;j<pTech->getNumPasses();j++)
		{
			Pass*pPass = pTech->getPass(j);
			for(unsigned short k=0;k<pPass->getNumTextureUnitStates();++k)
			{
				TextureUnitState* pState = pPass->getTextureUnitState(k);
				String tex  = pState->getTextureName();
				TexturePtr pTex = TextureManager::getSingleton().getByName(tex);				
				String fullPath;
				if(FindTextLocation(pTex,fullPath))
				{
					mTexSource[tex] = fullPath;
				}				
			}
		}
	}
}