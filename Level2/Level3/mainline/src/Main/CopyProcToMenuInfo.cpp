#include "stdafx.h"
#include "TermPlan.h"
#include ".\copyproctomenuinfo.h"
#include "TermPlanDoc.h"



CCopyProcToMenuInfo::CCopyProcToMenuInfo(void)
{
	m_vDocs.clear();
}

CCopyProcToMenuInfo::~CCopyProcToMenuInfo(void)
{
	m_vDocs.clear();
}


int CCopyProcToMenuInfo::GetAppDocuments(std::vector<CTermPlanDoc*>& _vDocs)
{
	m_vDocs.clear();
	_vDocs.clear();
	int iCount = 0;
	CWinApp* pApp = AfxGetApp();
	POSITION pos = pApp->GetFirstDocTemplatePosition();
	CDocTemplate* pTemplate = NULL;
	while (pos)
	{
		pTemplate = pApp->GetNextDocTemplate(pos);
		POSITION posDoc = pTemplate->GetFirstDocPosition();
		CDocument* pDoc = NULL;
		while (posDoc)
		{
			pDoc = pTemplate->GetNextDoc(posDoc);
			if (pDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)))
			{
				m_vDocs.push_back((CTermPlanDoc*)pDoc);
				_vDocs.push_back((CTermPlanDoc*)pDoc);
				iCount++;
			}
		}
	}

	return iCount;
}

int CCopyProcToMenuInfo::GetFloorsByDocument(const CTermPlanDoc* _pDoc, std::vector<CFloor2*>& _vFloors)
{
	_vFloors.clear();
	int iCount = 0;
	ASSERT(_pDoc);

	_vFloors = ((CTermPlanDoc*)_pDoc)->GetCurModeFloor().m_vFloors;
	iCount = _vFloors.size();

	return iCount;
}

BOOL CCopyProcToMenuInfo::GetRegularInfoByMenuID(int _nMenuID, CTermPlanDoc** _ppDoc, int& _nFloorID)
{
	BOOL bFound = FALSE;
	ASSERT(_nMenuID >= 0);
	int nCount = 0, nPrevCount = 0;
	std::vector<CTermPlanDoc*>::iterator iter;
	std::vector<CFloor2*>	vFloors;
	for (iter = m_vDocs.begin(); iter != m_vDocs.end(); iter++)
	{
		nPrevCount = nCount;
		nCount += GetFloorsByDocument(*iter, vFloors);
		if (nCount > _nMenuID)
		{
			*_ppDoc = *iter;
			_nFloorID = _nMenuID - nPrevCount;
			bFound = TRUE;
			break;
		}
	}

	return bFound;
}