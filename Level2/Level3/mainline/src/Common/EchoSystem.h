// EchoSystem.h: interface for the CEchoSystem class.
//
//////////////////////////////////////////////////////////////////////
/*****
*
*   Class       CEchoSystem
*   Author      Porter ma, Aviation Research Corporation
*   Date        November 24, 2006
*   Purpose     manage about logs,when there are errors in software,it can
*               record all logs or some type log,and save the log information
*               in file.thus developer can solve the error according to these information
*
*****/

#if !defined(AFX_LOGDEBUG_H__730523FB_10E0_4792_9DC9_35ED3ACF7B68__INCLUDED_)
#define AFX_LOGDEBUG_H__730523FB_10E0_4792_9DC9_35ED3ACF7B68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <utility>
#include "Shlwapi.h"
#include "../common/elaptime.h"
#include "../Common/point.h"


#include "LogFile.h"

#define ECHOLOG CEchoSystem::GetInstance()
#define DESTROY_ECHOLOG CEchoSystem::DeleteInstance();


//////////////////////////////////////////////////////////////////////////////
// if you want to add new Log Type, please modify LogTypeCount and add EchoLogType here,
// and don't froget to modify the GetStringByLogType function

enum EchoLogType
{
	LoadProject=0,
	VehicleMoveLog,
	RailWayLog,

	LogTypeCount
};

//////////////////////////////////////////////////////////////////////////////

enum BoolType{ Disable = 0, Enable = 1, FirstLoad = 1, FirstSave = 1 };

class CEchoSystem 
{
public:
	class echoSetting
	{
	public:
		echoSetting()
		{
			echoType = LoadProject;
			bEnable = false;
			nElementID = -1;
		}
		~echoSetting()
		{

		}


	public:
		EchoLogType echoType;
		bool bEnable;
		int nElementID;
	private:
	};
protected:
	static CEchoSystem* m_pInstance;        //the Log instance

	CString m_strProjPath;                //the file's path
	
	bool m_bEnable;                       //stand the whole logs are enable or not

	//std::map<EchoLogType,bool> m_mapLog;      //save the log's type and status(open or close)
	std::vector<echoSetting > m_vLog;
public:
	
	CEchoSystem();
	virtual ~CEchoSystem();
	
	//_bEnable:when write the status of the m_bEnable,control the status 
	//true: write "YES" into file,false:write "NO" into file
	//thus avoid developer rewrite the status of the m_bEnable
	virtual void writeData (CLogFile& p_file,bool _bEnable = false,bool _bFirstLoad = false) const ;
	virtual void readData (CLogFile& p_file) ;
	virtual void clear (void) { m_vLog.clear(); };
	void initDefaultValues(){m_bEnable = false;}

	virtual void loadDataSet (bool _bFirstLoad = false);
	
	// Exception:	DirCreateError;
	virtual void saveDataSet (bool _bEnable = false,bool _bFirstLoad = false) const;

	// Verify the state of log switchs
	virtual bool IsLogSwithcesOn();
  
public:

	//get the map's size
	int GetLogTypeCount() const { return (int)m_vLog.size();}
	//set the log file path
	void SetPath(const CString &strPath);

	std::vector<echoSetting >& GetLogTypeData()
	{
		return m_vLog;
	}

	static CEchoSystem* GetInstance();
	static void DeleteInstance();

	bool IsEnable(); 
	// turn off the master log switch
	void SetMasterLogDisable();
    //get the flag whose type is key
	bool IsLogEnable(EchoLogType key);
	// get the string by the key
	// when you add new log type, please modify the function
	CString GetStringByLogType(EchoLogType key) const;

	int GetLogID(EchoLogType key);

	// write the log file : string	
	BOOL Log(EchoLogType key ,CString content);
	//write the log file  : int 
	BOOL Log(EchoLogType key ,int content);
	// write the log file : long	
	BOOL Log(EchoLogType key ,long content);
	//write the log file  : float
	BOOL Log(EchoLogType key ,float content);
	//write the log file  : ElapsedTime 
	BOOL Log(EchoLogType key ,ElapsedTime content);
	//write the log file  : Point 
	BOOL Log(EchoLogType key ,const Point& aPoint);

};

#endif // !defined(AFX_LOGDEBUG_H__730523FB_10E0_4792_9DC9_35ED3ACF7B68__INCLUDED_)
