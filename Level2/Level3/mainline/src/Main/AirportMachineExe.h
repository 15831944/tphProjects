// AirportMachineExe.h: interface for the CAirportMachineExe class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AIRPORTMACHINEEXE_H__416CB8A5_5105_4071_9CDE_5D89F2F181EC__INCLUDED_)
#define AFX_AIRPORTMACHINEEXE_H__416CB8A5_5105_4071_9CDE_5D89F2F181EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\common\ProjectManager.h"

class CAirportMachineExe  
{
public:
	CAirportMachineExe();
	virtual ~CAirportMachineExe();

	void Run();

private:
	bool GetAirportMachineDataPath(CString& strDataDir);
	bool GetAirportMachineFilePath(CString& strFilePath);
	bool FindFileByEx(LPCTSTR DirName, LPCTSTR FileEx, CString& strFindFile);
};

#endif // !defined(AFX_AIRPORTMACHINEEXE_H__416CB8A5_5105_4071_9CDE_5D89F2F181EC__INCLUDED_)
