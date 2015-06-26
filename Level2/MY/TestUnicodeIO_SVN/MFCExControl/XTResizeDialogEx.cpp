#include "StdAfx.h"
#include ".\xtresizedialogex.h"


CXTResizeDialogEx::CXTResizeDialogEx( ResizeStyleEx st )
	: m_eStyle(st)
{

}

CXTResizeDialogEx::CXTResizeDialogEx( ResizeStyleEx st, const UINT nID, CWnd* pParent /*= 0*/, const UINT nFlags /*= 0*/ )
	: CXTResizeDialog(nID, pParent, nFlags)
	, m_eStyle(st)
{

}

CXTResizeDialogEx::CXTResizeDialogEx( ResizeStyleEx st, LPCTSTR lpszTemplateName, CWnd* pParent /*= 0*/, const UINT nFlags /*= 0*/ )
	: CXTResizeDialog(lpszTemplateName, pParent, nFlags)
	, m_eStyle(st)
{

}

CXTResizeDialogEx::~CXTResizeDialogEx(void)
{
}

BOOL CXTResizeDialogEx::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	CRect rcWindow;
	GetClientRect(rcWindow);
	m_szWindowInitialized = rcWindow.Size();
	return TRUE;
}

void CXTResizeDialogEx::Init()
{
	CXTResizeDialog::Init();
	if (styleUseInitSizeAsMinSize == m_eStyle)
	{
		m_szMin = m_szWindowInitialized;
	}
}