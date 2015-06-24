#include "stdafx.h"
#include "TermPlan.h"
#include "DlgALTOException.h"
#include "ALTOException.h"
#include "Version.h"
#include "../Common/ProjectManager.h"
#include "../Common/WinMsg.h"

IMPLEMENT_DYNAMIC(CDlgALTOException, CDialog)
CDlgALTOException::CDlgALTOException(ALTOExceptionMode mode, ALTOException& _Exception, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgALTOException::IDD, pParent)
	, mException( _Exception )
	, mMode( mode )
{

	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 16;
	lf.lfWeight = FW_BOLD;
	strcpy(lf.lfFaceName, "Arial");
	VERIFY(m_fontHeader.CreateFontIndirect(&lf));

	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 14;
	lf.lfWeight = FW_BOLD;
	strcpy(lf.lfFaceName, "Arial");
	VERIFY(m_fontErrorCode.CreateFontIndirect(&lf));


}

CDlgALTOException::~CDlgALTOException()
{
}

void CDlgALTOException::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_ERRORCODE, m_wndErrorCode);
	DDX_Control(pDX, IDC_EXCEPTION_HEADER, m_wndHeader);
}


BEGIN_MESSAGE_MAP(CDlgALTOException, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	//ON_BN_CLICKED(IDC_BUTTON_QUIT, OnBnClickedQuit)
END_MESSAGE_MAP()


// CDlgALTOException message handlers
BOOL CDlgALTOException::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString strTitle;
	strTitle.Format(_T("ARCport ALTOCEF   v%s"), ARCTERM_VERSION_STRING );
	SetWindowText( strTitle );

	m_wndHeader.SetFont(&m_fontHeader);
	m_wndHeader.SetWindowText(_T("An Unexpected Error Occurred"));

	CString strErrorType;
	strErrorType = mMode == DocOpenError ? _T("Load data error : ") : _T("Simulation error: ");

	CString strErrorCode;
	strErrorCode.Format("code: ( %s )", mException.what() );

	m_wndErrorCode.SetFont(&m_fontErrorCode, TRUE);
	m_wndErrorCode.SetWindowText(strErrorType + strErrorCode );

	//((CButton*)GetDlgItem(IDC_RADIO_EXPORT))->SetCheck( TRUE );
	((CButton*)GetDlgItem(IDC_RESTARTAPP))->SetCheck( FALSE );

	if(mMode == DocOpenError )
	{
		GetDlgItem(IDC_RADIO_EXPORT)->ShowWindow( FALSE );
		GetDlgItem(IDC_RADIO_SEND)->ShowWindow( FALSE );
		GetDlgItem(IDC_RESTARTAPP)->ShowWindow( FALSE );

		strErrorCode.Format("code :   ( %s )\r\n\r\n%s", mException.what(),
			_T("Please send project file to ARC support."));

		m_wndErrorCode.SetWindowText(strErrorType + strErrorCode );
	}

	return TRUE;
}

//void CDlgALTOException::OnBnClickedQuit()
//{
//	OnOK();
//	AfxGetMainWnd()->SendMessage(WM_CLOSE);
//}


void CDlgALTOException::OnBnClickedOk()
{
	//int mIgnore = ((CButton*)GetDlgItem(IDC_RADIO_IGNORE))->GetCheck();
	if( mMode == DocOpenError ) 
	{	
		OnOK();
		AfxGetMainWnd()->SendMessage(WM_CRASHCLOSE);
		return;
	}

	int mExportProject = ((CButton*)GetDlgItem(IDC_RADIO_EXPORT))->GetCheck();
	int mSendProject = ((CButton*)GetDlgItem(IDC_RADIO_SEND))->GetCheck();

	OnOK();

	//if(mMode == DocOpenError)// While loading project
	//{
	//	if( mExportProject )
	//		//CMainFrame::OnNoOpenProjectExportAsZipFile
	//		AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_EXPORTPROJECT_ASZIPFILE);	
	//	else if(mSendProject)
	//		//CMainFrame::OnNoOpenProjectExportAsInputAndEmail
	//		AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_EXPORTPROJECT_ASINPUT_EMAIL);
	//}
	//else 
	if(mMode == SimRunningError)//while simulation running. Project loaded. 
	{
		if(mExportProject )
			//CMainFrame::OnProjectExportAsInputzip
			AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_PROJECT_EXPORT_AS_INPUTZIP);		
		else if(mSendProject)
			//CMainFrame::OnProjectExportAndEmail
			AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_PROJECT_EXPORT_AND_EMAIL);
	}

	if (((CButton*)GetDlgItem(IDC_RESTARTAPP))->GetCheck())
	{
		AfxGetMainWnd()->SendMessage(WM_CRASHCLOSE,TRUE);
	}

	AfxGetMainWnd()->SendMessage(WM_CRASHCLOSE,FALSE);

}

