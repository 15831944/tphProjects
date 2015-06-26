#pragma once
#include "../Common/elaptime.h"
#include "../Common/termfile.h"
#include <vector>

class OnboardDoorLog
{
public:
	OnboardDoorLog(void);
	~OnboardDoorLog(void);

	void SetFlightID(const CString& sFlightID);
	const CString& GetFlightID()const;

	void SetDoorID(int nDoorID);
	int GetDoorID()const;

	void SetStartTime(const ElapsedTime& tTime);
	const ElapsedTime& GetStartTime()const;

	void SetEndTime(const ElapsedTime& tTime);
	const ElapsedTime& GetEndTime()const;

	void load(ArctermFile& outFile);
	void write(ArctermFile& outFile);
private:
	CString		m_sFlightID;
	int			m_nDoorID;
	ElapsedTime m_tStartTime;
	ElapsedTime m_tEndTime;
};

class OnboardDoorsLog
{
public:
	OnboardDoorsLog(void);
	~OnboardDoorsLog(void);

	void readData (const CString& _csFullFileName);
	void writeData (const CString& _csFullFileName);

	void ClearData();
	bool CreateLogFile( const CString& _csFullFileName);

	void WriteLog(OnboardDoorLog* pLog);

	int  GetItemCount()const;
	OnboardDoorLog* GetItem(int idx);
	void AddItem(OnboardDoorLog* pLog);

protected:
	ArctermFile m_LogFile;
	std::vector<OnboardDoorLog*> m_vLogs;

};