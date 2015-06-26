#pragma once

class CTermPlanApp
{
public:
	UINT GetClipboardFormat();
	UINT GetProfileInt( LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault );

	BOOL WriteProfileInt( LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue );
};
