#pragma once
#include "common\term_bin.h"
#include "eventlog.h"


class TempEventFile 
{
	friend class TempMobileEventLog;
public:
	TempEventFile(const CString& sDefaultName,long lFileID);
	~TempEventFile();

	void SetEventLog(EventLog<MobEventStruct>* pEventlog){m_pEventlog = pEventlog;}
	EventLog<MobEventStruct>* GetEventLog()const {return m_pEventlog;}

	//void setEventList( const std::vector<MobEventStruct*>& p_item );
	//bool needCreateFile(long lCount)const;
	//void openEventFile (const char *p_filename);

	void closeEventFile( void );
	//bool hasNext()const;
	//ListMultiEventFile* GetNext()const;
	//ListMultiEventFile* SetNext();
	//CString GetEventFileName()const;
	//CString GetNextFileName()const;

	//long getPositionForWrite (void) const;
	//long GetLastFileID()const;
	//long GetFileID()const {return m_lFileID;}

	void createEventFile (const char *p_filename);

protected:
	void ClearEventLog();
	CString GetFileName()const;
	void openEventFile( const char *p_filename );
	void addEvent( MobEventStruct *pEvent );
	void FlushLog();
private:
	long	m_lFileID;
	CString m_sFileEventName;
	EventLog<MobEventStruct>* m_pEventlog;

	std::vector<MobEventStruct *> m_vEvent;

private:
	size_t m_nReserveSize;
};



//temporary mobile element event log
class TempMobileEventLog : public EventLog<MobEventStruct>
{
public:
	TempMobileEventLog(void);
	virtual ~TempMobileEventLog(void);

public:
	void addEvent(MobEventStruct *pEvent);
	virtual void createEventFile (const char *p_filename);
	virtual void openEventFile (const char *p_filename);
	virtual void closeEventFile (void);
	int getFileCount();

	EventLog<MobEventStruct>* GetEventLog(int nFile);

	int getPaxPerFile() const{return m_nPaxPerFile;}
protected:
	TempEventFile *GetFile(int nFileID);
	void FlushLog();
	void deleteEventFile(const char *p_filename);

protected:
	std::vector<TempEventFile *> m_vFiles;

private:
	int m_nPaxPerFile;
	//default event name
	CString m_strDefaultName;

};