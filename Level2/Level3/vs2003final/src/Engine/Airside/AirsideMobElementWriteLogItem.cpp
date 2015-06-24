#include "StdAfx.h"
#include ".\airsidemobelementwritelogitem.h"
#include "AirsideMobileElement.h"

AirsideMobElementWriteLogItem::AirsideMobElementWriteLogItem(CAirsideMobileElement *pMobElement)
{
	m_pAirsideElement = pMobElement;
}

AirsideMobElementWriteLogItem::~AirsideMobElementWriteLogItem(void)
{

}

//void AirsideMobElementWriteLogItem::AddTimePosition( const AirsideMobElementWriteLogItem::TimePosition& timePos )
//{
//	m_vTimePosition.push_back(timePos);
//}
//
//void AirsideMobElementWriteLogItem::SetState( int nState )
//{
//	m_state = nState;
//}

void AirsideMobElementWriteLogItem::Execute()
{
	if (m_pAirsideElement)
		m_pAirsideElement->WritePureLog(this);
}

//int AirsideMobElementWriteLogItem::GetTimePositionCount() const
//{
//	return static_cast<int>(m_vTimePosition.size());
//}
//
//AirsideMobElementWriteLogItem::TimePosition AirsideMobElementWriteLogItem::GetTimePosition( int nIndex ) const
//{
//	ASSERT(nIndex >= 0 && nIndex < static_cast<int>(m_vTimePosition.size()));
//	return m_vTimePosition.at(nIndex);
//}
//
//int AirsideMobElementWriteLogItem::GetState() const
//{
//	return m_state;
//}

//////////////////////////////////////////////////////////////////////////
//AirsideMobElementWriteLogItem::TimePosition
//AirsideMobElementWriteLogItem::TimePosition::TimePosition( const ElapsedTime& eTime,const Point& ptPosition )
//{
//	m_eTime = eTime;
//	m_ptPosition = ptPosition;
//}
//
//AirsideMobElementWriteLogItem::TimePosition::~TimePosition()
//{
//
//}
//
//ElapsedTime AirsideMobElementWriteLogItem::TimePosition::GetTime() const
//{
//	return m_eTime;
//}
//
//Point AirsideMobElementWriteLogItem::TimePosition::GetPosition() const
//{
//	return m_ptPosition;
//}


//////////////////////////////////////////////////////////////////////////
//AirsideMobElementWriteLogItemList
AirsideMobElementWriteLogItemList::AirsideMobElementWriteLogItemList()
{

}

AirsideMobElementWriteLogItemList::~AirsideMobElementWriteLogItemList()
{

}

void AirsideMobElementWriteLogItemList::AddItem( AirsideMobElementWriteLogItem* item )
{
	m_itemList.push_back(item);
}
AirsideMobElementWriteLogItem* AirsideMobElementWriteLogItemList::GetItem()
{
	AirsideMobElementWriteLogItem* pRetItem = NULL;

	if (m_itemList.size() > 0)
	{
		pRetItem = *m_itemList.begin();
		m_itemList.erase(m_itemList.begin());
	}

 return pRetItem;
}

int AirsideMobElementWriteLogItemList::GetItemCount()
{
	return static_cast<int>(m_itemList.size());
}

//////////////////////////////////////////////////////////////////////////
///
AirsideFlightWriteLogItem::AirsideFlightWriteLogItem(CAirsideMobileElement *pMobElement)
:AirsideMobElementWriteLogItem(pMobElement)
{

}

AirsideFlightWriteLogItem::~AirsideFlightWriteLogItem()
{

}

CAirsideVehicleWriteLogItem::CAirsideVehicleWriteLogItem(CAirsideMobileElement *pMobElement)
:AirsideMobElementWriteLogItem(pMobElement)
{

}

CAirsideVehicleWriteLogItem::~CAirsideVehicleWriteLogItem()
{

}