// DlgExport3DView.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgExport3DView.h"
#include ".\dlgexport3dview.h"

#include "TermPlan.h"

static const LPCTSTR sc_strSupportedImageFileFormat =
_T("Bitmap Files (*.bmp)|*.bmp|")
// _T("Wireless Bitmap Files (*.wbmp)|*.wbmp|")
_T("JPEG (*.jpg;*.jpeg)|*.jpg;*.jpeg|")
_T("JPEG 2000(*.jp2)|*.jp2|")
//_T("(*.jpe)|*.jpe|")
// _T("GIF (*.gif)|*.gif|")
_T("TIFF (*.tiff;*.tif)|*.tiff;*.tif|")
// _T("IFF (*.iff)|*.iff|")
_T("TARGA (*.tga)|*.tga|")
_T("PNG (*.png)|*.png|")
// _T("PCX (*.pcx)|*.pcx|")
// _T("PSD (*.psd)|*.psd|")
// _T("SGI (*.sgi)|*.sgi|")
// _T("RAS (*.ras)|*.ras|")
_T("All Files (*.*)|*.*||")
;

// CDlgExport3DView dialog

IMPLEMENT_DYNAMIC(CDlgExport3DView, CDialog)

CDlgExport3DView::CDlgExport3DView( const CSize& viewSize, const CSize& maxSize, CWnd* pParent /*= NULL*/ )
	: CDialog(CDlgExport3DView::IDD, pParent)
	, m_nViewWidth(viewSize.cx)
	, m_nViewHeight(viewSize.cy)
	, m_nMaxWidth(maxSize.cx)
	, m_nMaxHeight(maxSize.cy)
{

}
CDlgExport3DView::~CDlgExport3DView()
{
}

void CDlgExport3DView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPIN_WIDTH, m_spnWidth);
	DDX_Control(pDX, IDC_SPIN_HEIGHT, m_spnHeight);
	DDX_Control(pDX, IDC_CHECK_LOCK_RATIO, m_btnLockRatio);
}


BEGIN_MESSAGE_MAP(CDlgExport3DView, CDialog)
	ON_BN_CLICKED(IDC_CHECK_LOCK_RATIO, OnBnClickedCheckLockRatio)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_WIDTH, OnDeltaposSpinWidth)
	ON_EN_CHANGE(IDC_EDIT_WIDTH, OnEnChangeEditWidth)
END_MESSAGE_MAP()


// CDlgExport3DView message handlers

BOOL CDlgExport3DView::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_spnWidth.SetBuddy(GetDlgItem(IDC_EDIT_WIDTH));
	m_spnHeight.SetBuddy(GetDlgItem(IDC_EDIT_HEIGHT));
	m_spnWidth.SetRange32(1, m_nMaxWidth);
	m_spnHeight.SetRange32(1, m_nMaxHeight);

	CTermPlanApp* pTermApp = (CTermPlanApp*)AfxGetApp();
	m_spnWidth.SetPos32(pTermApp->GetProfileInt(_T("Snapshot History"), _T("Width"), m_nMaxWidth));
	m_spnHeight.SetPos32(pTermApp->GetProfileInt(_T("Snapshot History"), _T("Height"), m_nMaxHeight));

	bool bLockRatio = _T("true") == pTermApp->GetProfileString(_T("Snapshot History"), _T("Lock Ratio"), _T("true"));
	m_btnLockRatio.SetCheck(bLockRatio ? BST_CHECKED : BST_UNCHECKED);
	SetLockRatioChecked(bLockRatio);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgExport3DView::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	m_nWidth = m_spnWidth.GetPos32();
	m_nHeight = m_spnHeight.GetPos32();

	TCHAR path[MAX_PATH] = { 0 };
	VERIFY(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL|CSIDL_FLAG_CREATE, NULL, 0, path)));
	CTermPlanApp* pTermApp = (CTermPlanApp*)AfxGetApp();
	CString strDir = pTermApp->GetProfileString(_T("Snapshot History"), _T("Directory"), path);
	CString strExt = pTermApp->GetProfileString(_T("Snapshot History"), _T("Format"), _T("bmp"));

	CFileDialog fileDlg(FALSE, strExt, NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, sc_strSupportedImageFileFormat,NULL, 0, FALSE
		);
	fileDlg.m_ofn.lpstrInitialDir = strDir;
	
	if(fileDlg.DoModal() == IDOK) 
	{
		m_strFileName = fileDlg.GetPathName();
		int nFound = m_strFileName.ReverseFind('\\');
		if (nFound == -1)
			nFound = m_strFileName.ReverseFind('/');
		strDir = nFound == -1 ? _T("") : m_strFileName.Left(nFound);

		nFound = m_strFileName.ReverseFind('.');
		if (nFound != -1)
			strExt = m_strFileName.Right(m_strFileName.GetLength() - nFound - 1);
		

		pTermApp->WriteProfileInt(_T("Snapshot History"), _T("Width"), m_nWidth);
		pTermApp->WriteProfileInt(_T("Snapshot History"), _T("Height"), m_nHeight);
		pTermApp->WriteProfileString(_T("Snapshot History"), _T("Directory"), strDir);
		pTermApp->WriteProfileString(_T("Snapshot History"), _T("Format"), strExt);
		pTermApp->WriteProfileString(_T("Snapshot History"), _T("Lock Ratio"), m_btnLockRatio.GetCheck() == BST_CHECKED ? _T("true") : _T("false"));
		CDialog::OnOK();
	}
}

void CDlgExport3DView::OnBnClickedCheckLockRatio()
{
	// TODO: Add your control notification handler code here
	bool bChecked = BST_CHECKED == m_btnLockRatio.GetCheck();
	SetLockRatioChecked(bChecked);
}

void CDlgExport3DView::SetLockRatioChecked(bool bChecked)
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_EDIT_HEIGHT)->EnableWindow(!bChecked);
	m_spnHeight.EnableWindow(!bChecked);
	UpdateHeightInput(bChecked);
	
}

void CDlgExport3DView::OnDeltaposSpinWidth(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	UpdateHeightInput(m_btnLockRatio.GetCheck() == BST_CHECKED);
	*pResult = 0;
}

void CDlgExport3DView::UpdateHeightInput( bool bChecked )
{
	if (bChecked)
	{
		int nRecommendedHeight = m_nViewHeight*m_spnWidth.GetPos32()/m_nViewWidth;
		m_spnHeight.SetPos32(nRecommendedHeight);
	}
}
void CDlgExport3DView::OnEnChangeEditWidth()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if (m_btnLockRatio.GetSafeHwnd())
	{
		UpdateHeightInput(m_btnLockRatio.GetCheck() == BST_CHECKED);
	}
}
