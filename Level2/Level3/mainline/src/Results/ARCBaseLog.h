#pragma once
#include <fstream>
#include "ARCEventLog.h"
#include "ARCLogItem.h"


template<class LOG_ITEM>
class ARCBaseLog : boost::noncopyable
{
public:
	typedef LOG_ITEM LogItem;

	ARCBaseLog(void){ pEventfstream = NULL; }
	~ARCBaseLog(void){ delete pEventfstream; Clean(); }
	

	//log entries
	int getItemCount()const{ return (int)mLogItems.size(); }
	LogItem& ItemAt(int idx){ return mLogItems[idx]; }
	const LogItem& ItemAt(int idx)const{ return mLogItems[idx]; }



	//input functions
	//open and read data
	void OpenInput(const char* descFile,const char* eventFile);

	void OpenObjectInput(const char* objectFile);
	
	//load item events throw exceptions, if it is already load will no effect
	void LoadItem(LOG_ITEM& item);
	

	//out put functions
	//open and ready for write
	void OpenOutput(const char* evetFile);
	//flush item events to file
	void SaveItem(LOG_ITEM& item);

	//flush altobject item
	void SaveObjectItem(LOG_ITEM& item);
	
	//flush all logs to file
	void Save(const char* descFile)const;
	
	//clean up all log items and its events
	void Clean(){
		for(int i=0;i<getItemCount();i++)
			ItemAt(i).ClearEvents();
		mLogItems.clear();
	}

	//
	std::fstream* pEventfstream;//io stream for the events log
	std::vector< LogItem >  mLogItems;

};

template<class LOG_ITEM>
void ARCBaseLog<LOG_ITEM>::OpenOutput( const char* evetFile )
{
	Clean();
	delete pEventfstream;
	pEventfstream = new std::fstream(evetFile,std::ios_base::out);
}
