#pragma once

#include "../Common/termfile.h"
#include <vector>
class Terminal;


class AirsideFlightDelay;
class CAirsideFlightDelayLog
{
public:

	//open log file, if not exist create one , else clear old data;
	bool CreateLogFile( const CString& _csFullFileName);
	void LoadData(const CString& _csFullFileName);
	void SaveData(const CString& _csFullFileName);


	//write delay to log ,assert log file is open
	void WriteDelay(AirsideFlightDelay* pDelay);


	void ClearData();
	int  GetItemCount()const{ return (int)m_vDelays.size(); }
	AirsideFlightDelay* GetItem(int idx){ return m_vDelays.at(idx); }
	void AddItem(AirsideFlightDelay* pDelay){ m_vDelays.push_back(pDelay); }
protected:
	ArctermFile m_LogFile;
	std::vector<AirsideFlightDelay*> m_vDelays;
	
};
