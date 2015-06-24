#pragma once
#include "afxadv.h"

class CArcTermDockState :
	public CDockState
{
public:
	CArcTermDockState(void);
	~CArcTermDockState(void);
public:
	void LoadState(LPCTSTR lpszProfileName) ;
	void SaveState(LPCTSTR lpszProfileName) ;
};
class CArctermControlBarInfo : public CControlBarInfo
{
public:
    CArctermControlBarInfo(void) ;
	virtual ~CArctermControlBarInfo(void) ;
public:
	BOOL LoadState(LPCTSTR lpszProfileName, int nIndex, CDockState* pDockState) ;
    BOOL SaveState(LPCTSTR lpszProfileName, int nIndex) ;
};