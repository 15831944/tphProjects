#pragma once
#include "../Common/termfile.h"
#include <vector>

class AirsideFlightStairsLog;

class AirsideFlightStairLogs 
{
public:
	AirsideFlightStairLogs(void);
	~AirsideFlightStairLogs(void);

	void readData (const CString& _csFullFileName);
	void writeData (const CString& _csFullFileName);

	void ClearData();
	bool CreateLogFile( const CString& _csFullFileName);

	void WriteLog(AirsideFlightStairsLog* pLog);

	int  GetItemCount()const;
	AirsideFlightStairsLog* GetItem(int idx);
	void AddItem(AirsideFlightStairsLog* pLog);

protected:
	ArctermFile m_LogFile;
	std::vector<AirsideFlightStairsLog*> m_vLogs;
};
