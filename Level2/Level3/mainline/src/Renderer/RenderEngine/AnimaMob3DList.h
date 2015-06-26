#pragma once
#include <vector>

template <class _Mob3D>
class CAnimaMob3DList
{
public:
	CAnimaMob3DList(const CString& sType):m_sType(sType){ }
	//~CAnimaMob3DList(void);

	inline _Mob3D* CreateOrRetrieveMob3D(int nMobID, bool& bCreated)
	{
		if((int)m_vMobList.size()<=nMobID)
			m_vMobList.resize(nMobID+1);

		bCreated = false;
		_Mob3D& mob = m_vMobList[nMobID];
		if(!mob)
		{
			bCreated = true;
			CString sID;
			sID.Format(_T("%s%d"), m_sType,nMobID);
			mob = _Mob3D(nMobID,m_rootNode.CreateChild(sID).GetSceneNode());		
		}
		return &mob;
	}

	inline _Mob3D* GetMob3D(int nMobID)
	{
		if(nMobID>=0 && nMobID<(int)m_vMobList.size())
		{
			return &m_vMobList[nMobID];
		}
		return NULL;
	}

	inline void SetRootNode(C3DNodeObject rootNode) { m_rootNode = rootNode; }
	
	inline void UpdateAllMobile( bool bShow ){
		for(size_t i=0;i<m_vMobList.size();i++)
			ShowMob3D(&m_vMobList[i],bShow);
	}
	inline void ShowMob3D( int nMobID, bool bShow )
	{
		if(nMobID<(int)m_vMobList.size())
			ShowMob3D( &m_vMobList[nMobID], bShow);		
	}
	inline void ShowMob3D( _Mob3D* pMob3D, bool bShow ){ bShow ? pMob3D->AttachTo(m_rootNode) : pMob3D->Detach(); }
	inline bool IsMob3DShow( _Mob3D* pMob3D ){ 	return pMob3D->GetParent()/* == m_rootNode*/;	}

	typedef std::vector<_Mob3D> MobList;
protected:
	C3DNodeObject m_rootNode; // hold the root scene node of the scene
	MobList m_vMobList;
	CString m_sType;
};
