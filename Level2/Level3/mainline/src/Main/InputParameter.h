// InputParameter.h: interface for the CInputParameter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUTPARAMETER_H__706E2C6D_ECAD_456D_AF5C_4E5125DD43D8__INCLUDED_)
#define AFX_INPUTPARAMETER_H__706E2C6D_ECAD_456D_AF5C_4E5125DD43D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "ModelManager.h"
#include "ReportManager2.h"

class CInputParameter  
{
public:
	CInputParameter();
	virtual ~CInputParameter();

//	void SetModelManager(const CModelManager& modelManager) { m_modelManager = modelManager; }
//	const CModelManager& GetModelManager() const { return m_modelManager; }

	void SetReportManager(const CReportManager2& reportManager) { m_reportManager = reportManager; }
	const CReportManager2& GetReportManager() const { return m_reportManager; }

	BOOL LoadData( const CString& strPath );
	BOOL Save( const CString& strPath );
private:
//	CModelManager m_modelManager;
	CReportManager2 m_reportManager;
};

#endif // !defined(AFX_INPUTPARAMETER_H__706E2C6D_ECAD_456D_AF5C_4E5125DD43D8__INCLUDED_)
