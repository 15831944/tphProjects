#pragma once
#include "XTResizeDialog.h"

class MFCEXCONTROL_API CXTResizeDialogEx : public CXTResizeDialog
{
public:

	enum ResizeStyleEx
	{
		styleUseInitSizeAsMinSize = 0,
	};

	CXTResizeDialogEx(ResizeStyleEx st);
	CXTResizeDialogEx(ResizeStyleEx st, const UINT nID, CWnd* pParent = 0, const UINT nFlags = 0); // <combine CXTResizeDialog::CXTResizeDialog>
	CXTResizeDialogEx(ResizeStyleEx st, LPCTSTR lpszTemplateName, CWnd* pParent = 0, const UINT nFlags = 0); // <combine CXTResizeDialog::CXTResizeDialog>

	virtual ~CXTResizeDialogEx(void);

	virtual BOOL OnInitDialog();
	virtual void Init();


private:
	ResizeStyleEx m_eStyle;
	CSize m_szWindowInitialized;
};
