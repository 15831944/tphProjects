// SimMobileElemList.cpp: implementation of the CSimMobileElemList class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "SimMobileElemList.h"
#include "../Common/ARCTracker.h"
#include "Movevent.h"
#include "PERSON.H"
#include "GroupLeaderInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSimMobileElemList::CSimMobileElemList()
{
	
	m_nSize = 0;
	m_lliveCount = 0L;
}

CSimMobileElemList::~CSimMobileElemList()
{
	Clear();
}


// allocated space.
void CSimMobileElemList::Initialize( int _nSize )
{
	Clear();

	if( _nSize <= 0 )
		return;
	m_vMobileElementList.resize(_nSize, 0);
	m_nSize = _nSize;
	//m_pMobileElementList = new MobileElement* [m_nSize];
	//for( int i=0; i<m_nSize; i++ )
	//	m_pMobileElementList[i] = NULL;
}

// register MobileElement base on its id as index.
void CSimMobileElemList::Register( MobileElement* _pMob )
{
	long lId = _pMob->getID(); // lId [0..n-1]?
	if( lId >= m_nSize )
		return;

	m_vMobileElementList[lId] = _pMob;
	m_lliveCount += 1;
}


// delete all mobileelement, and delete the list
void CSimMobileElemList::Clear()
{
	PLACE_METHOD_TRACK_STRING();
	//if( !m_pMobileElementList )
	//	return;

	//for( int i=0; i<m_nSize; i++ )
	//{
	//	if( m_pMobileElementList[i] )
	//	{
	//		m_pMobileElementList[i]->M_DELIDX = 0;
	//		delete m_pMobileElementList[i];
	//	}
	//}
	//delete m_pMobileElementList;
	//m_pMobileElementList = NULL;


	std::vector<MobileElement*>::iterator iter = m_vMobileElementList.begin();
	std::vector<MobileElement*>::iterator iterEnd = m_vMobileElementList.begin();
	for (; iter != iterEnd; ++iter)
	{
		delete *iter;
		*iter = NULL;
	}
	m_vMobileElementList.clear();
	m_nSize = 0;
}

void CSimMobileElemList::Unregister( MobileElement *_pMob )
{

	//if (!IsAlive(_pMob))
	//	return;

	long lId = _pMob->getID(); // lId [0..n-1]?
	if( lId >= m_nSize )
		return;

	//TRACE("Unregister Pax : %d /r/n", lId);
	MobileElementMovementEvent::RemoveAllMobileElementEvent(_pMob);

	Person *pPerson = (Person *)_pMob;
	if (pPerson != NULL)
	{
		if(pPerson->m_pGroupInfo)
		{
			if(pPerson->m_pGroupInfo->IsFollower())
			{
				Person *pLeader = pPerson->m_pGroupInfo->GetGroupLeader();
				if(IsAlive(pLeader) && pLeader && pLeader->m_pGroupInfo && !pLeader->m_pGroupInfo->IsFollower())
				{
					((CGroupLeaderInfo *)pLeader->m_pGroupInfo)->RemoveFollower(pPerson);
				}
			}
			else 
			{
				MobileElementList& followList = ((CGroupLeaderInfo *)pPerson->m_pGroupInfo)->GetFollowerList();
				for (int nFollow = 0; nFollow < static_cast<int>(followList.size()); ++ nFollow)
				{
					if((Person *)followList[nFollow] && IsAlive(followList[nFollow]))
					{
						Person *pFollow = (Person *)followList[nFollow];
						//pFollow->m_pGroupInfo->SetGroupLeader(pFollow);
						pFollow->m_pGroupInfo = new CGroupLeaderInfo(1,pFollow);
					}
				}
			}
		}

	}

	delete m_vMobileElementList[lId];

	m_vMobileElementList[lId] = NULL;
	m_lliveCount -= 1;

}

bool CSimMobileElemList::IsAlive( MobileElement *_pMob ) const
{
	if(std::find(m_vMobileElementList.begin(), m_vMobileElementList.end(), _pMob) != m_vMobileElementList.end())
		return true;

	return false;
}

MobileElement* CSimMobileElemList::GetAvailableElement( int nMobileID )
{
	if(nMobileID < 0 || nMobileID >= m_nSize )
		return NULL;

	return m_vMobileElementList[nMobileID];
}





