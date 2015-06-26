#include "StdAfx.h"
#include "arclogitem.h"
#include ".\ARCEventLog.h"
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

BOOST_CLASS_VERSION(ARCLogItem,1)

void ARCLogItem::serialize(boost::archive::text_iarchive &ar, const unsigned int file_version)
{
	ar & nStartPos;
	ar & startTime;
	ar & endTime;
}

void ARCLogItem::serialize(boost::archive::text_oarchive &ar, const unsigned int file_version)
{
	ar & nStartPos;
	ar & startTime;
	ar & endTime;
}

void ARCLogItem::ClearEvents()
{
	delete mpEventList; mpEventList = NULL;
}

void ARCLogItem::AddEvent( ARCEventLog* pEvent )
{
	if(!mpEventList)
	{
		mpEventList = new ARCEventLogList();
		startTime = pEvent->time;
		endTime = pEvent->time;
	}
	else
	{
		startTime = min(startTime,pEvent->time);
		endTime = max(startTime,pEvent->time);
	}
	mpEventList->addEvent(pEvent);
}

ARCLogItem::ARCLogItem( void )
{
	mpEventList=NULL;
	nStartPos = -1;
	startTime = 0;
	endTime = 0;
}

ARCLogItem::~ARCLogItem( void )
{
	//do nothing the event list should clear by other class 
	mpEventList = NULL;
}

BOOST_CLASS_VERSION(AirsideALTObjectLogItem,1)
AirsideALTObjectLogItem::AirsideALTObjectLogItem()
{

}

AirsideALTObjectLogItem::~AirsideALTObjectLogItem()
{

}

void AirsideALTObjectLogItem::ClearEvents()
{

}



void AirsideALTObjectLogItem::serialize(boost::archive::text_iarchive &ar, const unsigned int file_version)
{
	 	std::string strObject;
	 	ar & strObject;
	 	m_altObject.FromString(strObject.c_str());
	 	ar & m_emType;
}

void AirsideALTObjectLogItem::serialize(boost::archive::text_oarchive &ar, const unsigned int file_version)
{
	 	std::string strObject = m_altObject.GetIDString();
	 	ar & strObject;
	 	ar & m_emType;
}

void AirsideALTObjectLogItem::AddEvent( ARCEventLog* pEvent )
{

}
