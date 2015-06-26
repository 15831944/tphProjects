#pragma once
#include "ARCBaseLog.h"
#include <boost/noncopyable.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

template<class LOG_ITEM>
void ARCBaseLog<LOG_ITEM>::Save( const char* descFile ) const
{
	std::ofstream strout(descFile);
	if(!strout)
		return;
	boost::archive::text_oarchive arch(strout);
	arch<<mLogItems;
}


template<class LOG_ITEM>
void ARCBaseLog<LOG_ITEM>::SaveItem( LOG_ITEM& item )
{
	if(!pEventfstream || !(*pEventfstream) )
		return;

	//check if the item is already in
	for(int i=0;i<(int)mLogItems.size();i++)
	{
		LOG_ITEM& _item = mLogItems[i];
		if(item.mpEventList == _item.mpEventList)
		{
			return;
		}
	}
	//write to event file first
	if(item.mpEventList)
	{
		item.mpEventList->Sort();
		item.nStartPos = static_cast<long>(pEventfstream->tellp());
		boost::archive::text_oarchive archLog(*pEventfstream);
		const ARCEventLogList& eventList = *item.mpEventList;
		archLog << eventList ;				
		mLogItems.push_back(item);		
	}
}

template<class LOG_ITEM>
void ARCBaseLog<LOG_ITEM>::SaveObjectItem( LOG_ITEM& item )
{
	mLogItems.push_back(item);
}

template<class LOG_ITEM>
void ARCBaseLog<LOG_ITEM>::LoadItem( LOG_ITEM& item )
{
	if(item.mpEventList)
		return;

	if(!pEventfstream || !(*pEventfstream) )
		return;

	
	delete item.mpEventList; item.mpEventList = NULL;
	item.mpEventList = new ARCEventLogList();
	pEventfstream->seekg(item.nStartPos);
	boost::archive::text_iarchive arch(*pEventfstream);
	arch>>*item.mpEventList;
	item.mpEventList->Sort();			
}

template<class LOG_ITEM>
void ARCBaseLog<LOG_ITEM>::OpenInput( const char* descFile,const char* eventFile )
{
	Clean();
	std::ifstream descFstream(descFile);	
	if(!descFstream)
		return;	

	boost::archive::text_iarchive arch(descFstream);
	arch >> mLogItems;	
	descFstream.close();

	delete pEventfstream;
	pEventfstream = new std::fstream(eventFile,std::ios_base::in);
}

template<class LOG_ITEM>
void ARCBaseLog<LOG_ITEM>::OpenObjectInput( const char* objectFile)
{
	Clean();
	std::ifstream objectFstream(objectFile);	
	if(!objectFstream)
		return;	

	boost::archive::text_iarchive arch(objectFstream);
	arch >> mLogItems;	
	objectFstream.close();
}

