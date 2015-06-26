#pragma once
#include "common\term_bin.h"
#include "eventlog.h"

class ListMultiEventFile
{
public:
	ListMultiEventFile(const CString& sDefaultName,long lFileID);
	~ListMultiEventFile();

	void SetEventLog(EventLog<MobEventStruct>* pEventlog);
	EventLog<MobEventStruct>* GetEventLog()const {return m_pEventlog;}

	void setEventList( const std::vector<MobEventStruct*>& p_item );
	bool needCreateFile(long lCount)const;
	void openEventFile (const char *p_filename);

	void closeEventFile( void );
	bool hasNext()const;
	ListMultiEventFile* GetNext()const;
	ListMultiEventFile* SetNext();
	CString GetEventFileName()const;
	CString GetNextFileName()const;
	
	long getPositionForWrite (void) const;
	long GetLastFileID()const;
	long GetFileID()const {return m_lFileID;}
private:
	long	m_lFileID;
	CString m_sFileEventName;
	EventLog<MobEventStruct>* m_pEventlog;
	ListMultiEventFile* m_pNextEventLog;
};

class MultiFileEventLog : public EventLog<MobEventStruct>
{
public:
	MultiFileEventLog(void);
	virtual ~MultiFileEventLog(void);

public:
	 virtual void createEventFile (const char *p_filename);
	 virtual void openEventFile (const char *p_filename);
	 virtual void closeEventFile (void);

	 virtual bool needCreateFile(long lCount)const;
	 virtual long GetLastFileID()const;

	 virtual long getPositionForWrite (void) const;
	 virtual EventLog<MobEventStruct>* GetEventLog(long lFile = 0);

	 virtual void setEventList( const std::vector<MobEventStruct*>& p_item );
	
protected:
	void deleteEventFile(const char *p_filename);

protected:
	ListMultiEventFile* m_pMultiEventLog; //front event file 
};


























