// Dlg3DObjectProperty.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "Dlg3DObjectProperty.h"
#include ".\dlg3dobjectproperty.h"

#include <Common/ARCVector.h>


// CDlg3DObjectProperty dialog

IMPLEMENT_DYNAMIC(CDlg3DObjectProperty, CDialog)
CDlg3DObjectProperty::CDlg3DObjectProperty(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_3D_OBJECT_PROP/*CDlg3DObjectProperty::IDD*/, pParent)
	, m_pListener(NULL)
{
}

CDlg3DObjectProperty::~CDlg3DObjectProperty()
{
}

void CDlg3DObjectProperty::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlg3DObjectProperty, CDialog)
	ON_COMMAND(ID_APPLY, OnApply)
END_MESSAGE_MAP()


// CDlg3DObjectProperty message handlers

BOOL CDlg3DObjectProperty::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			OnApply();
			return TRUE;
		}
		else if (pMsg->wParam == VK_ESCAPE)
		{
			Reload3DObject();
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CDlg3DObjectProperty::Load3DObject( C3DNodeObject _3dObject )
{
	m_3dObject = _3dObject;
	if (m_3dObject && !m_3dObject.IsGroup())
	{
		ARCVector3 pos = m_3dObject.GetPosition();
		SetContent(IDC_EDIT_POS_X, pos.x);
		SetContent(IDC_EDIT_POS_Y, pos.y);
		SetContent(IDC_EDIT_POS_Z, pos.z);
		ARCVector3 rot = m_3dObject.GetRotation();
		SetContent(IDC_EDIT_ROT_X, rot.x, _T("бу"));
		SetContent(IDC_EDIT_ROT_Y, rot.y, _T("бу"));
		SetContent(IDC_EDIT_ROT_Z, rot.z, _T("бу"));
		ARCVector3 scale = m_3dObject.GetScale();
		SetContent(IDC_EDIT_SCALE_X, scale.x);
		SetContent(IDC_EDIT_SCALE_Y, scale.y);
		SetContent(IDC_EDIT_SCALE_Z, scale.z);
	}
	else
	{
		ClearContent(IDC_EDIT_POS_X);
		ClearContent(IDC_EDIT_POS_Y);
		ClearContent(IDC_EDIT_POS_Z);
		ClearContent(IDC_EDIT_ROT_X);
		ClearContent(IDC_EDIT_ROT_Y);
		ClearContent(IDC_EDIT_ROT_Z);
		ClearContent(IDC_EDIT_SCALE_X);
		ClearContent(IDC_EDIT_SCALE_Y);
		ClearContent(IDC_EDIT_SCALE_Z);
	}
}

void CDlg3DObjectProperty::CommitChanges()
{
	if (m_3dObject)
	{
		m_3dObject.SetPosition(ARCVector3(
			RetrieveContent(IDC_EDIT_POS_X),
			RetrieveContent(IDC_EDIT_POS_Y),
			RetrieveContent(IDC_EDIT_POS_Z)
			));
		m_3dObject.SetRotation(ARCVector3(
			RetrieveContent(IDC_EDIT_ROT_X),
			RetrieveContent(IDC_EDIT_ROT_Y),
			RetrieveContent(IDC_EDIT_ROT_Z)
			));
		m_3dObject.SetScale(ARCVector3(
			RetrieveContent(IDC_EDIT_SCALE_X),
			RetrieveContent(IDC_EDIT_SCALE_Y),
			RetrieveContent(IDC_EDIT_SCALE_Z)
			));
	}
}

void CDlg3DObjectProperty::Reload3DObject()
{
	Load3DObject(m_3dObject);
}

void CDlg3DObjectProperty::Update3DObjectView()
{
	
}

void CDlg3DObjectProperty::SetContent( int nControlID, double dValue, LPCTSTR strUnit /*= NULL*/ )
{
	CString strText;
	strText.Format(_T("%.3f"), dValue);
	if (strUnit)
	{
		strText += strUnit;
	}
	CWnd* pControl = GetDlgItem(nControlID);
	if (pControl)
	{
		pControl->EnableWindow(TRUE);
		pControl->SetWindowText(strText);
	}
	else
		ASSERT(FALSE);
}

void CDlg3DObjectProperty::ClearContent( int nControlID )
{
	CWnd* pControl = GetDlgItem(nControlID);
	if (pControl)
	{
		pControl->EnableWindow(FALSE);
		pControl->SetWindowText(_T(""));
	}
	else
		ASSERT(FALSE);
}

double CDlg3DObjectProperty::RetrieveContent( int nControlID )
{
	CWnd* pControl = GetDlgItem(nControlID);
	if (pControl)
	{
		CString strText;
		pControl->GetWindowText(strText);
		LPCTSTR lpText = strText;
		int nIndex = 0;
		bool bDotOccur = false;
		while (true)
		{
			TCHAR c = *(lpText + nIndex);
			if ('\x00' == c)
				break;
			if ((c>='0' && c<='9'))
			{
				nIndex++;
				continue;
			}
			if (c == '.')
			{
				if (false == bDotOccur)
				{
					bDotOccur = true;
					nIndex++;
					continue;
				}
			}
			strText == strText.Left(nIndex);
			break;
		}
		return atof(strText);
	}
	else
		ASSERT(FALSE);
	return 0.0;
}

void CDlg3DObjectProperty::On3DNodeObjectFired( C3DNodeObject selNodeObj )
{
	Load3DObject(selNodeObj);
}
void CDlg3DObjectProperty::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	OnApply();
	CDialog::OnOK();
}

void CDlg3DObjectProperty::OnApply()
{
	// TODO: Add your specialized code here and/or call the base class
	CommitChanges();
	if (m_pListener)
	{
		m_pListener->On3DObjectPropertyUpdate();
	}
	Reload3DObject();
}
