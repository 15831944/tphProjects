#include "stdafx.h"
#include "AirsideMultipleRunResult.h"


CAirsideMultipleRunResult::CAirsideMultipleRunResult(void)
{
}


CAirsideMultipleRunResult::~CAirsideMultipleRunResult(void)
{
	ClearResultPath();
}

void CAirsideMultipleRunResult::AddSimResultPath( const CString& strSimResult, const CString& strSimPath )
{
	m_mapResultPath[strSimResult] = strSimPath;
}

void CAirsideMultipleRunResult::ClearResultPath()
{
	m_mapResultPath.clear();
}
