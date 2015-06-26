#include "stdafx.h"
#include "3DMovableObject.h"
#include "3DSceneNode.h"



#define USEANY_OBJIMPL _T("objimpl") //this should not be use in other place for the 

using namespace Ogre;

IMovableObjectImpl* C3DMovableObject::getImpl()
{
	if(m_pObject){
		const Any& a = m_pObject->getUserObjectBindings().getUserAny(USEANY_OBJIMPL);
		return any_cast<IMovableObjectImpl*>(a);
	}
	return NULL;
}

void C3DMovableObject::setImpl( IMovableObjectImpl* pImpl )
{
	if(m_pObject){
		Any a(pImpl);
		m_pObject->getUserObjectBindings().setUserAny(USEANY_OBJIMPL, a );
	}	
}

void C3DMovableObject::OnLBDblClk( const MouseSceneInfo& mouseinfo )
{
	if(IMovableObjectImpl* pImpl = getImpl()){
		if(pImpl->OnLBDblClk(m_pObject,mouseinfo)){
			return;
		}
	}
	if( C3DSceneNode nodeObj = m_pObject->getParentSceneNode() ){
		//nodeObj.OnLBDblClk(mouseinfo);
	}	
}
